// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "KBossZombieGrenade.generated.h"

UCLASS()
class LASTOFNN_API AKBossZombieGrenade : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AKBossZombieGrenade();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


public:

	//플레이어 Target 정보 인스턴스
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grenade")
    class AJPlayer* target;

	//보스Enemy 정보 인스턴스
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grenade")
    class AKBossZombieEnemy* boss;

	UPROPERTY(VisibleDefaultsOnly, Category = "Grenade")
	class USphereComponent* CollisionComp;

	UPROPERTY(VisibleDefaultsOnly, Category = "Grenade")
	class UStaticMeshComponent* bodyMeshComp;

	UPROPERTY(VisibleDefaultsOnly, Category = "Grenade")
	class UProjectileMovementComponent* ProjectileComp;

	

	//Boss 수류탄 이펙트
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grenade")
	class UNiagaraSystem* BossGrenadeVFX;

	//Boss가 호출할 발사함수
	void BossFireInDirection(const FVector& ShootDirection);

	//전달받은 Direction값으로 발사 하는 내부함수
	void OnMyThrowGrenade(const FVector& ShootDirection);

	//충돌처리함수
	UFUNCTION()
	void GrenadeOnHit(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	//데미지적용함수
	void OnDamageSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

};
