// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/JPlayerShotGun.h"
#include "Player/JPlayer.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/DamageEvents.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
AJPlayerShotGun::AJPlayerShotGun()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	SceneComp = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	SetRootComponent(SceneComp);
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GunMesh"));
	MeshComp->SetupAttachment(SceneComp);
	MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

}

// Called when the game starts or when spawned
void AJPlayerShotGun::BeginPlay()
{
	Super::BeginPlay();
	Player = Cast<AJPlayer>(GetWorld()->GetFirstPlayerController()->GetPawn());
	PlayerCam = Player->CameraComp;
	
	CurrentBulletNum = MaxBulletNum;
}

// Called every frame
void AJPlayerShotGun::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AJPlayerShotGun::PullTrigger()
{
	if ( CurrentBulletNum == 0 )
	{
		return;
	}
	FVector startPos = PlayerCam->GetComponentLocation();
	FVector forwardVector = PlayerCam->GetForwardVector();

	float horizontalAngleStep = 4.0f; //수평각도 간격
	float verticalAngleStep = 4.0f;   //수직각도 간격
	int32 numTraces = 8; //라인트레이스 갯수

	//10개의 라인트레이스선이 뻗어나가도록 생성(샷건모드)
	for ( int32 i = 0; i < numTraces; i++ )
	{
		//무작위각도를 생성하여 원뿔형태로 퍼지도록 함
		float randomYaw = FMath::RandRange(-horizontalAngleStep, horizontalAngleStep);
		float randomPitch = FMath::RandRange(-verticalAngleStep, verticalAngleStep);

		//각도를 회전벡터로 변환
		FRotator randomRotator = FRotator(randomPitch, randomYaw, 0.0f);
		FVector rotatedVector = randomRotator.RotateVector(forwardVector);
		FVector endPos = startPos + (rotatedVector * BaseWeaponRange);

		//충돌정보 저장
		FHitResult hitInfo;
		FCollisionQueryParams params;
		//나 자신은 충돌 무시
		params.AddIgnoredActor(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));

		//라인트레이스실행 및 설정에 따른 충돌 및 데미지 적용
		bool bHit = GetWorld()->LineTraceSingleByChannel(hitInfo, startPos, endPos, ECollisionChannel::ECC_Pawn, params);

		if ( bHit )
		{
			// 충돌 처리 -> 총알 파편 효과 재생
			FTransform bulletTrans;
			bulletTrans.SetLocation(hitInfo.ImpactPoint);
			//UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), bulletEffectFactory, bulletTrans);

			// 적중한 액터에 데미지 적용
			FDamageEvent DamageEvent;
			AActor* HitActor = hitInfo.GetActor();
			AController* OwnerController = GetWorld()->GetFirstPlayerController();

			HitActor->TakeDamage(10, DamageEvent, OwnerController, this);
			UE_LOG(LogTemp, Error, TEXT("Hitactor : %s"), *HitActor->GetName());

			// 피격 물체 날려보내기 구현
			auto* hitComp = hitInfo.GetComponent();
			if ( hitComp && hitComp->IsSimulatingPhysics() )
			{
				FVector force = -hitInfo.ImpactNormal * hitComp->GetMass() * 3000;
				hitComp->AddForce(force);
			}
		}
	}
	CurrentBulletNum--;
}

