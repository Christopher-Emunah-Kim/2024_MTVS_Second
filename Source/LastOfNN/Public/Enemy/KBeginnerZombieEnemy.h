// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy/KBaseEnemy.h"
#include "GenericTeamAgentInterface.h"
#include "KBeginnerZombieEnemy.generated.h"

/**
 * 
 */
UCLASS()
class LASTOFNN_API AKBeginnerZombieEnemy : public AKBaseEnemy
{
	GENERATED_BODY()
	
	
public:
	AKBeginnerZombieEnemy();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void EnemySetState(EEnemyState newstate) override;

    virtual void EnemyIDLE() override;

	virtual void OnEnemyNoiseHeard(AActor* Actor, FAIStimulus Stimulus) override;

	virtual void OnEnemySightVision(const TArray<AActor*>& UpdatedActors) override;

	virtual void OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus) override; 

    virtual void EnemyMove() override;

	void EnemyRandomMove(); //Enemy 랜덤이동함수

    virtual void EnemyAttack() override;

	virtual void EnemyOverlapDamage(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

	virtual void EnemySpecialAttack() override;

	virtual void SetAllEnemiesToIdle() override;

	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	virtual void OnEnemyDamageProcess(float damage) override;

	virtual void EnemyTakeDamage() override;

	virtual void EnemyExecuted() override;

    virtual void EnemyDead() override;

	////IGenericTeamAgentInterface 인터페이스 구현
	//virtual FGenericTeamId GetGenericTeamId() const override;
	////팀별 태도 결정
	//virtual ETeamAttitude::Type GetTeamAttitudeTowards(const AActor& Other) const override;

	//암살 이벤트 사용 함수 변수
	UPROPERTY(EditAnywhere)
	class UBoxComponent* AssassinBox;
	UPROPERTY(VisibleAnywhere)
	class USceneComponent* AssassinSceneComp;

	FTimerHandle ExcecuteTimer;
	//플레이어에게 암살위치정보 전달
	FTransform GetAttackerTransform();

	//SPECIL Attack SFX 변수
	UPROPERTY(EditDefaultsOnly)
	class USoundBase* GrabSFXFactory;

	//암살 당할 시 SFX 변수
	UPROPERTY(EditDefaultsOnly)
	class USoundBase* ExcecutedSFXFactory;
};
