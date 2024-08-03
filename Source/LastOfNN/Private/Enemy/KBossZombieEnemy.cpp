// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/KBossZombieEnemy.h"
#include "Player/JPlayer.h"
#include "Enemy/KEnemyAnim.h"
#include "Runtime/AIModule/Classes/AIController.h"
#include "NavigationSystem.h"
#include "Runtime/AIModule/Classes/Navigation/PathFollowingComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Hearing.h"
#include <Kismet/GameplayStatics.h>
#include "Engine/World.h"
#include "EngineUtils.h"
#include "Components/BoxComponent.h"
#include "Components/SceneComponent.h"

AKBossZombieEnemy::AKBossZombieEnemy()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//외관 세팅
	ConstructorHelpers::FObjectFinder<USkeletalMesh> tempMesh(TEXT("/Script/Engine.SkeletalMesh'/Game/Assests/Enemy/Boss/Can_Use/SkeletalMesh/Boss_Skeletal.Boss_Skeletal'"));
	if ( tempMesh.Succeeded() )
	{
		GetMesh()->SetSkeletalMesh(tempMesh.Object);
		GetMesh()->SetRelativeLocationAndRotation(FVector(0, 0, -88), FRotator(0, -90, 0));
	}
	
	//애니메이션 BP 할당
	ConstructorHelpers::FClassFinder<UAnimInstance> tempClass(TEXT("/Script/Engine.AnimBlueprint'/Game/BluePrints/Animation/Enemy/ABP_BossZombieEnemyAnim.ABP_BossZombieEnemyAnim_C'"));
	if ( tempClass.Succeeded() )
	{
		GetMesh()->SetAnimInstanceClass(tempClass.Class);
	}

	//팀타입 초기화
	TeamType = ETeamType::FRIENDLY;

	//AI Perception Component 초기화
	AIPerceptionComp = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("PerceptionComponent"));

	//Enemy Status 초기화
	EnemySoundDetectionRadius = 2000.0f;
	EnemyWalkSpeed = 150.0f;
	EnemyRunSpeed = 300.0f;
	EnemyAttackRange = 145.0f;
	//원거리 범위 필요
	EnemyAttackDelayTime = 2.0f;
	EnemyAttackDamage = 60.0f;
	EnemyMoveDistanceOnSound = 300.0f;
	EnemyHP = 300;
}

void AKBossZombieEnemy::BeginPlay()
{
	Super::BeginPlay();

	//초기속도를 걷기로 설정
	GetCharacterMovement()->MaxWalkSpeed = EnemyWalkSpeed;

}

void AKBossZombieEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AKBossZombieEnemy::EnemySetState(EEnemyState newstate)
{
	Super::EnemySetState(newstate);
}

void AKBossZombieEnemy::EnemyIDLE()
{
	Super::EnemyIDLE();
}



