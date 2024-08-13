// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/JPlayerBat.h"
#include "Engine/DamageEvents.h"
#include "Components/SceneComponent.h"

#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Player/JPlayer.h"

// Sets default values
AJPlayerBat::AJPlayerBat()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	Bat = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Bat"));
	Bat->SetupAttachment(RootComponent);
	Bat->SetCollisionProfileName(TEXT("OverlapAll"));
	Bat->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	HitCapsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("HitCapsule"));
	HitCapsule->SetupAttachment(RootComponent);
	HitCapsule->SetRelativeLocation(FVector(61.189181f, -31.936999f, 13.889282f));
	HitCapsule->SetRelativeRotation(FRotator(-82, 10, 334));
	HitCapsule->SetWorldScale3D(FVector(1, 1, 1.6f));
	HitCapsule->SetCollisionEnabled(ECollisionEnabled::NoCollision);


}

// Called when the game starts or when spawned
void AJPlayerBat::BeginPlay()
{
	Super::BeginPlay();
	
	HitCapsule->OnComponentBeginOverlap.AddDynamic(this, &AJPlayerBat::OverlapDamage);

}

// Called every frame
void AJPlayerBat::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AJPlayerBat::OverlapDamage(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UGameplayStatics::PlaySound2D(this, PipeSound);
	UGameplayStatics::PlaySound2D(this, HitSound);
	if ( OtherActor && OtherActor != GetWorld()->GetFirstPlayerController()->GetPawn() && OtherActor != this )
	{
		//일단 빠따 데미지 20으로 설정
		FPointDamageEvent DamageEvent(10, FHitResult(), GetActorForwardVector(), nullptr);
		AController* ActorController = nullptr;
		//애들 컨트롤러 얻어와서 데미지 주기
		APawn* ActorPawn = Cast<APawn>(OtherActor);
		if ( ActorPawn )
		{
			ActorController = ActorPawn->GetController();
		}
		OtherActor->TakeDamage(10, DamageEvent, ActorController, this);
		MakeTimeSlow(0.75f);
		//if ( GetOwner() )
		//{
		//	auto* Player = Cast<AJPlayer>(GetOwner());
		//	if ( Player )
		//	{
		//		UE_LOG(LogTemp, Error, TEXT("ZOPBEDF"));
		//		Player->CameraShake();
		//	}
		//}
		UE_LOG(LogTemp, Log, TEXT("Applied 10 damage to %s"), *OtherActor->GetName());
	}
}

void AJPlayerBat::MakeTimeSlow(float SlowRate)
{
	UGameplayStatics::SetGlobalTimeDilation(this, SlowRate);
	GetWorld()->GetTimerManager().SetTimer(TimeDilationHandle, this, &AJPlayerBat::ResetTimeDilation, 0.2f, false);
}

void AJPlayerBat::ResetTimeDilation()
{
	UGameplayStatics::SetGlobalTimeDilation(this, 1.0f);
}