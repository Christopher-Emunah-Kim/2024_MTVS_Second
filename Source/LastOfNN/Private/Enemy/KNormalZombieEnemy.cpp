// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/KNormalZombieEnemy.h"

AKNormalZombieEnemy::AKNormalZombieEnemy()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//외관 세팅
	ConstructorHelpers::FObjectFinder<USkeletalMesh> tempMesh(TEXT("/Script/Engine.SkeletalMesh'/Game/Assests/Enemy/Mesh/NormalZombie/01/T-Pose.T-Pose'"));
	if (tempMesh.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(tempMesh.Object);
		GetMesh()->SetRelativeLocationAndRotation(FVector(0,0, -88), FRotator(0,-90,0));
	}

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
