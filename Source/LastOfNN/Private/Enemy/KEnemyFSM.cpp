// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/KEnemyFSM.h"
#include "Enemy/KBaseEnemy.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "Player/JPlayer.h"



// Sets default values for this component's properties
UKEnemyFSM::UKEnemyFSM()
{
    // Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
    // off to improve performance if you don't need them.
    PrimaryComponentTick.bCanEverTick = true;

    
}


// Called when the game starts
void UKEnemyFSM::BeginPlay()
{
    Super::BeginPlay();

    //QTE이벤트 진행 여부 초기화
    bIsQTEActive = false;

    // BaseEnemy 초기화
    BaseEnemy = Cast<AKBaseEnemy>(GetOwner());
    if (!BaseEnemy)
    {
        UE_LOG(LogTemp, Error, TEXT("KEnemyFSM: BaseEnemy is not properly initialized."));
    }

    Player = Cast<AJPlayer>(GetWorld()->GetFirstPlayerController()->GetPawn());
}


// Called every frame
void UKEnemyFSM::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    //실행창에 현재 상태 출력하기
    FString logMsg = UEnum::GetValueAsString(CurrentState);
    GEngine->AddOnScreenDebugMessage(0,1, FColor::Blue, logMsg);


    // QTE 이벤트가 진행 중이면 모든 Enemy의 상태를 IDLE로 유지 -> Player->GetIsGrabbed()로 잠깐 바꿈
    if ( Player->GetIsGrabbed() && CurrentState != EEnemyState::GRAB )
    {
        SetState(EEnemyState::IDLE);
        //UE_LOG(LogTemp, Warning, TEXT("IDLE!!!!!!!!"));
    }
    else if (Player->GetIsExecuting())
    {
        SetState(EEnemyState::IDLE);
    }
    
    else
    {
        //그외엔 계속 상태 업데이트
        UpdateState();
        //UE_LOG(LogTemp, Warning, TEXT("STATE UPDATE!!!!"));
    }
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
        BaseEnemy->EnemyIDLE();
        break;
    case EEnemyState::PATROL:
        // Patrol 상태 행동 처리...
        break;
    case EEnemyState::DETECT:
        // Detect 상태 행동 처리...
        break;
    case EEnemyState::MOVE:
        // Move 상태 행동 처리...
        BaseEnemy->EnemyMove();
        break;
    case EEnemyState::ATTACK:
        // Attack 상태 행동 처리...
        BaseEnemy->EnemyAttack();
        break;
    case EEnemyState::GRAB:
        // Grab 상태 행동 처리...
        BaseEnemy->EnemyGrab();
        break;
    case EEnemyState::EVADE:
        // Evade 상태 행동 처리...
        break;
    case EEnemyState::HIDE:
        // Hide 상태 행동 처리...
        break;
    case EEnemyState::TAKEDAMAGE:
        // TakeDamage 상태 행동 처리...
        BaseEnemy->EnemyTakeDamage();
        break;
    case EEnemyState::KNOCKBACK:
        // Finishing 상태 행동 처리...
        break;
    case EEnemyState::FINISHING:
        // Finishing 상태 행동 처리...
        break;    
    case EEnemyState::EXECUTED:
        // Executed 상태 행동 처리...
        BaseEnemy->EnemyExecuted();
        break;  
    case EEnemyState::DEAD:
        // Dead 상태 행동 처리...
        BaseEnemy->EnemyDead();
        break;
    }
}

