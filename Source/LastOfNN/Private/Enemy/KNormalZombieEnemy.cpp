﻿// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/KNormalZombieEnemy.h"
#include "Enemy/KBeginnerZombieEnemy.h"
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
#include "Components/SphereComponent.h"
#include "Components/SceneComponent.h"
#include "Components/AudioComponent.h"
#include <Enemy/KBossZombieEnemy.h>
#include "Kismet/KismetMathLibrary.h"
#include "Components/CapsuleComponent.h"

//=======================================================================================
//이 좀비는 이름만 Normal이지 사실 Clicker임 ㅋㅋ
//=======================================================================================

AKNormalZombieEnemy::AKNormalZombieEnemy()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//외관 세팅
	ConstructorHelpers::FObjectFinder<USkeletalMesh> tempMesh(TEXT("/Script/Engine.SkeletalMesh'/Game/Assests/Enemy/Clicker/Can_Use/SkeletalMesh/Clicker_Skeletal.Clicker_Skeletal'"));
	if (tempMesh.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(tempMesh.Object);
		GetMesh()->SetRelativeLocationAndRotation(FVector(0,0, -88), FRotator(0,-90,0));
		GetMesh()->SetRelativeScale3D(FVector(0.11f));
	}
	//데미지처리를 위한 충돌체 손에 붙이기
	LeftAttackSphere->SetupAttachment(GetMesh(), TEXT("LeftHand"));
	LeftAttackSphere->SetSphereRadius(10.f);
	RightAttackSphere->SetupAttachment(GetMesh(), TEXT("RightHand"));
	RightAttackSphere->SetSphereRadius(10.f);

	//암살 이벤트를 위한 충돌체 세팅
	AssassinBox = CreateDefaultSubobject<UBoxComponent>(TEXT("AssassinBox"));
	if ( AssassinBox )
	{
		AssassinBox->SetupAttachment(GetRootComponent());
		AssassinBox->SetRelativeLocation(FVector(-105.f, 0.f, 0.f));
	}

	AssassinSceneComp = CreateDefaultSubobject<USceneComponent>(TEXT("AttackerLocation"));
	if ( AssassinSceneComp )
	{
		AssassinSceneComp->SetupAttachment(GetRootComponent());
		AssassinSceneComp->SetRelativeLocation(FVector(-177.f, 5.f, 0.f));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to create AssassinSceneComp."));
	}
	//애니메이션 BP 할당
	ConstructorHelpers::FClassFinder<UAnimInstance> tempClass(TEXT("/Script/Engine.AnimBlueprint'/Game/BluePrints/Animation/Enemy/ABP_NormalZombieEnemyAnim.ABP_NormalZombieEnemyAnim_C'"));
	if (tempClass.Succeeded())
	{
		GetMesh()->SetAnimInstanceClass(tempClass.Class);
	}

	//팀타입 초기화
	TeamType = ETeamType::FRIENDLY;
	//TeamID = FGenericTeamId(1);

	//AI Perception Component 초기화
	AIPerceptionComp = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("PerceptionComponent"));
	HearingConfig = CreateDefaultSubobject<UAISenseConfig_Hearing>(TEXT("HearingConfig"));
	if ( HearingConfig )
	{
		//소리감지 설정
		HearingConfig->HearingRange = EnemySoundDetectionRadius;
		HearingConfig->DetectionByAffiliation.bDetectEnemies = true; 
		HearingConfig->DetectionByAffiliation.bDetectFriendlies = true;
		HearingConfig->DetectionByAffiliation.bDetectNeutrals = true;

		//PerceptionComp에 Hearing Config 전달받은 값 연결
		AIPerceptionComp->ConfigureSense(*HearingConfig);
		AIPerceptionComp->SetDominantSense(HearingConfig->GetSenseImplementation());
	}
	//노이즈 발생시 처리내용 초기화
	//소음 발생위치 이동여부 초기화
	bShouldMoveToSound = false;

	//Enemy Status 초기화
	EnemySoundDetectionRadius = 2000.0f;
	EnemyWalkSpeed = 200.0f;
	EnemyRunSpeed = 400.0f;
	EnemyAttackRange = 145.0f;
	EnemyAttackDelayTime = 2.0f;
	EnemyAttackDamage = 5.0f;
	EnemyMoveDistanceOnSound = 300.0f;
	EnemyHP = 100;
}

