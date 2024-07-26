// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/KNormalZombieEnemy.h"
#include "Player/JPlayer.h"

AKNormalZombieEnemy::AKNormalZombieEnemy()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//외관 세팅
	ConstructorHelpers::FObjectFinder<USkeletalMesh> tempMesh(TEXT("/Script/Engine.SkeletalMesh'/Game/Assests/Enemy/Clicker/Idle/Clicker_idle__1_.Clicker_idle__1_'"));
	if (tempMesh.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(tempMesh.Object);
		GetMesh()->SetRelativeLocationAndRotation(FVector(0,0, -88), FRotator(0,-90,0));
		GetMesh()->SetRelativeScale3D(FVector(0.1f));
	}

	//애니메이션 BP 할당
	ConstructorHelpers::FClassFinder<UAnimInstance> tempClass(TEXT("/Script/Engine.AnimBlueprint'/Game/BluePrints/Enemy/ABP_NormalZombieAnim.ABP_NormalZombieAnim_C'"));
	if (tempClass.Succeeded())
	{
		GetMesh()->SetAnimInstanceClass(tempClass.Class);
	}

	//Enemy Status 초기화
	EnemyAttackRange = 150.0f;
	EnemyAttackDelayTime = 2.0f;
	EnemyHP = 200;
}

void AKNormalZombieEnemy::BeginPlay()
{
	Super::BeginPlay();

}

void AKNormalZombieEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AKNormalZombieEnemy::EnemyIDLE()
{
	Super::EnemyIDLE();
}

void AKNormalZombieEnemy::EnemyMove()
{
	Super::EnemyMove();

	FVector dir;
	if (target)
	{
		//타깃목적지
		FVector EnemyDestination = target->GetActorLocation();
		//방향
		dir = EnemyDestination - GetActorLocation();
		//이동
		AddMovementInput(dir.GetSafeNormal());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Target is null"));
	}
	
	//타깃과 가까워지면 공격상태 전환
	//공격범위 안에 들어오면
	if (dir.Size() < EnemyAttackRange)
	{
		//공격상태 전환
		FSMComponent->CurrentState = EEnemyState::ATTACK;
	}

}


void AKNormalZombieEnemy::EnemyAttack()
{
	Super::EnemyAttack();

	//시간이 흐르다가
	CurrentTime += GetWorld()->DeltaTimeSeconds;
	//공격시간이 되면
	if (CurrentTime>EnemyAttackDelayTime)
	{
		//공격한다.(내용은 나중에 구현)
		GEngine->AddOnScreenDebugMessage(0, 2, FColor::Red, TEXT("Attack!!"));
		//대기시간 초기화
		CurrentTime = 0;
	}
	//타깃과의 거리를 구하고
	float TargetDistance = FVector::Distance(target->GetActorLocation(), GetActorLocation());
	//거리가 공격범위를 벗어나면
	if (TargetDistance > EnemyAttackRange)
	{
		//이동상태 전환
		FSMComponent->CurrentState = EEnemyState::MOVE;
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
		//IDLE상태로 전환
		FSMComponent->CurrentState = EEnemyState::IDLE;
		CurrentTime = 0;
	}
}

void AKNormalZombieEnemy::EnemyDead()
{
	Super::EnemyDead();

	//죽으면아래로 내려간다.
	FVector P = GetActorLocation() + FVector::DownVector * DieDownfallSpeed * GetWorld()->DeltaTimeSeconds;
	SetActorLocation(P);
	//2미터 이상 내려가면
	if (P.Z < -200.0f)
	{
		Destroy();
	}
}
