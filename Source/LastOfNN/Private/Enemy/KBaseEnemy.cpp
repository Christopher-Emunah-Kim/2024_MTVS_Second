// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/KBaseEnemy.h"
#include "Enemy/KEnemyFSM.h"
#include "Player/JPlayer.h"
#include <Kismet/GameplayStatics.h>
#include "Components/CapsuleComponent.h"
#include "Enemy/KEnemyAnim.h"
#include "Runtime/AIModule/Classes/AIController.h"
#include "NavigationSystem.h"  
#include "Runtime/AIModule/Classes/Navigation/PathFollowingComponent.h" 
#include "GameFramework/CharacterMovementComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "Engine/World.h"
#include "EngineUtils.h"

// Sets default values
AKBaseEnemy::AKBaseEnemy()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//팀타입 초기화
	TeamType = ETeamType::FRIENDLY;

	//EnemyFSM 컴포넌트 추가
	FSMComponent = CreateDefaultSubobject<UKEnemyFSM>(TEXT("FSM"));

}

// Called when the game starts or when spawned
void AKBaseEnemy::BeginPlay()
{
	Super::BeginPlay();
	
	//이동상태 구현을 위한 target 변수 초기화
	//월드에서 플레이어 액터찾기
	auto FirstPlayer = UGameplayStatics::GetActorOfClass(GetWorld(),AJPlayer::StaticClass());

	//target을 해당 플레이어타입으로 캐스팅
	target = Cast<AJPlayer>(FirstPlayer);
	
	//UKEnemyAnim 할당
	anim = Cast<UKEnemyAnim>(GetMesh()->GetAnimInstance());

	//AAIController 할당
	ai = Cast<AAIController>(GetController());
}

// Called every frame
void AKBaseEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


bool AKBaseEnemy::GetRandomPositionInNavMesh(FVector centerLocation, float radius, FVector& dest)
{
	//내비게이션 시스템 인스턴스를 얻어온다.
	auto ns = UNavigationSystemV1::GetNavigationSystem(GetWorld());
	//객체 멤버함수호출하여 매개변수에 값 전달
	//centerLocation을 기준으로 radius영역 안에 랜덤으로 loc 변수에 담아주는 역할의 함수
	//정상적으로 값이 호출되면 true, 아니면 false반환.
	FNavLocation loc;
	bool result = ns->GetRandomReachablePointInRadius(centerLocation, radius, loc);
	//이 값을 함수의 반환 값으로 사용 후 랜덤함 위치를 dest변수에 할당
	dest = loc.Location;
	
	return result;
}



void AKBaseEnemy::EnemySetState(EEnemyState newstate)
{
	//상태 전환
	FSMComponent->CurrentState = newstate;
	//애니메이션 상태 동기화
	anim->EnemyAnimState = newstate;
}

void AKBaseEnemy::EnemyIDLE()
{
	//시간이 흐르면
	CurrentTime += GetWorld()->DeltaTimeSeconds;
	//UE_LOG(LogTemp, Warning, TEXT("CurrentTime : %f"), CurrentTime);
	//경과시간이 대기시간을 지나면
	if (CurrentTime > IdleDelayTime )
	{
		//UE_LOG(LogTemp, Warning, TEXT("Enemy Move!!!!"));
		//이동상태로 전환/애니메이션 상태 동기화
		EnemySetState(EEnemyState::MOVE);
		//속도를 걷기속도로 설정
		GetCharacterMovement()->MaxWalkSpeed = EnemyRunSpeed;
		//BlendSpace Anim에 액터의 속도 할당
		anim->EnemyVSpeed = FVector::DotProduct(GetActorRightVector(), GetVelocity());
		anim->EnemyHSpeed = FVector::DotProduct(GetActorForwardVector(), GetVelocity());
		//경과시간 초기화
		CurrentTime = 0;

		
		//랜덤위치값 최초설정
		GetRandomPositionInNavMesh(GetActorLocation(), 500, EnemyRandomPos);
	}
}

void AKBaseEnemy::EnemyMove()
{
	
}



void AKBaseEnemy::OnEnemyNoiseHeard(AActor* Actor, FAIStimulus Stimulus)
{
	if ( Stimulus.Type == UAISense::GetSenseID<UAISense_Hearing>() )
	{
		//UE_LOG(LogTemp, Log, TEXT("소리 감지: 위치 - %s, 강도 - %f"), *Stimulus.StimulusLocation.ToString(), Stimulus.Strength);
		
		// 소리 발생 위치와 강도 저장
		FVector NoiseLocation = Stimulus.StimulusLocation; //소리위치
		float Loudness = Stimulus.Strength; //소리강도

		// 소리 강도에 따라 이동 플래그 설정
		if ( Loudness > 100.0f ) // 특정 소리 강도 기준
		{
			UE_LOG(LogTemp, Warning, TEXT("OnEnemyNoiseHeard called with stimulus: Loudness(%f) > 100.0f"), Loudness);

			bShouldMoveToSound = true;
			SoundLocation = NoiseLocation;
		}
	}
}

void AKBaseEnemy::EnemyAttack()
{
	
}

void AKBaseEnemy::EnemySpecialAttack()
{
	
}

void AKBaseEnemy::EnemyGrab()
{
	
}

void AKBaseEnemy::SetAllEnemiesToIdle()
{
	
}

float AKBaseEnemy::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float FinalDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	
	OnEnemyDamageProcess(FinalDamage);

	return FinalDamage;
}

void AKBaseEnemy::OnEnemyDamageProcess(float damage)
{
	//HP가 감소한다.(데미지만큼)
	EnemyHP -= damage;
	//만약 체력이 남았다면
	if (EnemyHP > 0)
	{
		//피격상태 전환
		
		EnemySetState(EEnemyState::TAKEDAMAGE);

		CurrentTime = 0;

		//피격애니메이션 재생
		int32 index = FMath::RandRange(0,1);
		FString SectionName = FString::Printf(TEXT("EnemyTDamage%d"), index);
		anim->PlayEnemyTDamageAnim(FName(*SectionName));
	}
	else
	{
		//죽음상태 전환
		
		EnemySetState(EEnemyState::DEAD);
		//충돌체비활성화
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		//죽음애니메이션 재생
		anim->PlayEnemyTDamageAnim(TEXT("EnemyDie"));
	}
	
	//이땐 AI길찾기 기능 정지시켜두기
	ai->StopMovement();
}

void AKBaseEnemy::EnemyTakeDamage()
{
	
}

void AKBaseEnemy::EnemyExecuted()
{

}

void AKBaseEnemy::EnemyDead()
{
}