void AKNormalZombieEnemy::BeginPlay()
{
	Super::BeginPlay();

	//초기속도를 걷기로 설정
	GetCharacterMovement()->MaxWalkSpeed = EnemyWalkSpeed;

	//Sound Attenuation 데이터로드
	EnemyAttenuation = LoadObject<USoundAttenuation>(nullptr, TEXT("/Script/Engine.SoundAttenuation'/Game/BluePrints/Effects/SFX/EnemyAttenuation.EnemyAttenuation'"));
	//소리3D 설정
	AudioComp->AttenuationSettings = EnemyAttenuation;
	//기본상태는 재생안함
	AudioComp->Stop();

	//소리감지처리함수 바인딩
	if ( AIPerceptionComp )
	{
		AIPerceptionComp->OnTargetPerceptionUpdated.AddDynamic(this, &AKNormalZombieEnemy::OnEnemyNoiseHeard);
		UE_LOG(LogTemp, Warning, TEXT("Perception Component 초기화 완료"));

		if ( HearingConfig )
		{
			HearingConfig->HearingRange = EnemySoundDetectionRadius;
			AIPerceptionComp->ConfigureSense(*HearingConfig);
		}
	}
	//데미지처리함수 바인딩
	if ( RightAttackSphere )
	{
		RightAttackSphere->OnComponentBeginOverlap.AddDynamic(this, &AKNormalZombieEnemy::EnemyOverlapDamage);
	}
	if ( LeftAttackSphere )
	{
		LeftAttackSphere->OnComponentBeginOverlap.AddDynamic(this, &AKNormalZombieEnemy::EnemyOverlapDamage);
	}
}

void AKNormalZombieEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AKNormalZombieEnemy::EnemySetState(EEnemyState newstate)
{
	Super::EnemySetState(newstate);
}

void AKNormalZombieEnemy::EnemyIDLE()
{
	Super::EnemyIDLE();
}

void AKNormalZombieEnemy::OnEnemyNoiseHeard(AActor* Actor, FAIStimulus Stimulus)
{
	Super::OnEnemyNoiseHeard(Actor, Stimulus);
	UE_LOG(LogTemp, Warning, TEXT("OnEnemyNoiseHeard called with stimulus: %s"), *Stimulus.Tag.ToString());
}

void AKNormalZombieEnemy::EnemyMove()
{
	Super::EnemyMove();

	FVector dir;
	FVector EnemyDestination;

	if ( bShouldMoveToSound )
	{
		// 소음에 의해 이동해야 하는 경우
		EnemyDestination = SoundLocation;
		dir = EnemyDestination - GetActorLocation();		

		// 소리 방향으로 이동할 위치 계산
		FVector Direction = dir.GetSafeNormal();
		FVector NewLocation = GetActorLocation() + Direction * EnemyMoveDistanceOnSound;

		//속도를 뛰기속도로 변경
		GetCharacterMovement()->MaxWalkSpeed = EnemyWalkSpeed;
		//BlendSpace Anim에 액터의 속도 할당
		anim->EnemyVSpeed = FVector::DotProduct(GetActorRightVector(), GetVelocity());
		anim->EnemyHSpeed = FVector::DotProduct(GetActorForwardVector(), GetVelocity());

		// AI를 이용하여 계산된 위치로 이동
		ai->MoveToLocation(NewLocation);
		
		//만약 목적지에 도착했다면
		if ( dir.Size() < 150.0f )
		{
			//이동 플래그 초기화
			bShouldMoveToSound = false;
			//IDLE상태 전환
			EnemySetState(EEnemyState::IDLE);
		}
	}
	else if (target)
	{
		EnemyRandomMove();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Target is null"));
	}
}

