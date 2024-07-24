// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/KNormalZombieEnemy.h"
#include "Player/JPlayer.h"

AKNormalZombieEnemy::AKNormalZombieEnemy()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//�ܰ� ����
	ConstructorHelpers::FObjectFinder<USkeletalMesh> tempMesh(TEXT("/Script/Engine.SkeletalMesh'/Game/Assests/Enemy/Mesh/NormalZombie/01/T-Pose.T-Pose'"));
	if (tempMesh.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(tempMesh.Object);
		GetMesh()->SetRelativeLocationAndRotation(FVector(0,0, -88), FRotator(0,-90,0));
	}

	//Enemy Status �ʱ�ȭ
	EnemyAttackRange = 150.0f;
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

void AKNormalZombieEnemy::EnemyDamage()
{
	Super::EnemyDamage();
}

void AKNormalZombieEnemy::EnemyAttack()
{
	Super::EnemyAttack();
}

void AKNormalZombieEnemy::EnemyDead()
{
	Super::EnemyDead();
}
