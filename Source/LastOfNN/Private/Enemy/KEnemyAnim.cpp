// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/KEnemyAnim.h"
#include "Enemy/KBossZombieEnemy.h"

void UKEnemyAnim::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	boss = Cast<AKBossZombieEnemy>(TryGetPawnOwner());
}

void UKEnemyAnim::OnEnemyEndAttackAnimation()
{
	bEnemyAttackPlay = false;
}

//void UKEnemyAnim::OnBossThrowGrenadeAnimation()
//{
//	
//}

void UKEnemyAnim::AnimNotify_GrenadeShoot()
{
	bBossThrowGrenade = true;

	if ( boss )
	{
		boss->BossThrowGrenade();
	}
}

void UKEnemyAnim::OnEndAnimation(FName sectionName, int32 anistate)
{
	
}