void AKNormalZombieEnemy::EnemyRandomMove()
{
	
	//if(target && target이 시야 어그로 수치를 만족시켰을경우)

	FVector dir;
	FVector EnemyDestination;

	//타깃목적지
	EnemyDestination = target->GetActorLocation();
	//방향
	dir = EnemyDestination - GetActorLocation();
	//거리 구하기
	float targetdistance = dir.Size();

	//(1단계) 길찾기 결과 얻어오기
	//Navigation 객체 얻어오기
	auto ns = UNavigationSystemV1::GetNavigationSystem(GetWorld());
	//목적지 길찾기 경로 데이터 탐색
	FPathFindingQuery query;
	FAIMoveRequest req;
	//목적지 인지 가능 범위
	req.SetAcceptanceRadius(50);
	req.SetGoalLocation(EnemyDestination);
	//길찾기 위한 쿼리 생성
	ai->BuildPathfindingQuery(req, query);
	//길찾기 결과 가져오기
	FPathFindingResult FindingResult = ns->FindPathSync(query);

	//(2단계) 길찾기 데이터 결과에 따른 이동 수행하기
	if ( FindingResult.Result == ENavigationQueryResult::Success && target->GetCharaterState() != ECharacterState::ECS_Crouching )
	{
		//속도를 뛰기속도로 변경
		GetCharacterMovement()->MaxWalkSpeed = EnemyRunSpeed;
		//UE_LOG(LogTemp, Warning, TEXT("EnemySpeed : %f"), GetCharacterMovement()->MaxWalkSpeed);
		//BlendSpace Anim에 액터의 속도 할당
		anim->EnemyVSpeed = FVector::DotProduct(GetActorRightVector(), GetVelocity());
		anim->EnemyHSpeed = FVector::DotProduct(GetActorForwardVector(), GetVelocity());
		//타깃에게 이동
		ai->MoveToLocation(EnemyDestination);

		//SFX재생
		check(ChaseSFXFactory)
			if ( false == AudioComp->IsPlaying() )
			{
				AudioComp->SetSound(ChaseSFXFactory);
				AudioComp->Play();
			}

		//타깃과 가까워지면 공격상태 전환
		//공격범위 안에 들어오면
		if ( targetdistance < EnemyAttackRange && target->GetCharaterState() != ECharacterState::ECS_Crouching )
		{
			//AI의 길찾기 기능을 정지한다.
			//ai->StopMovement();
			 
			//카메라 회전값 복원
			bUseControllerRotationYaw = true;
			GetCharacterMovement()->bOrientRotationToMovement = false;
			//공격상태 전환 / 애니메이션 상태 동기화
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
		//UE_LOG(LogTemp, Warning, TEXT("EnemySpeed : %f"), GetCharacterMovement()->MaxWalkSpeed);
		//BlendSpace Anim에 액터의 속도 할당
		anim->EnemyVSpeed = FVector::DotProduct(GetActorRightVector(), GetVelocity());
		anim->EnemyHSpeed = FVector::DotProduct(GetActorForwardVector(), GetVelocity());
		//자연스럽게 메시회전
		bUseControllerRotationYaw = false;
		GetCharacterMovement()->bOrientRotationToMovement = true;
		//목적지에 도착하면
		if ( RanResult == EPathFollowingRequestResult::AlreadyAtGoal || RanResult == EPathFollowingRequestResult::Failed )
		{
			//새로운 랜덤위치 가져오기
			GetRandomPositionInNavMesh(GetActorLocation(), 500, EnemyRandomPos);
		}
	}
}

void AKNormalZombieEnemy::EnemyAttack()
{
	Super::EnemyAttack();

	//시간이 흐르다가
	CurrentTime += GetWorld()->DeltaTimeSeconds;
	//공격시간이 되면
	if (CurrentTime>EnemyAttackDelayTime && target->GetCharaterState() != ECharacterState::ECS_Crouching)
	{
		// 일정 확률로 Grab 상태로 전환
		float RandomChance = FMath::FRand();
		if ( RandomChance < 0.2f ) // 20% 확률로 Grab
		{
			EnemySetState(EEnemyState::SPECIL);
		}
		else
		{
			//공격한다.(내용은 나중에 구현)
			GEngine->AddOnScreenDebugMessage(1, 1, FColor::Red, TEXT("Attack!!"));

			//공격 충돌체 활성화
			RightAttackSphere->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

			//공격 애니메이션 재생 활성화
			anim->bEnemyAttackPlay = true;

			//공격한다.(내용은 나중에 구현)
			//SFX재생
			check(AttackSFXFactory)
				if ( false == AudioComp->IsPlaying() )
				{
					AudioComp->SetSound(AttackSFXFactory);
					AudioComp->Play();
				}

			//공격시 메시 회전
			FLatentActionInfo LatentInfo;
			LatentInfo.CallbackTarget = this;

			bUseControllerRotationYaw = false; //회전하게 하기(이거 꺼야 회전함) -> 나중에 다시 true로 돌려놓기
			FRotator rot = UKismetMathLibrary::FindLookAtRotation(this->GetActorLocation(), target->GetActorLocation());//서로 바라보는 방향

			UKismetSystemLibrary::MoveComponentTo(
			GetCapsuleComponent(),   // 이동할 컴포넌트
			GetActorLocation(),  //목표위치(현재위치에서 회전)
			rot,         // 목표 회전
			false,  //바로 빠져나와서                            
			true,	//천천히 돌아감
			0.5f,    //1초동안                       
			false, // 텔레포트하지 않음
			EMoveComponentAction::Type::Move,
			LatentInfo
			);
		}
		// 대기 시간 초기화
		CurrentTime = 0;
	}

	//타깃과의 거리를 구하고
	float TargetDistance = FVector::Distance(target->GetActorLocation(), GetActorLocation());
	//거리가 공격범위를 벗어나면
	if (TargetDistance > EnemyAttackRange)
	{
		//이동상태 전환 /애니메이션 상태 동기화
		EnemySetState(EEnemyState::MOVE);

		//공격 충돌체 꺼버리기
		RightAttackSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		//랜덤위치값을 이때도 다시 설정
		GetRandomPositionInNavMesh(GetActorLocation(), 500, EnemyRandomPos);
	}
}

void AKNormalZombieEnemy::EnemyOverlapDamage(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::EnemyOverlapDamage(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep,SweepResult);

}

void AKNormalZombieEnemy::EnemySpecialAttack()
{
	Super::EnemySpecialAttack();

	// Grab 애니메이션 재생
	if ( anim )
	{
		FString SectionName = FString::Printf(TEXT("Grab"));
		anim->PlayEnemyGrabAnim(FName(*SectionName));
	}

	//SFX재생
	check(GrabSFXFactory)
		if ( false == AudioComp->IsPlaying() )
		{
			AudioComp->SetSound(GrabSFXFactory);
			AudioComp->Play();
		}

	// Player에게 Grab 상태 알림 및 QTE 이벤트 시작
	if ( target && !bIsPlayerGrabbed )
	{
		AJPlayer* Player = Cast<AJPlayer>(target);
		if ( Player )
		{
			// Player를 잡았음을 표시
			bIsPlayerGrabbed = true;
			// Player의 Grab 상태 시작 및 QTE 이벤트 시작
			Player->StartGrabbedState(this);

			//Grab Event시 메시 회전
			FLatentActionInfo LatentInfo;
			LatentInfo.CallbackTarget = this;

			bUseControllerRotationYaw = false; //회전하게 하기(이거 꺼야 회전함) -> 나중에 다시 true로 돌려놓기
			FRotator rot = UKismetMathLibrary::FindLookAtRotation(this->GetActorLocation(), target->GetActorLocation());//서로 바라보는 방향

			UKismetSystemLibrary::MoveComponentTo(
			GetCapsuleComponent(),   // 이동할 컴포넌트
			GetActorLocation(),  //목표위치(현재위치에서 회전)
			rot,         // 목표 회전
			false,  //바로 빠져나와서                            
			true,	//천천히 돌아감
			1.f,    //1초동안                       
			false, // 텔레포트하지 않음
			EMoveComponentAction::Type::Move,
			LatentInfo
			);
		}
	}

	// QTE 이벤트가 시작되었음을 전역 변수에 표시
	FSMComponent->bIsQTEActive = true;

	// 모든 Enemy를 IDLE 상태로 유지
	SetAllEnemiesToIdle();
}

void AKNormalZombieEnemy::SetAllEnemiesToIdle()
{
	Super::SetAllEnemiesToIdle();

	// 월드에 존재하는 모든 Enemy를 IDLE 상태로 전환
	for ( TActorIterator<AKBeginnerZombieEnemy> It(GetWorld()); It; ++It )
	{
		AKBeginnerZombieEnemy* Enemy = *It;
		if ( Enemy && Enemy->TeamType == ETeamType::FRIENDLY )
		{
			EnemySetState(EEnemyState::IDLE);
		}
	}
	for ( TActorIterator<AKNormalZombieEnemy> It(GetWorld()); It; ++It )
	{
		AKNormalZombieEnemy* Enemy = *It;
		if ( Enemy && Enemy->TeamType == ETeamType::FRIENDLY && Enemy != this )
		{
			EnemySetState(EEnemyState::IDLE);
		}
	}
	for ( TActorIterator<AKBossZombieEnemy> It(GetWorld()); It; ++It )
	{
		AKBossZombieEnemy* Enemy = *It;
		if ( Enemy && Enemy->TeamType == ETeamType::FRIENDLY )
		{
			EnemySetState(EEnemyState::IDLE);
		}
	}
}

float AKNormalZombieEnemy::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float FinalDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	return FinalDamage;
}

