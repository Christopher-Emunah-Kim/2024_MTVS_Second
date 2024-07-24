// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "PlayerLockOn.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class LASTOFNN_API UPlayerLockOn : public USceneComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UPlayerLockOn();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(EditAnywhere)
	float VectorCaculationNum = 500;
	UPROPERTY(EditAnywhere)
	float Radius = 125;

	UFUNCTION()
	void LockOnTarget();

	class UCameraComponent* PlayerCam;
	class AActor* HitActor;
	class AJPlayer* Player;

	UPROPERTY(EditAnywhere)
	bool TargetLock = false;

	void SetTargetLockTrue();
	void SetTargetLockFalse();
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame

		
};
