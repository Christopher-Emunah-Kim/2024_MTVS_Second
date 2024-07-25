// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/PlayerLockOn.h"
#include "Kismet/GameplayStatics.h"
#include "Player/JPlayer.h"
#include "Camera/CameraComponent.h"
#include "DrawDebugHelpers.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values for this component's properties
UPlayerLockOn::UPlayerLockOn()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

void UPlayerLockOn::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	LockOnTarget();
}


void UPlayerLockOn::LockOnTarget()
{
	if (TargetLock)
	{
		FHitResult HitResult;


		Player = Cast<AJPlayer>(GetWorld()->GetFirstPlayerController()->GetPawn());
		FVector StartLocation = Player->GetActorLocation();

		FVector ViewLocation;
		FRotator ViewRotate;
		Player->GetController()->GetPlayerViewPoint(ViewLocation, ViewRotate);
		
		PlayerCam = Player->GetComponentByClass<UCameraComponent>();
		FVector EndLocation = StartLocation + VectorCaculationNum * (PlayerCam->GetForwardVector());

		TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
		ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn)); //일단 폰 감지
		
		TArray<AActor*> ActorsToIgnore;
		ActorsToIgnore.Add(Player); //자기 자신 무시하기

		bool bHit = UKismetSystemLibrary::SphereTraceSingleForObjects(
			GetWorld(),
			StartLocation,
			EndLocation,
			Radius,
			ObjectTypes,
			false,
			ActorsToIgnore,
			EDrawDebugTrace::None,
			HitResult,
			false);

		if (bHit)
		{
			HitActor = HitResult.GetActor();
			//UE_LOG(LogTemp, Error, TEXT("Hit Actor: %s"), *HitActor->GetName());
			FRotator Rotate = UKismetMathLibrary::FindLookAtRotation(ViewLocation, HitActor->GetActorLocation());;
			Player->GetController()->SetControlRotation(Rotate);
		}
	}
}

void UPlayerLockOn::SetTargetLockTrue()
{
	TargetLock = true;
}

void UPlayerLockOn::SetTargetLockFalse()
{
	TargetLock = false;
}

// Called when the game starts
void UPlayerLockOn::BeginPlay()
{
	Super::BeginPlay();

		
}
