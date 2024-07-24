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

	//Enemy Status 관련 변수 선언
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
    float EnemyAttackDamage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Attack")
    class AJPlayer* target;

	//FSM 인스턴스
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "FSMComp")
    UKEnemyFSM* FSMComponent;

	//Base기본 가상함수 및 필요 속성
    //대기상태처리함수
	virtual void EnemyIDLE();
	//대기시간
	UPROPERTY(EditDefaultsOnly, Category = "FSMComp")
	float IdleDelayTime = 2.0f;
	//경과시간
	float CurrentTime = 0;

    virtual void EnemyMove();

	virtual void EnemyDamage();

    virtual void EnemyAttack();

    virtual void EnemyDead();

};
