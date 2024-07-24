// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/PlayerGun.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
APlayerGun::APlayerGun()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	SceneComp = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	SceneComp = RootComponent;
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GunMesh"));
	MeshComp->SetupAttachment(SceneComp); 
}

void APlayerGun::PullTrigger()
{

}

void APlayerGun::GunTrace(FHitResult& Hit, FVector& ShotDirection)
{
}

// Called when the game starts or when spawned
void APlayerGun::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APlayerGun::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

