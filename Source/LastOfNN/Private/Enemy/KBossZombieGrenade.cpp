// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/KBossZombieGrenade.h"
#include "Enemy/KBaseEnemy.h"
#include "Enemy/KBossZombieEnemy.h"
#include "Player/JPlayer.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "../../../../../../../Program Files/Epic Games/UE_5.4/Engine/Plugins/FX/Niagara/Source/Niagara/Classes/NiagaraSystem.h" 
#include "../../../../../../../Program Files/Epic Games/UE_5.4/Engine/Plugins/FX/Niagara/Source/Niagara/Public/NiagaraComponent.h" 
#include "../../../../../../../Program Files/Epic Games/UE_5.4/Engine/Plugins/FX/Niagara/Source/Niagara/Public/NiagaraFunctionLibrary.h"
#include <Kismet/GameplayStatics.h>

// Sets default values
AKBossZombieGrenade::AKBossZombieGrenade()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CollisionComp->InitSphereRadius(15.0f);
	SetRootComponent(CollisionComp);
	//충돌처리
	CollisionComp->OnComponentHit.AddDynamic(this, &AKBossZombieGrenade::GrenadeOnHit);

	bodyMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	bodyMeshComp->SetupAttachment(CollisionComp);
	bodyMeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	bodyMeshComp->SetRelativeScale3D(FVector(0.25f));

	ProjectileComp = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComp"));
	ProjectileComp->SetUpdatedComponent(CollisionComp);
	ProjectileComp->InitialSpeed = 2000.0f;
	ProjectileComp->MaxSpeed = 3000.0f;
	ProjectileComp->bRotationFollowsVelocity = true;
	ProjectileComp->bShouldBounce = true;
	ProjectileComp->Bounciness = 0.3f;


}

// Called when the game starts or when spawned
void AKBossZombieGrenade::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AKBossZombieGrenade::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AKBossZombieGrenade::BossFireInDirection(const FVector& ShootDirection)
{
	check(target);
	if ( nullptr == target )
	{
		return;
	}
	//전달받은 방향으로 수류탄 발사
	ProjectileComp->Velocity = ShootDirection * ProjectileComp->InitialSpeed;
}

void AKBossZombieGrenade::GrenadeOnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// 충돌한 액터가 JPlayer 타입이거나 다른 오브젝트인 경우
	if ( OtherActor && (OtherActor->IsA(AJPlayer::StaticClass()) || OtherActor) )
	{
		// 수류탄 발사체 제거
		this->Destroy();


		//나이아가라 이펙트 생성
		if ( BossGrenadeVFX )
		{
			UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), BossGrenadeVFX, GetActorLocation());
		}

		//임시충돌체 생성
		USphereComponent* DamageSphere = NewObject<USphereComponent>(this);
		DamageSphere->InitSphereRadius(100.0f);
		DamageSphere->SetCollisionProfileName(TEXT("OverlapAll"));
		DamageSphere->OnComponentBeginOverlap.AddDynamic(this, &AKBossZombieGrenade::OnDamageSphereOverlap);

		//데미지 부여함수 발동
		DamageSphere->RegisterComponent();
		DamageSphere->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepWorldTransform);

		//일정 시간 후 충돌체 제거
		FTimerHandle GrenadeZoneTimer;
		GetWorld()->GetTimerManager().SetTimer(GrenadeZoneTimer, [DamageSphere]()
		{
				DamageSphere->DestroyComponent();
		}, 3.0f, false);
	}
}

void AKBossZombieGrenade::OnDamageSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// 충돌체에 겹친 액터가 Player이면 데미지 적용
	if ( OtherActor && OtherActor->IsA(AJPlayer::StaticClass()) )
	{
		check(target);
		if ( target )
		{
			UGameplayStatics::ApplyDamage(target, boss->BossGrenadeAttackDamage, nullptr, this, UDamageType::StaticClass());
		}
	}
}



