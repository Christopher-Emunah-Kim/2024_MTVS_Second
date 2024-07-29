// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "JCharacterAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class LASTOFNN_API UJCharacterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
public:

	UJCharacterAnimInstance();

	virtual void NativeInitializeAnimation() override;

	UPROPERTY(BlueprintReadOnly, Category = Movement)
	class UCharacterMovementComponent* CharacterMovement;
	UPROPERTY(EditAnywhere)
	class AJPlayer* Player;
	UPROPERTY(BlueprintReadOnly, Category = Movement)
	float GroundSpeed;
	UPROPERTY(BlueprintReadOnly, Category = Movement)
	bool bIsFalling;

	virtual void NativeUpdateAnimation(float DeltaTime) override;
};
