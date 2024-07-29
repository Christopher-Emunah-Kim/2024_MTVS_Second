// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "JCharacterAnimInstance.generated.h"

/**
 * 
 */
DECLARE_MULTICAST_DELEGATE(FOnNextAttackCheckDelegate);

UCLASS()
class LASTOFNN_API UJCharacterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
public:

	UJCharacterAnimInstance();

	virtual void NativeInitializeAnimation() override;

	//캐릭터 움직임
	UPROPERTY(BlueprintReadOnly, Category = Movement)
	class UCharacterMovementComponent* CharacterMovement;


	class AJPlayer* Player;
	UPROPERTY(BlueprintReadOnly, Category = Movement)
	float GroundSpeed;
	UPROPERTY(BlueprintReadOnly, Category = Movement)
	bool bIsFalling;

	//연속공격
	FOnNextAttackCheckDelegate OnNextAttackCheck;

	UFUNCTION()
	void JumpToAttackMontageSection(int32 NewSection);

	UFUNCTION()
	FName GetAttackMontageSectionName(int32 Section);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	UAnimMontage* AttackMontage;

	virtual void NativeUpdateAnimation(float DeltaTime) override;

	void PlayAttackMontage();
private:
	UFUNCTION()
	void AnimNotify_NextAttackCheck();
};
