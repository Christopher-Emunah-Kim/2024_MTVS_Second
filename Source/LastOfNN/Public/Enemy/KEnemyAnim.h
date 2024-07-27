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
	
	//FSM 열거형 타입 인스턴스
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = FSM)
	EEnemyState EnemyAnimState;

	//공격상태 재생할지 여부
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = FSM)
	bool bEnemyAttackPlay = false;

	//AttackEnd 노티파이 이벤트 함수
	UFUNCTION(BlueprintCallable, Category = FSMEvent)
	void OnEnemyEndAttackAnimation();

	//피격 애니메이션 몽타주 재생 함수
	UFUNCTION(BlueprintImplementableEvent, Category = FSMEvent)
	void PlayEnemyTDamageAnim(FName sectionName);

};
