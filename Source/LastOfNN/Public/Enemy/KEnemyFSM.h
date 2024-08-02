// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "KEnemyFSM.generated.h"

UENUM(BlueprintType)
enum class EEnemyState : uint8
{
    IDLE UMETA(DisplayName = "Idle"),
    PATROL UMETA(DisplayName = "Patrol"),
    DETECT UMETA(DisplayName = "Detect"),
    MOVE UMETA(DisplayName = "Move"),
    ATTACK UMETA(DisplayName = "Attack"),
    GRAB UMETA(DisplayName = "Grab"),
    EVADE UMETA(DisplayName = "Evade"),
    HIDE UMETA(DisplayName = "Hide"),
    TAKEDAMAGE UMETA(DisplayName = "TakeDamage"),
    KNOCKBACK UMETA(DisplayName = "KnockBack"),
    FINISHING UMETA(DisplayName = "Finishing"),
    DEAD UMETA(DisplayName = "Dead")
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class LASTOFNN_API UKEnemyFSM : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UKEnemyFSM();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;


public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
    
    //현재상태 저장변수. 기본상태는 IDLE로 한다.
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State")
    EEnemyState CurrentState = EEnemyState::IDLE;

    //전달받은 상태값 전환 함수
    UFUNCTION(BlueprintCallable, Category = "FSM")
    void SetState(EEnemyState NewState);

    //Tick마다 상태 업데이트 함수
    UFUNCTION(BlueprintCallable, Category = "FSM")
    void UpdateState();
	
    //BaseEnemy 인스턴스
    UPROPERTY(VisibleAnywhere, Category = "FSM")
    class AKBaseEnemy* BaseEnemy;

    //QTE이벤트 진행 여부 확인 플래그
    bool bIsQTEActive;

    //플레이어 캐릭터
    class AJPlayer* Player;
};
