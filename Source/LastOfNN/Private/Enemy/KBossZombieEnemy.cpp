// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/KBossZombieEnemy.h"
#include "Player/JPlayer.h"
#include "Enemy/KEnemyAnim.h"
#include "Enemy/KBossZombieGrenade.h"
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
#include "Components/SphereComponent.h"
#include "Components/AudioComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/CapsuleComponent.h"

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

	//데미지처리를 위한 충돌체 손에 붙이기
	LeftAttackSphere->SetupAttachment(GetMesh(), TEXT("mixamorig_LeftHand"));
	LeftAttackSphere->SetSphereRadius(10.f);
	RightAttackSphere->SetupAttachment(GetMesh(), TEXT("mixamorig_RightHand"));
	RightAttackSphere->SetSphereRadius(10.f);

	//애니메이션 BP 할당
	ConstructorHelpers::FClassFinder<UAnimInstance> tempClass(TEXT("/Script/Engine.AnimBlueprint'/Game/BluePrints/Animation/Enemy/ABP_BossZombieEnemyAnim.ABP_BossZombieEnemyAnim_C'"));
	if ( tempClass.Succeeded() )
	{
		GetMesh()->SetAnimInstanceClass(tempClass.Class);
	}

	//팀타입 초기화
	TeamType = ETeamType::FRIENDLY;
	//TeamID = FGenericTeamId(1);

	//AI Perception Component 초기화
	AIPerceptionComp = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("PerceptionComponent"));

	//Enemy Status 초기화
	EnemySoundDetectionRadius = 2000.0f;
	EnemyWalkSpeed = 150.0f;
	EnemyRunSpeed = 300.0f;
	EnemyAttackRange = 200.0f;
	EnemySpecialAttackDamage = 20.0f;
	EnemySpecialAttackRange = 1000.0f;
	EnemyAttackDelayTime = 2.0f;
	EnemyAttackDamage = 15.0f;
	EnemyMoveDistanceOnSound = 300.0f;
	EnemyHP = 300;
}

void AKBossZombieEnemy::BeginPlay()
{
	Super::BeginPlay();

	//초기속도를 걷기로 설정
	GetCharacterMovement()->MaxWalkSpeed = EnemyWalkSpeed;

	//데미지처리함수 바인딩
	if ( RightAttackSphere )
	{
		RightAttackSphere->OnComponentBeginOverlap.AddDynamic(this, &AKBossZombieEnemy::EnemyOverlapDamage);
	}
	if ( LeftAttackSphere )
	{
		LeftAttackSphere->OnComponentBeginOverlap.AddDynamic(this, &AKBossZombieEnemy::EnemyOverlapDamage);
	}

	//Sound Attenuation 데이터로드
	EnemyAttenuation = LoadObject<USoundAttenuation>(nullptr, TEXT("/Script/Engine.SoundAttenuation'/Game/BluePrints/Effects/SFX/EnemyAttenuation.EnemyAttenuation'"));
	//소리3D 설정
	AudioComp->AttenuationSettings = EnemyAttenuation;
	//기본상태는 재생안함
	AudioComp->Stop();

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

	CurrentTime += GetWorld()->DeltaTimeSeconds;

	if ( target )
	{
		//(우선순위) 보스 특수 공격(수류탄) 발동을 위한 상태 변경 조건 설정
		float DistanceToTarget = FVector::Distance(target->GetActorLocation(), GetActorLocation());
		//근거리 공격범위 밖이고, 원거리 공격범위 안이면
		if ( DistanceToTarget > EnemyAttackRange && DistanceToTarget <= EnemySpecialAttackRange) 
		{
			//특수공격 쿨타임이 지나면
			if( CurrentTime > BossGrenadeDelayTime ) 
			{
				//속도를 0으로 만들고
				GetCharacterMovement()->MaxWalkSpeed = 0;
				//ai->StopMovement();
				//BlendSpace Anim에 액터의 속도 할당
				anim->EnemyVSpeed = FVector::DotProduct(GetActorRightVector(), GetVelocity());
				anim->EnemyHSpeed = FVector::DotProduct(GetActorForwardVector(), GetVelocity());

				//원거리 공격 상태로 전환
				isBossCanThrowGrenade = true;
				EnemySetState(EEnemyState::SPECIL);
				UE_LOG(LogTemp, Error, TEXT("Grenade Ready"));

				//공격 상태 전환 후 대기시간이 바로 끝나도록 처리
				CurrentTime = 0;
				return;
			}
		}

		//기본적으로 랜덤 지역 이동
		//(기본 Attack으로의 전환도 여기서 처리)
		EnemyRandomMove();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Target is null"));
	}
}

