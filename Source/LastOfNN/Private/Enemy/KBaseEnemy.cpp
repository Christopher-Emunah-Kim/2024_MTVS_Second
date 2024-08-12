// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/KBaseEnemy.h"
#include "Enemy/KEnemyFSM.h"
#include "Player/JPlayer.h"
#include <Kismet/GameplayStatics.h>
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "Enemy/KEnemyAnim.h"
#include "Runtime/AIModule/Classes/AIController.h"
#include "NavigationSystem.h"  
#include "Runtime/AIModule/Classes/Navigation/PathFollowingComponent.h" 
#include "GameFramework/CharacterMovementComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Engine/World.h"
#include "Engine/DamageEvents.h"
#include "EngineUtils.h"

// Sets default values
AKBaseEnemy::AKBaseEnemy()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//팀타입 초기화
	TeamType = ETeamType::FRIENDLY;
	//TeamID = FGenericTeamId(1);

	//EnemyFSM 컴포넌트 추가
	FSMComponent = CreateDefaultSubobject<UKEnemyFSM>(TEXT("FSM"));

	//데미지 처리를 위한 충돌체형성
	RightAttackSphere = CreateDefaultSubobject<USphereComponent>(TEXT("RightAttackSphere"));
	//RightAttackSphere->SetSphereRadius(200.f);
	RightAttackSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	//RightAttackSphere->SetCollisionProfileName(TEXT("NoCollision"));
	LeftAttackSphere = CreateDefaultSubobject<USphereComponent>(TEXT("LeftAttackSphere"));
	//LeftAttackSphere->SetSphereRadius(200.f);
	//LeftAttackSphere->SetCollisionProfileName(TEXT("NoCollision"));
	LeftAttackSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
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

	//데미지처리함수 바인딩
	if ( RightAttackSphere )
	{
		RightAttackSphere->OnComponentBeginOverlap.AddDynamic(this, &AKBaseEnemy::EnemyOverlapDamage);
	}
	if ( LeftAttackSphere )
	{
		LeftAttackSphere->OnComponentBeginOverlap.AddDynamic(this, &AKBaseEnemy::EnemyOverlapDamage);
	}
}

// Called every frame
void AKBaseEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//시간이 흘러감에 따라 어그로수치를 계속 줄임.
	FMath::Max(EnemyAttentionDegree - (DeltaTime/120), 0);
}
////Team ID Setting
//FGenericTeamId AKBaseEnemy::GetGenericTeamId() const
//{
//	return TeamID;
//}
////Team Attitude Setting
//ETeamAttitude::Type AKBaseEnemy::GetTeamAttitudeTowards(const AActor& Other) const
//{
//	if ( const IGenericTeamAgentInterface* TeamAgent = Cast<const IGenericTeamAgentInterface>(&Other) )
//	{
//		FGenericTeamId OtherTeamID = TeamAgent->GetGenericTeamId();
//
//		// 팀 ID가 동일하지 않은 경우 적대적 태도 설정
//		if ( OtherTeamID != TeamID )
//		{
//			return ETeamAttitude::Hostile;
//		}
//	}
//
//	return ETeamAttitude::Neutral; // 동일 팀이거나 팀 ID를 알 수 없을 경우 중립
//}

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
	//다음상태가 이동상태가 아니라면 Ai한테 멈추라고 지시한다.
	if ( newstate != EEnemyState::MOVE )
	{
		ai->StopMovement();
	}
}

void AKBaseEnemy::EnemyIDLE()
{
	//시간이 흐르면
	CurrentTime += GetWorld()->DeltaTimeSeconds;

	//경과시간이 대기시간을 지나면
	if (CurrentTime > IdleDelayTime )
	{
		//이동상태로 전환/애니메이션 상태 동기화
		EnemySetState(EEnemyState::MOVE);
		//속도를 걷기속도로 설정
		GetCharacterMovement()->MaxWalkSpeed = EnemyWalkSpeed;
		//BlendSpace Anim에 액터의 속도 할당
		anim->EnemyVSpeed = FVector::DotProduct(GetActorRightVector(), GetVelocity());
		anim->EnemyHSpeed = FVector::DotProduct(GetActorForwardVector(), GetVelocity());
		//경과시간 초기화
		CurrentTime = 0;

		//랜덤위치값 최초설정
		GetRandomPositionInNavMesh(GetActorLocation(), 500, EnemyRandomPos);
	}
}


