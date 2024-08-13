// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy/KBaseEnemy.h"
#include "GenericTeamAgentInterface.h"
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

	//=======================================================================================
	// Functions
	//=======================================================================================

	virtual void EnemySetState(EEnemyState newstate) override;

    virtual void EnemyIDLE() override;

    virtual void EnemyMove() override;

	void EnemyRandomMove(); //Boss랜덤이동함수

    virtual void EnemyAttack() override;

	virtual void EnemySpecialAttack() override;

	void BossThrowGrenade(); //Boss수류탄 공격 함수

	virtual void EnemyOverlapDamage(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	virtual void OnEnemyDamageProcess(float damage) override;

	virtual void EnemyTakeDamage() override;

    virtual void EnemyDead() override;


	////IGenericTeamAgentInterface 인터페이스 구현
	//virtual FGenericTeamId GetGenericTeamId() const override;
	////팀별 태도 결정
	//virtual ETeamAttitude::Type GetTeamAttitudeTowards(const AActor& Other) const override;

	//=======================================================================================
	// Instance, Variables
	//=======================================================================================

	//Boss 수류탄 인스턴스
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grenade")
	TSubclassOf<class AKBossZombieGrenade> BossGrenade;

	//Boss 수류탄 공격체크
	bool isBossCanThrowGrenade = false;
	
	//Boss 수류탄 발사주기
	float BossGrenadeDelayTime = 7.0f;

	//SPECIL Attack SFX 변수
	UPROPERTY(EditDefaultsOnly)
	class USoundBase* BossGrenadeSFXFactory;

};
