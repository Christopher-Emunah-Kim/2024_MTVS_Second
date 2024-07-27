// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/JBurningField.h"
#include "Particles/ParticleSystemComponent.h"
#include "Components/BoxComponent.h"
#include "Kismet/KismetArrayLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/DamageEvents.h"

// Sets default values
AJBurningField::AJBurningField()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	FireParticle = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("FireParticle"));
	FireParticle->SetupAttachment(GetRootComponent());
	Box = CreateDefaultSubobject<UBoxComponent>(TEXT("Box"));
	Box->SetupAttachment(GetRootComponent());

}

// Called when the game starts or when spawned
void AJBurningField::BeginPlay()
{
	Super::BeginPlay();
	
	//불필드 안에 들어온 애들 배열 만들어서
	Box->OnComponentBeginOverlap.AddDynamic(this, &AJBurningField::BeginOverlap);
	//불필드 밖으로 나가면 배열에서 빼주고
	Box->OnComponentEndOverlap.AddDynamic(this, &AJBurningField::EndOverlap);
	//배열 안에 있는 액터들 데미지 가함
	GetWorld()->GetTimerManager().SetTimer(DamageTimerHandle, this, &AJBurningField::DamageTick, DamageInterval, true);
}

void AJBurningField::DamageTick()
{
	for ( AActor* Actor : HitActors )
	{
		if ( Actor )
		{
			FPointDamageEvent DamageEvent(FireDamage, FHitResult(), GetActorForwardVector(), nullptr);
			AController* ActorController = nullptr;
			//불 필드 위에 있는 애들 컨트롤러 얻어와서 데미지 주기
			APawn* ActorPawn = Cast<APawn>(Actor);
			if ( ActorPawn )
			{
				ActorController = ActorPawn->GetController();
			}
			Actor->TakeDamage(FireDamage, DamageEvent, ActorController, this);

			UE_LOG(LogTemp, Log, TEXT("Applied %f damage to %s"), FireDamage, *Actor->GetName());
		}
	}
}

void AJBurningField::BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if ( OtherActor && OtherActor != this )
	{
		HitActors.AddUnique(OtherActor); //데미지를 입을 액터들
	}
}

void AJBurningField::EndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	//불필드 밖으로 나가면 데미지를 안 입게 하고 싶다.
	if ( OtherActor && OtherActor != this )
	{
		int32 Index = HitActors.Find(OtherActor);
		if ( Index != INDEX_NONE )
		{
			HitActors.RemoveAt(Index); 
		}
	}
}
// Called every frame
void AJBurningField::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

