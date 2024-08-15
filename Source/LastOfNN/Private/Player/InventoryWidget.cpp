// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/InventoryWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Player/JPlayer.h"
#include "Player/JPlayerShotGun.h"
#include "Player/PlayerGun.h"

void UInventoryWidget::NativeConstruct()
{
	Super::NativeConstruct();

	Player = Cast<AJPlayer>(GetWorld()->GetFirstPlayerController()->GetPawn());
	Shotgun = Cast<AJPlayerShotGun>(Player->Shotgun);
	Pistol = Cast<APlayerGun>(Player->Gun); 

	// 초기화 후 바로 탄약 수 업데이트
	SetBulletNums();
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

void UInventoryWidget::ShotgunEquip()
{
	Player->SetStateShotgunEquipped();
}

void UInventoryWidget::SetBulletNums()
{
	if ( Shotgun && ShotgunBullet )
	{
		ShotgunBullet->SetText(FText::AsNumber(Shotgun->CurrentBulletNum));
	}

	if ( Pistol && PistolBullet )
	{
		PistolBullet->SetText(FText::AsNumber(Pistol->CurrentBulletNum));
	}
}
