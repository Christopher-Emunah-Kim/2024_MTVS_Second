// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/InventoryWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Player/JPlayer.h"

void UInventoryWidget::NativeConstruct()
{
	Super::NativeConstruct();

	Player = Cast<AJPlayer>(GetWorld()->GetFirstPlayerController()->GetPawn());
}

void UInventoryWidget::PistolEquip()
{
	Player->SetStateEquipGun();
}

void UInventoryWidget::BatEquip()
{
	Player->SetStateBatEquipped();
}

void UInventoryWidget::FireEquip()
{
	Player->SetStateEquipThrowWeapon();
}
