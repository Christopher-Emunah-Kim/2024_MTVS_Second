// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/KEnemyAnim.h"
#include "Enemy/KBossZombieEnemy.h"

void UKEnemyAnim::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	boss = Cast<AKBossZombieEnemy>(TryGetPawnOwner());
}


void UKEnemyAnim::AnimNotify_GrenadeShoot()
{
	bBossThrowGrenade = true;

	if ( boss )
	{
		boss->BossThrowGrenade();
	}
}

//각 좀비 공격완료상태 체크 Notify
void UKEnemyAnim::AnimNotify_AttackEnd()
{
	GEngine->AddOnScreenDebugMessage(9, 1, FColor::Red, TEXT("ATTACK COMPLETE"));
	//일반 공격 재생완료 체크
	bEnemyAttackPlay = false;

	//boss의 수류탄 공격 후 MOVE로 빠져나가기
	if ( boss )
	{
		boss->EnemySetState(EEnemyState::MOVE);
	}
}

void UKEnemyAnim::OnEndAnimation(FName sectionName, int32 anistate)
{
	
}
