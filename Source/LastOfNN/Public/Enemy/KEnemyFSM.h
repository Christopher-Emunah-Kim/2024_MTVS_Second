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
    
    //���� ���� ����
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State")
    EEnemyState CurrentState;

    //���޹��� ���°� ��ȯ �Լ�
    UFUNCTION(BlueprintCallable, Category = "FSM")
    void SetState(EEnemyState NewState);

    //Tick���� ���� ������Ʈ �Լ�
    UFUNCTION(BlueprintCallable, Category = "FSM")
    void UpdateState();
	
    //BaseEnemy �ν��Ͻ�
    UPROPERTY(VisibleAnywhere, Category = "FSM")
    class AKBaseEnemy* BaseEnemy;

};
