// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/KEnemyAnim.h"

void UKEnemyAnim::OnEnemyEndAttackAnimation()
{
	bEnemyAttackPlay = false;
}

void UKEnemyAnim::OnBossThrowGrenadeAnimation()
{
	bBossThrowGrenade = true;



}

void UKEnemyAnim::OnEndAnimation(FName sectionName, int32 anistate)
{
	
}


