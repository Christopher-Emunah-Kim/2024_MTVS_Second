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

	class UStaticMeshComponent* MeshComp;
	class USceneComponent* SceneComp;

	void PullTrigger();
	void GunTrace(FHitResult &Hit, FVector& ShotDirection);
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
