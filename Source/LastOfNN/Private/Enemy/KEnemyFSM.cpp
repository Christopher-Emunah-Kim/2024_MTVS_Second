// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/KEnemyFSM.h"
#include "Enemy/KBaseEnemy.h"

// Sets default values for this component's properties
UKEnemyFSM::UKEnemyFSM()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	//기본상태는 IDLE로 한다.
	CurrentState = EEnemyState::IDLE;
}


// Called when the game starts
void UKEnemyFSM::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UKEnemyFSM::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	//계속해서 상태를 업데이트
	UpdateState();
}

void UKEnemyFSM::SetState(EEnemyState NewState)
{
	//입력받은 상태값으로 전환
	CurrentState = NewState;
}

void UKEnemyFSM::UpdateState()
{
    switch (CurrentState)
    {
    case EEnemyState::IDLE:
        // Idle 상태 행동 처리...
        break;
    case EEnemyState::PATROL:
        // Patrol 상태 행동 처리...
        break;
    case EEnemyState::DETECT:
        // Detect 상태 행동 처리...
        break;
    case EEnemyState::MOVE:
        // Move 상태 행동 처리...
        break;
    case EEnemyState::ATTACK:
        // Attack 상태 행동 처리...
        break;
    case EEnemyState::EVADE:
        // Evade 상태 행동 처리...
        break;
    case EEnemyState::HIDE:
        // Hide 상태 행동 처리...
        break;
    case EEnemyState::TAKEDAMAGE:
        // TakeDamage 상태 행동 처리...
        break;
    case EEnemyState::KNOCKBACK:
        // Finishing 상태 행동 처리...
        break;
    case EEnemyState::FINISHING:
        // Finishing 상태 행동 처리...
        break;
    case EEnemyState::DEAD:
        // Dead 상태 행동 처리...
        break;
    }
}

