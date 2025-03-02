﻿// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "CharacterTypes.h"
#include "JCharacterAnimInstance.generated.h"

/**
 * 
 */
DECLARE_MULTICAST_DELEGATE(FOnNextAttackCheckDelegate);
DECLARE_MULTICAST_DELEGATE(FOnAttackHitCheckDelegate);

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
	UPROPERTY(BlueprintReadOnly, Category = Movement)
	float Angle;

	//연속공격
	FOnNextAttackCheckDelegate OnNextAttackCheck;
	FOnNextAttackCheckDelegate OnAttackHitCheck;

	UFUNCTION()
	void JumpToAttackMontageSection(int32 NewSection);

	UFUNCTION()
	FName GetAttackMontageSectionName(int32 Section);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	UAnimMontage* AttackMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Resist, Meta = (AllowPrivateAccess = true))
	UAnimMontage* ResistanceMontage;	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Resist, Meta = (AllowPrivateAccess = true))
	UAnimMontage* ExecuteMontage;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Resist, Meta = (AllowPrivateAccess = true))
	UAnimMontage* GunShotMontage;	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Resist, Meta = (AllowPrivateAccess = true))
	UAnimMontage* BatMontage;	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Resist, Meta = (AllowPrivateAccess = true))
	UAnimMontage* DieMontage;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Resist, Meta = (AllowPrivateAccess = true))
	UAnimMontage* HitMontage;	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Resist, Meta = (AllowPrivateAccess = true))
	UAnimMontage* EquipMontage;	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Resist, Meta = (AllowPrivateAccess = true))
	UAnimMontage* ShotgunMontage;	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Resist, Meta = (AllowPrivateAccess = true))
	UAnimMontage* FallingMontage;

	virtual void NativeUpdateAnimation(float DeltaTime) override;

	void PlayAttackMontage();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ECharacterState AnimCharacterState;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ECharacterEquipState AnimCharacterEquipState;

	void PlayResistanceMontage();
	void StopResistanceMontage();
	void PlayResistanceReleaseSection();

	void PlayExecuteMontage();

	void PlayGunShotMontage();
	void PlayGunShotMontageSection(FName Section);
	void StopGunshotMontage();

	void PlayDieMontage();
	void PlayHitMontage();

	void PlayEquipAnimMontage(FName Section);
	//무기 바꿀때 못 움직이는 불값
	bool bChangingWeapon = false;
	bool GetChangingWeapon();

	void PlayShotgunMontage();

	bool bChangeSpringArmComp = false;

	void PlayFallingMontage();
private:

	UFUNCTION()
	void AnimNotify_NextAttackCheck();	
	UFUNCTION()
	void AnimNotify_AttackHitCheck();
	UFUNCTION()
	void AnimNotify_StopMove();	
	UFUNCTION()
	void AnimNotify_Move();

	UFUNCTION()
	void AnimNotify_MoveCamera();
	UFUNCTION()
	void AnimNotify_CameraBack();

};

