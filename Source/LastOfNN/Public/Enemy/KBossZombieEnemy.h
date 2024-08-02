// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy/KBaseEnemy.h"
#include "KBossZombieEnemy.generated.h"

/**
 * 
 */
UCLASS()
class LASTOFNN_API AKBossZombieEnemy : public AKBaseEnemy
{
	GENERATED_BODY()
	
public:
	AKBossZombieEnemy();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void EnemySetState(EEnemyState newstate) override;

    virtual void EnemyIDLE() override;

    virtual void EnemyMove() override;

    virtual void EnemyAttack() override;

	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	virtual void OnEnemyDamageProcess(float damage) override;

	virtual void EnemyTakeDamage() override;

    virtual void EnemyDead() override;


};
