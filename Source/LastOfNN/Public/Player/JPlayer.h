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


UCLASS()
class LASTOFNN_API AJPlayer : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AJPlayer();

	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(EditAnywhere)
	UCameraComponent* CameraComp;	
	UPROPERTY(EditAnywhere)
	USpringArmComponent* SpringArmComp;

	UPROPERTY(EditAnywhere, Category = "Input")
	UInputMappingContext* IMC_Joel;
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* IA_Move;

	FVector direction;
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* IA_Look;

	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


};
