// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Enemy/KEnemyFSM.h"
#include "KBaseEnemy.generated.h"

UCLASS()
class LASTOFNN_API AKBaseEnemy : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AKBaseEnemy();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	//virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	//Enemy Status관련 변수 선언
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Stats")
    float EnemyHealth;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Stats")
    float EnemyWalkSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Stats")
    float EnemyRunSpeed;

	//Enemy 이동, 공격 사용 Vector변수
    FVector EnemyDirection;

	//Enemy 공격 사용 변수
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Attack")
    float EnemyNoticeRange;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Attack")
    float EnemyDamage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Attack")
    class AJoel* target;

	//FSM 인스턴스
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "FSM")
    UKEnemyFSM* FSMComponent;

	//Base기본 가상함수
    virtual void EnemyIDLE();

    virtual void EnemyMove();

	virtual float TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

    virtual void EnemyAttack();

    virtual void EnemyDead();

};
