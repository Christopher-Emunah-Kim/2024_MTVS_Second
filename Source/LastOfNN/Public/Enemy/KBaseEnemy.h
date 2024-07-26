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
	
	//FSM 인스턴스
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "FSM")
    UKEnemyFSM* FSMComponent;

	//플레이어 Target 정보 인스턴스
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FSM")
    class AJPlayer* target;

	//사용중인 에니메이션 BP 인스턴스
	UPROPERTY()
	class UKEnemyAnim* anim;

#pragma region virtual function with properties
	
	//기본 가상함수 및 필요 속성
	
    //**대기상태처리함수
	virtual void EnemyIDLE();
	//대기시간
	UPROPERTY(EditDefaultsOnly, Category = "FSM")
	float IdleDelayTime = 2.0f;
	//경과시간
	float CurrentTime = 0;


	//**이동상태처리함수
    virtual void EnemyMove();
	//걷기속도
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FSM")
    float EnemyWalkSpeed;
	//뛰기속도
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FSM")
    float EnemyRunSpeed;
	//방향
    FVector EnemyDirection;

	//**공격상태처리함수
    virtual void EnemyAttack();
	//Enemy탐지범위
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FSM")
    float EnemyNoticeRange;
	//Enemy데미지
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FSM")
    float EnemyAttackDamage;
	//Enemy공격범위
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FSM")
    float EnemyAttackRange;
	//Enemy공격대기시간
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FSM")
    float EnemyAttackDelayTime;


	//피격알림이벤트함수
	float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	virtual void OnEnemyDamageProcess(float damage);
	//Enemy HP
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FSM")
    int32 EnemyHP;

	//**피격상태처리함수
	virtual void EnemyTakeDamage();
	//피격 후 IDLE전환 대기시간
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FSM")
    float EnemyTDamageDelayTime = 1.5f;

	//**죽음상태처리함수
    virtual void EnemyDead();
	//죽음 후 메시 내려가는 속도
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FSM")
    float DieDownfallSpeed = 50.0f;


#pragma endregion
};
