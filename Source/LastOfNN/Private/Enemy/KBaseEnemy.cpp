// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/KBaseEnemy.h"
#include "Enemy/KEnemyFSM.h"

// Sets default values
AKBaseEnemy::AKBaseEnemy()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//EnemyFSM ������Ʈ �߰�
	FSMComponent = CreateDefaultSubobject<UKEnemyFSM>(TEXT("FSM"));
}

// Called when the game starts or when spawned
void AKBaseEnemy::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AKBaseEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
//void AKBaseEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
//{
//	Super::SetupPlayerInputComponent(PlayerInputComponent);
//
//}

void AKBaseEnemy::EnemyIDLE()
{
	//�ð��� �帣��
	CurrentTime += GetWorld()->DeltaTimeSeconds;
	//����ð��� ���ð��� ������
	if (CurrentTime > IdleDelayTime)
	{
		//�̵����·� ��ȯ�Ѵ�.
		FSMComponent->CurrentState = EEnemyState::MOVE;
		//����ð� �ʱ�ȭ
		CurrentTime = 0;
	}
}

void AKBaseEnemy::EnemyMove()
{

}


void AKBaseEnemy::EnemyDamage()
{
	
}

void AKBaseEnemy::EnemyAttack()
{
	
}

void AKBaseEnemy::EnemyDead()
{
}

