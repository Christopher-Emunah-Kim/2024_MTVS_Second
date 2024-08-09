// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "JPlayerShotGun.generated.h"

UCLASS()
class LASTOFNN_API AJPlayerShotGun : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AJPlayerShotGun();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere)
	class UStaticMeshComponent* MeshComp;
	UPROPERTY(EditAnywhere)
	class USceneComponent* SceneComp;

	class UCameraComponent* PlayerCam;

	void PullTrigger();

	float BaseWeaponRange = 2000;

	int32 CurrentBulletNum;
	int32 MaxBulletNum = 8;

	class AJPlayer* Player;

};
