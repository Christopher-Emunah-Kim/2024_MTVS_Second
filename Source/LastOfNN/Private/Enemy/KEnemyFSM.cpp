// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/KEnemyFSM.h"
#include "Enemy/KBaseEnemy.h"

// Sets default values for this component's properties
UKEnemyFSM::UKEnemyFSM()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	//�⺻���´� IDLE�� �Ѵ�.
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

	//����ؼ� ���¸� ������Ʈ
	UpdateState();
}

void UKEnemyFSM::SetState(EEnemyState NewState)
{
	//�Է¹��� ���°����� ��ȯ
	CurrentState = NewState;
}

void UKEnemyFSM::UpdateState()
{
    switch (CurrentState)
    {
    case EEnemyState::IDLE:
        // Idle ���� �ൿ ó��...
        break;
    case EEnemyState::PATROL:
        // Patrol ���� �ൿ ó��...
        break;
    case EEnemyState::DETECT:
        // Detect ���� �ൿ ó��...
        break;
    case EEnemyState::MOVE:
        // Move ���� �ൿ ó��...
        break;
    case EEnemyState::ATTACK:
        // Attack ���� �ൿ ó��...
        break;
    case EEnemyState::EVADE:
        // Evade ���� �ൿ ó��...
        break;
    case EEnemyState::HIDE:
        // Hide ���� �ൿ ó��...
        break;
    case EEnemyState::TAKEDAMAGE:
        // TakeDamage ���� �ൿ ó��...
        break;
    case EEnemyState::KNOCKBACK:
        // Finishing ���� �ൿ ó��...
        break;
    case EEnemyState::FINISHING:
        // Finishing ���� �ൿ ó��...
        break;
    case EEnemyState::DEAD:
        // Dead ���� �ൿ ó��...
        break;
    }
}

