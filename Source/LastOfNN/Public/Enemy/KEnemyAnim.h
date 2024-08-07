// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "KEnemyFSM.h" //추가
#include "KEnemyAnim.generated.h"

/**
 * 
 */
UCLASS()
class LASTOFNN_API UKEnemyAnim : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	
	//BeginPlay역할
	virtual void NativeInitializeAnimation() override;

	//FSM 열거형 타입 인스턴스
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = FSM)
	EEnemyState EnemyAnimState;

	//보스 인스턴스
	UPROPERTY()
	class AKBossZombieEnemy* boss;

	//몽타주 인스턴스
	UPROPERTY(EditDefaultsOnly)
	class UAnimMontage* EnemyMontage;

	//공격상태 재생할지 여부
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = FSM)
	bool bEnemyAttackPlay = false;

	//AttackEnd 노티파이 이벤트 함수
	UFUNCTION(BlueprintCallable, Category = FSMEvent)
	void OnEnemyEndAttackAnimation();

	//Boss 수류탄 스폰할지 여부
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = FSM)
	bool bBossThrowGrenade = false;

	//BossGrenadeThrow 노티파이 이벤트 함수
	/*UFUNCTION(BlueprintCallable, Category = FSMEvent)
	void OnBossThrowGrenadeAnimation();*/

	UFUNCTION()
	void AnimNotify_GrenadeShoot();

	// 애니메이션 end 노티파이 이벤트 함수
	UFUNCTION(BlueprintCallable, Category = FSMEvent)
	void OnEndAnimation(FName sectionName, int32 anistate);

	//피격, 죽음 애니메이션 몽타주 재생 함수
	UFUNCTION(BlueprintImplementableEvent, Category = FSMEvent)
	void PlayEnemyTDamageAnim(FName sectionName);

	//Grab 애니메이션 몽타주 재생 함수
	UFUNCTION(BlueprintImplementableEvent, Category = FSMEvent)
	void PlayEnemyGrabAnim(FName sectionName);

	//Boss수류탄 애니메이션 몽타주 재생 함수
	UFUNCTION(BlueprintImplementableEvent, Category = FSMEvent)
	void PlayBossEnemyGrenadeAnim(FName sectionName);

	//죽음애니메이션 종료여부 판단
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category=FSM)
	bool bEnemyDieDone = false;

	//BlendSpace Horizontal Speed
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FSM")
    float EnemyHSpeed;
	//BlendSpace Vertical Speed
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FSM")
    float EnemyVSpeed;
};
