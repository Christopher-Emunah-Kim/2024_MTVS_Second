// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "KEnemyFSM.h" //�߰�
#include "KEnemyAnim.generated.h"

/**
 * 
 */
UCLASS()
class LASTOFNN_API UKEnemyAnim : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	
	//FSM ������ Ÿ�� �ν��Ͻ�
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = FSM)
	EEnemyState EnemyAnimState;

	//���ݻ��� ������� ����
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = FSM)
	bool bEnemyAttackPlay = false;

};
