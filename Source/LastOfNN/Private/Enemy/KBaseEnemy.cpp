// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/KBaseEnemy.h"

// Sets default values
AKBaseEnemy::AKBaseEnemy()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

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

}

void AKBaseEnemy::EnemyMove()
{

}

float AKBaseEnemy::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float FinalDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	/*if (DamageEvent.IsOfType(FPointDamageEvent::ClassID))
	{
		const FPointDamageEvent* PointDamageEvent = static_cast<const FPointDamageEvent*>(&DamageEvent);
		FHitResult result = PointDamageEvent->HitInfo;
		if (result.Component.IsValid())
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = nullptr;

			if (EnemyHPStat != nullptr && !IsDead())
			{
				if (result.Component->ComponentHasTag("Head"))
				{
					auto* DamageAmt = GetWorld()->SpawnActor<ADamageAmt>(CriticalDamageAmtFactory, result.Component->GetComponentLocation(), FRotator::ZeroRotator);
					FinalDamage *= 2;
					DamageAmt->SetDamageText(FinalDamage, true);
				}
				else
				{
					auto* DamageAmt = GetWorld()->SpawnActor<ADamageAmt>(DamageAmtFactory, GetActorLocation(), FRotator::ZeroRotator);
					DamageAmt->SetDamageText(FinalDamage, false);
				}

				HPBarWidget->SetVisibility(true);
				EnemyHPStat->SetDamage(FinalDamage);
			}
		}
		else
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = nullptr;
			auto* DamageAmt = GetWorld()->SpawnActor<ADamageAmt>(DamageAmtFactory, GetActorLocation(), FRotator::ZeroRotator);
			HPBarWidget->SetVisibility(true);
			EnemyHPStat->SetDamage(FinalDamage);
			DamageAmt->SetDamageText(FinalDamage, false);
		}
	}
	else
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = nullptr;
		auto* DamageAmt = GetWorld()->SpawnActor<ADamageAmt>(DamageAmtFactory, GetActorLocation(), FRotator::ZeroRotator);
		HPBarWidget->SetVisibility(true);
		EnemyHPStat->SetDamage(FinalDamage);
		DamageAmt->SetDamageText(FinalDamage, false);
	}*/

	return FinalDamage;
}

void AKBaseEnemy::EnemyAttack()
{
	
}

void AKBaseEnemy::EnemyDead()
{
}

