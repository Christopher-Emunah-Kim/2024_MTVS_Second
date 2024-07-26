// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/KNormalZombieEnemy.h"
#include "Player/JPlayer.h"

AKNormalZombieEnemy::AKNormalZombieEnemy()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//�ܰ� ����
	ConstructorHelpers::FObjectFinder<USkeletalMesh> tempMesh(TEXT("/Script/Engine.SkeletalMesh'/Game/Assests/Enemy/Clicker/Idle/Clicker_idle__1_.Clicker_idle__1_'"));
	if (tempMesh.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(tempMesh.Object);
		GetMesh()->SetRelativeLocationAndRotation(FVector(0,0, -88), FRotator(0,-90,0));
		GetMesh()->SetRelativeScale3D(FVector(0.1f));
	}

	//�ִϸ��̼� BP �Ҵ�
	ConstructorHelpers::FClassFinder<UAnimInstance> tempClass(TEXT("/Script/Engine.AnimBlueprint'/Game/BluePrints/Enemy/ABP_NormalZombieAnim.ABP_NormalZombieAnim_C'"));
	if (tempClass.Succeeded())
	{
		GetMesh()->SetAnimInstanceClass(tempClass.Class);
	}

	//Enemy Status �ʱ�ȭ
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
		//Ÿ�������
		FVector EnemyDestination = target->GetActorLocation();
		//����
		dir = EnemyDestination - GetActorLocation();
		//�̵�
		AddMovementInput(dir.GetSafeNormal());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Target is null"));
	}
	
	//Ÿ��� ��������� ���ݻ��� ��ȯ
	//���ݹ��� �ȿ� ������
	if (dir.Size() < EnemyAttackRange)
	{
		//���ݻ��� ��ȯ
		FSMComponent->CurrentState = EEnemyState::ATTACK;
	}

}


void AKNormalZombieEnemy::EnemyAttack()
{
	Super::EnemyAttack();

	//�ð��� �帣�ٰ�
	CurrentTime += GetWorld()->DeltaTimeSeconds;
	//���ݽð��� �Ǹ�
	if (CurrentTime>EnemyAttackDelayTime)
	{
		//�����Ѵ�.(������ ���߿� ����)
		GEngine->AddOnScreenDebugMessage(0, 2, FColor::Red, TEXT("Attack!!"));
		//���ð� �ʱ�ȭ
		CurrentTime = 0;
	}
	//Ÿ����� �Ÿ��� ���ϰ�
	float TargetDistance = FVector::Distance(target->GetActorLocation(), GetActorLocation());
	//�Ÿ��� ���ݹ����� �����
	if (TargetDistance > EnemyAttackRange)
	{
		//�̵����� ��ȯ
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

	//�ð��� �帣�ٰ�
	CurrentTime += GetWorld()->DeltaTimeSeconds;
	//��ȯ���ð��� ������
	if (CurrentTime > EnemyTDamageDelayTime)
	{
		//IDLE���·� ��ȯ
		FSMComponent->CurrentState = EEnemyState::IDLE;
		CurrentTime = 0;
	}
}

void AKNormalZombieEnemy::EnemyDead()
{
	Super::EnemyDead();

	//������Ʒ��� ��������.
	FVector P = GetActorLocation() + FVector::DownVector * DieDownfallSpeed * GetWorld()->DeltaTimeSeconds;
	SetActorLocation(P);
	//2���� �̻� ��������
	if (P.Z < -200.0f)
	{
		Destroy();
	}
}
