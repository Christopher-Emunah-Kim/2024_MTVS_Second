// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "JPlayer.generated.h"

class UCameraComponent;
class USpringArmComponent;
class UInputAction;
class UInputMappingContext;
class UPlayerLockOn;
class APlayerGun;

enum class ECharacterState : uint8
{
	ECS_Grabbed UMETA(DisplayName = "Grabbed"),
	ECS_Escape UMETA(DisplayName = "Escape"),
	ECS_NoGrabbed UMETA(DisplayName = "NoGrabbed")
};

enum class ECharacterEquipState : uint8
{
	ECES_UnEquipped UMETA(DisplayName = "UnEquipped"),
	ECES_GunEquipped UMETA(DisplayName = "GunEquipped"),
	ECES_BatEquipped UMETA(DisplayName = "BatEquipped"),
	ECES_ThrowWeaponEquipped UMETA(DisplayName = "ThrowWeaponEquipped"),
};


UCLASS()
class LASTOFNN_API AJPlayer : public ACharacter
{
	GENERATED_BODY()

public:

	ECharacterEquipState CharacterEquipState = ECharacterEquipState::ECES_UnEquipped;
	// Sets default values for this character's properties
	AJPlayer();

	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	//이동 방향

	UPROPERTY(EditAnywhere)
	UCameraComponent* CameraComp;	
	UPROPERTY(EditAnywhere)
	USpringArmComponent* SpringArmComp;

	UPROPERTY(EditAnywhere, Category = "Input")
	UInputMappingContext* IMC_Joel;
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* IA_Move;

	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* IA_Look;	
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* IA_Fire;
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* IA_Zoom;	
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* IA_Run;

	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void Fire(const FInputActionValue& Value);
	void Zoom(const FInputActionValue& Value);
	void Run(const FInputActionValue& Value);
	UCameraComponent* GetCamera();

	UPROPERTY()
	APlayerGun* Gun;

	UPlayerLockOn* LockOnComp;
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class APlayerGun> GunClass;

	//이동 관련
	FVector direction;
	UCharacterMovementComponent* CharacterMovement;
	bool bIsRunning = false;

	//연속공격
	class UJCharacterAnimInstance* CharacterAnimInstance;
	bool bIsComboInputOn;
	bool bIsAttacking;
	bool bCanNextCombo;
	float CurrentCombo;
	float MaxCombo = 2;
	UPROPERTY(EditAnywhere)
	class UAnimMontage* AttackMontage;

	void PostInitializeComponents() override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted);
	void AttackStartComboState();
	void AttackEndComboState();

};
