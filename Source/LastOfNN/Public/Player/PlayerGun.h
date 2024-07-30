// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PlayerGun.generated.h"

UCLASS()
class LASTOFNN_API APlayerGun : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APlayerGun();

	UPROPERTY(EditAnywhere)
	class UStaticMeshComponent* MeshComp;

	class USceneComponent* SceneComp;

	void PullTrigger();
	bool GunTrace(FHitResult& Hit, FVector& ShotDirection);

	float MaxRange = 1000;

	UPROPERTY(EditAnywhere)
	float GunDamage = 10;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//소리내기 관련
	void MakeSound();
	UPROPERTY(EditAnywhere)
	USoundBase* LandingSound;
};