//감지처리 통합함수(BeginPlay바인딩 목적)
void AKBaseEnemy::OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	if ( FSMComponent->CurrentState != EEnemyState::MOVE )
		return;

	if ( Stimulus.Type == UAISense::GetSenseID<UAISense_Hearing>() )
	{
		OnEnemyNoiseHeard(Actor, Stimulus);
	}
	else if ( Stimulus.Type == UAISense::GetSenseID<UAISense_Sight>() )
	{
		OnEnemySightVision(TArray<AActor*>{ Actor });
	}
}

void AKBaseEnemy::OnEnemyNoiseHeard(AActor* Actor, FAIStimulus Stimulus)
{
	if ( Stimulus.Type == UAISense::GetSenseID<UAISense_Hearing>() )
	{
		// 소리 발생 위치와 강도 저장
		FVector NoiseLocation = Stimulus.StimulusLocation; //소리위치
		EnemyAttentionDegree += Stimulus.Strength; //소리강도

		// 소리 강도에 따라 이동 플래그 설정
		if ( EnemyAttentionDegree > AttentionThreshold ) // 특정 소리 강도 기준
		{
			bShouldMoveToSound = true;
			EnemySetState(EEnemyState::MOVE);
			SoundLocation = NoiseLocation;
			//ai->MoveToLocation(SoundLocation); //이걸 여기서?
			GEngine->AddOnScreenDebugMessage(2, 1, FColor::Red, FString::Printf(TEXT("OnEnemyNoiseHeard called with stimulus: Loudness(%f) > 100.0f"), EnemyAttentionDegree));

			//어그로 수치 초기화
			EnemyAttentionDegree = 0;
			return;
		}
	}
}

void AKBaseEnemy::OnEnemySightVision(const TArray<AActor*>& UpdatedActors)
{
	//UpdatedActor배열에 있는 각 Actor에 대한 반복
	for ( AActor* Actor : UpdatedActors )
	{

		FActorPerceptionBlueprintInfo Info;
		//AIPeceprtionComp를 통해 특정 Actor에 대한 인식을 가져옴
		AIPerceptionComp->GetActorsPerception(Actor, Info);

		// JPlayer 타입의 액터인지 확인
		if ( AJPlayer* Player = Cast<AJPlayer>(Actor) )
		{

			//Info구조체의 LastSensedStimuli 배열을 탐색
			for ( const FAIStimulus& Stimulus : Info.LastSensedStimuli )
			{
				//자극이 성공적으로 인식되고, 그 타입이 Ai_Sight인지 체크
				//if ( Stimulus.WasSuccessfullySensed() && Stimulus.Type.Name == TEXT("Default__AISense_Sight") )
				if ( Stimulus.WasSuccessfullySensed() && Stimulus.Type == UAISense::GetSenseID<UAISense_Sight>() )
				{
					if ( !bShoutMoveToSight )
					{
						bShoutMoveToSight = true;
						ShownLocation = Player->GetActorLocation(); // 플레이어 위치 저장
						EnemySetState(EEnemyState::MOVE);
						break;

					}
				}
			}
		}
		//아니면 다시 초기화
		else
		{
			bShoutMoveToSight = false;
		}
	}
}

void AKBaseEnemy::EnemyMove() { }

void AKBaseEnemy::EnemyAttack() { }

void AKBaseEnemy::EnemyOverlapDamage(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if ( target && OtherComp == target->GetCapsuleComponent() )
	{
		FPointDamageEvent DamageEvent(EnemyAttackDamage, FHitResult(), GetActorForwardVector(), nullptr);
		AController* ActorController = target->GetController();
		if ( ActorController ) 
		{
			target->TakeDamage(EnemyAttackDamage, DamageEvent, ActorController, this);
		}
	}
}

void AKBaseEnemy::EnemySpecialAttack() { }

void AKBaseEnemy::SetAllEnemiesToIdle() { }

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

		//배트 들었을 때 피격 애니메이션
		if ( target->CharacterEquipState == ECharacterEquipState::ECES_BatEquipped )
		{
			FString BatSectionName = FString::Printf(TEXT("EnemyTDamage%d"), BatIndex);
			anim->PlayEnemyTDamageAnim(FName(*BatSectionName));
			UE_LOG(LogTemp, Warning, TEXT("%s"), *BatSectionName);
			BatIndex++;
			if ( BatIndex > 4 )
			{
				BatIndex = 2;
			}
		}
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
	//ai->StopMovement();
}

void AKBaseEnemy::EnemyTakeDamage() { }

void AKBaseEnemy::EnemyExecuted() { }

void AKBaseEnemy::EnemyDead() { }