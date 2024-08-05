// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Enemy//KBaseEnemy.h"
#include "JBurningField.generated.h"

UCLASS()
class LASTOFNN_API AJBurningField : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AJBurningField();

	UPROPERTY(EditAnywhere)
	class UParticleSystemComponent* FireParticle;
	UPROPERTY(EditAnywhere)
	class UBoxComponent* Box;
	UPROPERTY(EditAnywhere)
	class USceneComponent* Root;

	//데미지를 받을 액터들
	TArray<AActor*> HitActors;
	//데미지 주는 함수
	void DamageTick();

	//불 데미지
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
    float FireDamage = 10.0f;

    //1초마다 데미지 주게 하기
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
    float DamageInterval = 1.0f;

	//겹치는 거 인식
	UFUNCTION()
	void BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void EndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	//타이머관련
	FTimerHandle DamageTimerHandle;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI", meta = (AllowPrivateAccess = "true"))
    class UAIPerceptionStimuliSourceComponent* PerceptionStimuliSource;

	// 팀 타입
	ETeamType TeamType;

	// 팀 타입 반환 함수
	ETeamType GetTeamType() const;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void MakeSound();
	UPROPERTY(EditAnyWhere)
	USoundBase* LandingSound;
};
