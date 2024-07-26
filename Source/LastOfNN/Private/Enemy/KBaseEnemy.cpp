// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/KBaseEnemy.h"
#include "Enemy/KEnemyFSM.h"
#include "Player/JPlayer.h"
#include <Kismet/GameplayStatics.h>
#include "Components/CapsuleComponent.h"
#include "Enemy/KEnemyAnim.h"
#include "Runtime/AIModule/Classes/AIController.h"
#include "NavigationSystem.h"  
#include "Runtime/AIModule/Classes/Navigation/PathFollowingComponent.h" 

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
	
	//�̵����� ������ ���� target ���� �ʱ�ȭ
	//���忡�� �÷��̾� ����ã��
	auto FirstPlayer = UGameplayStatics::GetActorOfClass(GetWorld(),AJPlayer::StaticClass());
	//target�� �ش� �÷��̾�Ÿ������ ĳ����
	target = Cast<AJPlayer>(FirstPlayer);
	
	//UKEnemyAnim �Ҵ�
	anim = Cast<UKEnemyAnim>(GetMesh()->GetAnimInstance());

	//AAIController �Ҵ�
	ai = Cast<AAIController>(GetController());
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

bool AKBaseEnemy::GetRandomPositionInNavMesh(FVector centerLocation, float radius, FVector& dest)
{
	//������̼� �ý��� �ν��Ͻ��� ���´�.
	auto ns = UNavigationSystemV1::GetNavigationSystem(GetWorld());
	//��ü ����Լ�ȣ���Ͽ� �Ű������� �� ����
	//centerLocation�� �������� radius���� �ȿ� �������� loc ������ ����ִ� ������ �Լ�
	//���������� ���� ȣ��Ǹ� true, �ƴϸ� false��ȯ.
	FNavLocation loc;
	bool result = ns->GetRandomReachablePointInRadius(centerLocation, radius, loc);
	//�� ���� �Լ��� ��ȯ ������ ��� �� ������ ��ġ�� dest������ �Ҵ�
	dest = loc.Location;
	
	return result;
}

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

		//�ִϸ��̼� ���� ����ȭ
		anim->EnemyAnimState = FSMComponent->CurrentState;
		//������ġ�� ���ʼ���
		GetRandomPositionInNavMesh(GetActorLocation(), 500, EnemyRandomPos);
	}
}

void AKBaseEnemy::EnemyMove()
{
	
}

void AKBaseEnemy::EnemyAttack()
{
	
}

float AKBaseEnemy::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float FinalDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	
	OnEnemyDamageProcess(FinalDamage);

	return FinalDamage;
}

void AKBaseEnemy::OnEnemyDamageProcess(float damage)
{
	//HP�� �����Ѵ�.(��������ŭ)
	EnemyHP -= damage;
	//���� ü���� ���Ҵٸ�
	if (EnemyHP > 0)
	{
		//�ǰݻ��� ��ȯ
		FSMComponent->CurrentState = EEnemyState::TAKEDAMAGE;
	}
	else
	{
		//�������� ��ȯ
		FSMComponent->CurrentState = EEnemyState::DEAD;
		//�浹ü��Ȱ��ȭ
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
	//�ִϸ��̼� ���� ����ȭ
	anim->EnemyAnimState = FSMComponent->CurrentState;
	//�̶� AI��ã�� ��� �������ѵα�
	ai->StopMovement();
}

void AKBaseEnemy::EnemyTakeDamage()
{
	
}

void AKBaseEnemy::EnemyDead()
{
}

