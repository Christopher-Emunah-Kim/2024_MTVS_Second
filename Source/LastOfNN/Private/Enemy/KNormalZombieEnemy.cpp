// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/KNormalZombieEnemy.h"

AKNormalZombieEnemy::AKNormalZombieEnemy()
{
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

float AKNormalZombieEnemy::TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);

	return Damage;
}

void AKNormalZombieEnemy::EnemyAttack()
{
	Super::EnemyAttack();
}

void AKNormalZombieEnemy::EnemyDead()
{
	Super::EnemyDead();
}
