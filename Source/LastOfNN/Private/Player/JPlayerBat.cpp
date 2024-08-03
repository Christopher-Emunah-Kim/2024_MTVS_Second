// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/JPlayerBat.h"
#include "Engine/DamageEvents.h"
#include "Components/SceneComponent.h"

// Sets default values
AJPlayerBat::AJPlayerBat()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	Bat = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Bat"));
	Bat->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void AJPlayerBat::BeginPlay()
{
	Super::BeginPlay();
	
	Bat->OnComponentBeginOverlap.AddDynamic(this, &AJPlayerBat::OverlapDamage);
}

// Called every frame
void AJPlayerBat::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AJPlayerBat::OverlapDamage(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if ( OtherActor && OtherActor != this )
	{
		HitActors.AddUnique(OtherActor); //데미지를 입을 액터들
		UE_LOG(LogTemp, Log, TEXT("Applied"));
	}
	for ( AActor* Actor : HitActors )
	{
		if ( Actor )
		{
			//일단 빠따 데미지 20으로 설정
			FPointDamageEvent DamageEvent(20, FHitResult(), GetActorForwardVector(), nullptr);
			AController* ActorController = nullptr;
			//애들 컨트롤러 얻어와서 데미지 주기
			APawn* ActorPawn = Cast<APawn>(Actor);
			if ( ActorPawn )
			{
				ActorController = ActorPawn->GetController();
			}
			Actor->TakeDamage(20, DamageEvent, ActorController, this);

			UE_LOG(LogTemp, Log, TEXT("Applied 10 damage to %s"), *Actor->GetName());
		}
	}
}