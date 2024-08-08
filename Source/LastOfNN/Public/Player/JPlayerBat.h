// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "JPlayerBat.generated.h"

UCLASS()
class LASTOFNN_API AJPlayerBat : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AJPlayerBat();

	UPROPERTY(EditAnywhere)
	class UStaticMeshComponent* Bat;	
	UPROPERTY(EditAnywhere)
	class USceneComponent* Root;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UCapsuleComponent* HitCapsule;

	TArray<AActor*> HitActors;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void OverlapDamage(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

};