void AKNormalZombieEnemy::OnEnemyDamageProcess(float damage)
{
	Super::OnEnemyDamageProcess(damage);
}

void AKNormalZombieEnemy::EnemyTakeDamage()
{
	Super::EnemyTakeDamage();

	//시간이 흐르다가
	CurrentTime += GetWorld()->DeltaTimeSeconds;
	//전환대기시간이 지나면
	if (CurrentTime > EnemyTDamageDelayTime)
	{
		//IDLE상태로 전환/애니메이션 상태 동기화
		EnemySetState(EEnemyState::IDLE);
		CurrentTime = 0;
	}
}

void AKNormalZombieEnemy::EnemyExecuted()
{
	Super::EnemyExecuted();

	ai->StopMovement();

	//SFX재생
	check(ExcecutedSFXFactory)
		if ( false == AudioComp->IsPlaying() )
		{
			AudioComp->SetSound(ExcecutedSFXFactory);
			AudioComp->Play();
		}
}

void AKNormalZombieEnemy::EnemyDead()
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
	if (P.Z < -200.0f)
	{
		Destroy();
	}
}

//FGenericTeamId AKNormalZombieEnemy::GetGenericTeamId() const
//{
//	Super::GetGenericTeamId;
//
//	return FGenericTeamId();
//}
//
//ETeamAttitude::Type AKNormalZombieEnemy::GetTeamAttitudeTowards(const AActor& Other) const
//{
//	Super:GetTeamAttitudeTowards(Other);
//
//	return ETeamAttitude::Type();
//}

//Player암살 이벤트 시 메시 고정
FTransform AKNormalZombieEnemy::GetAttackerTransform()
{
	return AssassinSceneComp->GetComponentToWorld();
}
