// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/KBaseEnemy.h"
#include "Enemy/KEnemyFSM.h"
#include "Player/JPlayer.h"
#include <Kismet/GameplayStatics.h>

// Sets default values
AKBaseEnemy::AKBaseEnemy()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//EnemyFSM 컴포넌트 추가
	FSMComponent = CreateDefaultSubobject<UKEnemyFSM>(TEXT("FSM"));
}

// Called when the game starts or when spawned
void AKBaseEnemy::BeginPlay()
{
	Super::BeginPlay();
	
	//이동상태 구현을 위한 target 변수 초기화
	//월드에서 플레이어 액터찾기
	auto FirstPlayer = UGameplayStatics::GetActorOfClass(GetWorld(),AJPlayer::StaticClass());
	//target을 해당 플레이어타입으로 캐스팅
	target = Cast<AJPlayer>(FirstPlayer);
	
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
	//시간이 흐르면
	CurrentTime += GetWorld()->DeltaTimeSeconds;
	//경과시간이 대기시간을 지나면
	if (CurrentTime > IdleDelayTime)
	{
		//이동상태로 전환한다.
		FSMComponent->CurrentState = EEnemyState::MOVE;
		//경과시간 초기화
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