void AKBossZombieEnemy::EnemyMove()
{
	Super::EnemyMove();

	FVector dir;
	FVector EnemyDestination;

	if ( target )
	{
		//타깃목적지
		EnemyDestination = target->GetActorLocation();
		//방향
		dir = EnemyDestination - GetActorLocation();

		//(1단계) 길찾기 결과 얻어오기
		//Navigation 객체 얻어오기
		auto ns = UNavigationSystemV1::GetNavigationSystem(GetWorld());
		//목적지 길찾기 경로 데이터 탐색
		FPathFindingQuery query;
		FAIMoveRequest req;
		//목적지 인지 가능 범위
		req.SetAcceptanceRadius(3);
		req.SetGoalLocation(EnemyDestination);
		//길찾기 위한 쿼리 생성
		ai->BuildPathfindingQuery(req, query);
		//길찾기 결과 가져오기
		FPathFindingResult FindingResult = ns->FindPathSync(query);

		//(2단계) 길찾기 데이터 결과에 따른 이동 수행하기
		//if ( FindingResult.Result == ENavigationQueryResult::Success && target->GetCharaterState() != ECharacterState::ECS_Crouching )
		if ( FindingResult.Result == ENavigationQueryResult::Success )
		{
			//속도를 뛰기속도로 변경
			GetCharacterMovement()->MaxWalkSpeed = EnemyRunSpeed;
			//BlendSpace Anim에 액터의 속도 할당
			anim->EnemyVSpeed = FVector::DotProduct(GetActorRightVector(), GetVelocity());
			anim->EnemyHSpeed = FVector::DotProduct(GetActorForwardVector(), GetVelocity());
			//타깃에게 이동
			ai->MoveToLocation(EnemyDestination);

			//타깃과 가까워지면 공격상태 전환
			//공격범위 안에 들어오면
			//if ( dir.Size() < EnemyAttackRange && target->GetCharaterState() != ECharacterState::ECS_Crouching )
			if ( dir.Size() < EnemyAttackRange )
			{
				//AI의 길찾기 기능을 정지한다.
				ai->StopMovement();
				//공격상태전환 /애니메이션 동기화 
				EnemySetState(EEnemyState::ATTACK);
				//공격 애니메이션 재생 활성화
				anim->bEnemyAttackPlay = true;
				//공격 상태 전환 후 대기시간이 바로 끝나도록 처리
				CurrentTime = EnemyAttackDelayTime;
			}
		}
		else
		{
			//랜덤하게 이동
			auto RanResult = ai->MoveToLocation(EnemyRandomPos);
			//속도를 걷기속도로 변경
			GetCharacterMovement()->MaxWalkSpeed = EnemyWalkSpeed;
			//BlendSpace Anim에 액터의 속도 할당
			anim->EnemyVSpeed = FVector::DotProduct(GetActorRightVector(), GetVelocity());
			anim->EnemyHSpeed = FVector::DotProduct(GetActorForwardVector(), GetVelocity());
			//목적지에 도착하면
			if ( RanResult == EPathFollowingRequestResult::AlreadyAtGoal )
			{
				//새로운 랜덤위치 가져오기
				GetRandomPositionInNavMesh(GetActorLocation(), 500, EnemyRandomPos);
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Target is null"));
	}

}

void AKBossZombieEnemy::EnemyAttack()
{
	Super::EnemyAttack();

	//시간이 흐르다가
	CurrentTime += GetWorld()->DeltaTimeSeconds;
	//공격시간이 되면
	if ( CurrentTime > EnemyAttackDelayTime )
	{
		// 일정 확률로 Grab 상태로 전환
		float RandomChance = FMath::FRand();
		if ( RandomChance < 0.3f ) // 30% 확률로 Grab
		{
			//EnemyGrab();
			CurrentTime = 0;
		}
		else
		{
			//공격한다.(내용은 나중에 구현)
			GEngine->AddOnScreenDebugMessage(0, 2, FColor::Red, TEXT("Attack!!"));

			//공격 애니메이션 재생 활성화
			anim->bEnemyAttackPlay = true;
		}

		// 대기 시간 초기화
		CurrentTime = 0;

	}
	//타깃과의 거리를 구하고
	float TargetDistance = FVector::Distance(target->GetActorLocation(), GetActorLocation());
	//거리가 공격범위를 벗어나면
	if ( TargetDistance > EnemyAttackRange )
	{
		//이동상태 전환
		EnemySetState(EEnemyState::MOVE);
		GetRandomPositionInNavMesh(GetActorLocation(), 500, EnemyRandomPos);
	}
}



float AKBossZombieEnemy::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float FinalDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	return FinalDamage;
}

void AKBossZombieEnemy::OnEnemyDamageProcess(float damage)
{
	Super::OnEnemyDamageProcess(damage);
}

void AKBossZombieEnemy::EnemyTakeDamage()
{
	Super::EnemyTakeDamage();

	//시간이 흐르다가
	CurrentTime += GetWorld()->DeltaTimeSeconds;
	//전환대기시간이 지나면
	if ( CurrentTime > EnemyTDamageDelayTime )
	{
		//IDLE상태로 전환/애니메이션 상태 동기화
		EnemySetState(EEnemyState::IDLE);
		
		CurrentTime = 0;
	}
}

void AKBossZombieEnemy::EnemyDead()
{
	Super::EnemyDead();

	//아직 죽음애니메이션 처리가 완료되지않았다면
	//바닥으로 내려가지않도록 처리
	if ( anim->bEnemyDieDone == false )
	{
		return;
	}

	//죽으면아래로 내려간다.
	FVector P = GetActorLocation() + FVector::DownVector * DieDownfallSpeed * GetWorld()->DeltaTimeSeconds;
	SetActorLocation(P);
	//2미터 이상 내려가면
	if ( P.Z < -200.0f )
	{
		Destroy();
	}
}
