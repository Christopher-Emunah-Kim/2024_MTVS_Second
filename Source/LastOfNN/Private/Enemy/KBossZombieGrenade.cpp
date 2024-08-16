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
#include "Engine/DamageEvents.h"

// Sets default values
AKBossZombieGrenade::AKBossZombieGrenade()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CollisionComp->InitSphereRadius(150.0f);
	SetRootComponent(CollisionComp);
	CollisionComp->SetCollisionProfileName(TEXT("OverlapAll"));

	bodyMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	bodyMeshComp->SetupAttachment(CollisionComp);
	bodyMeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	bodyMeshComp->SetRelativeScale3D(FVector(0.3f));

	ProjectileComp = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComp"));
	ProjectileComp->SetUpdatedComponent(CollisionComp);
	ProjectileComp->InitialSpeed = 1000.0f;
	ProjectileComp->MaxSpeed = 1300.0f;
	ProjectileComp->ProjectileGravityScale = 1.0f;
	ProjectileComp->bRotationFollowsVelocity = true;
	ProjectileComp->bShouldBounce = true;
	ProjectileComp->Bounciness = 0.3f;

}

// Called when the game starts or when spawned
void AKBossZombieGrenade::BeginPlay()
{
	Super::BeginPlay();
	
	target = Cast<AJPlayer>(UGameplayStatics::GetActorOfClass(this, AJPlayer::StaticClass()));
	boss = Cast<AKBossZombieEnemy>(UGameplayStatics::GetActorOfClass(this, AKBossZombieEnemy::StaticClass()));

	CollisionComp->OnComponentBeginOverlap.AddDynamic(this, &AKBossZombieGrenade::GrenadeOnHit);
}

// Called every frame
void AKBossZombieGrenade::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AKBossZombieGrenade::BossThrowGrenade(const FVector& ShootDirection)
{
	if ( nullptr == target )
	{
		return;
	}

	CurrentTime += GetWorld()->DeltaTimeSeconds;
	if ( CurrentTime > 3.0f )
	{
		//포물선을 그리도록 수류탄에 UpVector추가
		FVector LaunchVelocity = ShootDirection * ProjectileComp->InitialSpeed;

		// SetVelocityInLocalSpace를 사용하여 발사체의 초기 속도를 설정
		ProjectileComp->SetVelocityInLocalSpace(LaunchVelocity);

		CurrentTime = 0;
	}
	

 //  // 발사지연타이머 설정
	//FTimerHandle LaunchTimerHandle;

	//// 1.2초 후에 발사되도록 타이머 설정
	//FTimerDelegate TimerDel;
	//TimerDel.BindLambda([this, ShootDirection]()
	//{
	//	 //포물선을 그리도록 수류탄에 UpVector추가
	//	FVector LaunchVelocity = ShootDirection * ProjectileComp->InitialSpeed;

	//	// SetVelocityInLocalSpace를 사용하여 발사체의 초기 속도를 설정
	//	ProjectileComp->SetVelocityInLocalSpace(LaunchVelocity);
	//});

	//GetWorld()->GetTimerManager().SetTimer(LaunchTimerHandle, TimerDel, 3.0f, false);
	//// 1.2초 후에 한 번만 실행되도록 설정

}

void AKBossZombieGrenade::GrenadeOnHit(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// 충돌한 액터가 JPlayer 타입이거나 다른 오브젝트인 경우
	if ( OtherActor && OtherActor != boss)
	{
		GEngine->AddOnScreenDebugMessage(7, 1, FColor::Green, FString::Printf(TEXT("Overlapped Actor : %s"), *OtherActor->GetName()));
		//나이아가라 이펙트 생성
		if ( BossGrenadeVFX )
		{
			UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), BossGrenadeVFX, GetActorLocation());
		}

		// 충돌체에 겹친 액터가 Player이면 데미지 적용
		if ( OtherActor && OtherActor->IsA(AJPlayer::StaticClass()) )
		{
			check(target);
			if ( target )
			{
				FPointDamageEvent DamageEvent(boss->EnemySpecialAttackDamage, FHitResult(), GetActorForwardVector(), nullptr);
				AController* PlayerController = target->GetController();
				
				target->TakeDamage(boss->EnemySpecialAttackDamage, DamageEvent,PlayerController, this);
			}
		}

		// 수류탄 발사체 제거
		this->Destroy();
	}
}