//Boss 랜덤 이동 함수
void AKBossZombieEnemy::EnemyRandomMove()
{
	FVector dir;
	FVector EnemyDestination;

	CurrentTime += GetWorld()->DeltaTimeSeconds;
	
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
	req.SetAcceptanceRadius(50);
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

		//SFX재생
		check(ChaseSFXFactory)
			if ( false == AudioComp->IsPlaying() )
			{
				AudioComp->SetSound(ChaseSFXFactory);
				AudioComp->Play();
			}

		//타깃과 가까워지면 공격상태 전환
		//공격범위 안에 들어오면
		//if ( dir.Size() < EnemyAttackRange && target->GetCharaterState() != ECharacterState::ECS_Crouching )
		if ( dir.Size() < EnemyAttackRange )
		{
			//AI의 길찾기 기능을 정지한다.
			//ai->StopMovement();
			
			//카메라 회전값 복원
			bUseControllerRotationYaw = true;
			GetCharacterMovement()->bOrientRotationToMovement = false;

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

void AKBossZombieEnemy::EnemyAttack()
{
	Super::EnemyAttack();

	//타깃과의 거리를 구하고
	float TargetDistance = FVector::Distance(target->GetActorLocation(), GetActorLocation());
	//시간이 흐르다가
	CurrentTime += GetWorld()->DeltaTimeSeconds;
	
	//보스 근접공격
	//공격시간이 되고, 근접공격범위 안이라면
	if ( CurrentTime > EnemyAttackDelayTime && TargetDistance <= EnemyAttackRange)
	{
		//공격 애니메이션 재생 활성화
		anim->bEnemyAttackPlay = true;

		//공격 충돌체 활성화
		LeftAttackSphere->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

		//Log
		GEngine->AddOnScreenDebugMessage(1, 1, FColor::Red, TEXT("Attack!!"));
		// 대기 시간 초기화
		CurrentTime = 0;

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
		0.5f,    //0.5초동안                       
		false, // 텔레포트하지 않음
		EMoveComponentAction::Type::Move,
		LatentInfo
		);

	}

	//근접공격거리 공격범위를 벗어나고, 원거리 공격상태가 아니라면 이동상태 전환
	if ( TargetDistance > EnemyAttackRange && isBossCanThrowGrenade == false )
	{
		//이동상태 전환
		EnemySetState(EEnemyState::MOVE);

		//공격 충돌체 꺼버리기
		LeftAttackSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		//랜덤 도착지역 재설정
		GetRandomPositionInNavMesh(GetActorLocation(), 500, EnemyRandomPos);
	}
}

//보스 특수공격(수류탄 던지기 처리)
void AKBossZombieEnemy::EnemySpecialAttack()
{
	GEngine->AddOnScreenDebugMessage(6, 2, FColor::Red, TEXT("Special Attack!!"));

	anim->bEnemyAttackPlay = true;

	if ( isBossCanThrowGrenade )
	{
		// 메시를 target방향으로 돌림
		GetCharacterMovement()->bOrientRotationToMovement = true;
		//보스 수류탄 공격 애니메이션 몽타주 재생
		anim->PlayBossEnemyGrenadeAnim(TEXT("ThrowGrenade"));

		//SFX재생
		check(BossGrenadeSFXFactory)
			if ( false == AudioComp->IsPlaying() )
			{
				AudioComp->SetSound(BossGrenadeSFXFactory);
				AudioComp->Play();
			}

		//수류탄 던질때 메시 회전
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
		0.7f,    //0.7초동안                       
		false, // 텔레포트하지 않음
		EMoveComponentAction::Type::Move,
		LatentInfo
		);

		//수류탄공격여부 비활성화
		isBossCanThrowGrenade = false;
		// 대기 시간 초기화
		CurrentTime = 0;
	}
}

void AKBossZombieEnemy::EnemyOverlapDamage(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::EnemyOverlapDamage(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
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


//FGenericTeamId AKBossZombieEnemy::GetGenericTeamId() const
//{
//	Super::GetGenericTeamId;
//
//	return FGenericTeamId();
//}
//
//ETeamAttitude::Type AKBossZombieEnemy::GetTeamAttitudeTowards(const AActor& Other) const
//{
//	Super:GetTeamAttitudeTowards(Other);	
//	
//	return ETeamAttitude::Type();
//}

//보스 수류탄 공격 함수
void AKBossZombieEnemy::BossThrowGrenade()
{
	//if ( BossGrenade && isBossCanThrowGrenade )
	if ( BossGrenade  )
	{
		//발사체 생성위치
		FVector SpawnLocation = GetActorLocation() + FVector(0, 0, 45); 
		FRotator SpawnRotation = (target->GetActorLocation() - GetActorLocation()).Rotation();
		SpawnRotation.Pitch += 50.0f;
		// 생성된 수류탄을 저장
		AKBossZombieGrenade* Grenade = GetWorld()->SpawnActor<AKBossZombieGrenade>(BossGrenade, SpawnLocation, SpawnRotation);
		//if ( Grenade && anim->bBossThrowGrenade == true )
		if ( Grenade )
		{
			
			// 수류탄 발사
			FVector LaunchDirection = SpawnRotation.Vector();

			Grenade->BossThrowGrenade(LaunchDirection);

			UE_LOG(LogTemp, Warning, TEXT("Boss Throw Grenade!!"));

			////애니메이션이 완료되면
			//if ( false == anim->bEnemyAttackPlay )
			//{
			//	//IDLE상태로 전환
			//	EnemySetState(EEnemyState::IDLE);
			//}
			

			////발사지연타이머 설정
			//FTimerHandle LaunchTimerHandle;
			//// 1.2초 후에 발사되도록 타이머 설정
			//GetWorld()->GetTimerManager().SetTimer(LaunchTimerHandle, FTimerDelegate::CreateLambda([=]()
			//	{
			//		// 수류탄 발사
			//		FVector LaunchDirection = SpawnRotation.Vector();
			//		Grenade->BossThrowGrenade(LaunchDirection);
			//		UE_LOG(LogTemp, Warning, TEXT("Boss Throw Grenade!!"));

			//	}), 2.5f, false); // 1.2초 후에 한 번만 실행되도록 설정a
		}

		//체크변수 초기화
		isBossCanThrowGrenade = false;
		anim->bBossThrowGrenade = false;
	}
}
