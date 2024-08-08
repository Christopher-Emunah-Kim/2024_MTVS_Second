// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/PlayerGun.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/DamageEvents.h"
#include "Perception/AISense_Hearing.h"

// Sets default values
APlayerGun::APlayerGun()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	SceneComp = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	SetRootComponent(SceneComp);
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GunMesh"));
	MeshComp->SetupAttachment(SceneComp); 
	MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	CurrentBulletNum = MaxBulletNum;
}

void APlayerGun::PullTrigger()
{
	//if ( CurrentBulletNum == 0 )
	//{
	//	return;
	//}
	//걸린 액터에게 데미지
	FHitResult Hit;
	FVector ShotDirection;

	bool bSuccess = GunTrace(Hit, ShotDirection);
	if (bSuccess)
	{
		AActor* HitActor = Hit.GetActor();
		FPointDamageEvent DamageEvent(GunDamage, Hit, ShotDirection, nullptr); //데미지 이벤트 발생
		AController* OwnerController = GetWorld()->GetFirstPlayerController();
		Hit.GetActor()->TakeDamage(GunDamage, DamageEvent, OwnerController, this);
		UE_LOG(LogTemp, Error, TEXT("%f, %s"), GunDamage, *HitActor->GetName());
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("No Actor")); 
	}
	MakeSound();
	/*CurrentBulletNum--;*/
}

bool APlayerGun::GunTrace(FHitResult& Hit, FVector& ShotDirection)
{
	Hit;
	FVector Location;
	FRotator Rotation;
	//뷰포인트 얻어오기
	AController* OwnerController = GetWorld()->GetFirstPlayerController();
	if (OwnerController)
	{
		OwnerController->GetPlayerViewPoint(Location, Rotation);
	}

	ShotDirection = -Rotation.Vector();
	FVector EndLocation = Location + Rotation.Vector() * MaxRange;

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this); //총알이 총에 안맞게
	Params.AddIgnoredActor(GetOwner()); //총 주인이 총에 안맞게
	//라인트레이스 해서 걸리는 타겟 리턴
	DrawDebugPoint(GetWorld(), Hit.Location, 20, FColor::Red, true);
	return GetWorld()->LineTraceSingleByChannel(
		Hit,
		Location,
		EndLocation,
		ECollisionChannel::ECC_Pawn,
		Params
	);
}

void APlayerGun::ReLoad()
{
	CurrentBulletNum = MaxBulletNum;
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

void APlayerGun::MakeSound()
{
	// 소리 자극 발생시키기
	FVector NoiseLocation = GetActorLocation();
	float Loudness = 50.5f;  // 소리 강도 (예시 값)
	UGameplayStatics::PlaySoundAtLocation(this, LandingSound, NoiseLocation); // 착지 소리 재생
	UAISense_Hearing::ReportNoiseEvent(GetWorld(), NoiseLocation, Loudness, this, 10.0f, TEXT("ObjectLanding"));
}

