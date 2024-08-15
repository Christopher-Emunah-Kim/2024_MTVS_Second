// Fill out your copyright notice in the Description page of Project Settings.


#include "JPlayerWidget.h"
#include "Components/ProgressBar.h"
#include "Components/VerticalBox.h"
#include "Components/TextBlock.h"
#include "Components/Widget.h"
#include "Player/JPlayerShotGun.h"
#include "Player/JPlayer.h"
#include "Player/PlayerGun.h"
#include "Kismet/GameplayStatics.h"

void UJPlayerWidget::NativeConstruct()
{
	Super::NativeConstruct();

	ShotGunBox->SetVisibility(ESlateVisibility::Hidden);
	FireBottleBox->SetVisibility(ESlateVisibility::Hidden);
	PistolBox->SetVisibility(ESlateVisibility::Hidden);
	BatBox->SetVisibility(ESlateVisibility::Hidden);

	Player = Cast<AJPlayer>(UGameplayStatics::GetPlayerCharacter(this, 0));
	ShotGunGun = Cast<AJPlayerShotGun>(Player->Shotgun); //Shotgun은 이미지
	PlayerPistol = Cast<APlayerGun>(Player->Gun);
}

void UJPlayerWidget::SetHpBar(float Hp, float MaxHp)
{
	HpBar->SetPercent(Hp / MaxHp);
}

void UJPlayerWidget::SetBatEquipped()
{
	ShotGunBox->SetVisibility(ESlateVisibility::Hidden);
	FireBottleBox->SetVisibility(ESlateVisibility::Hidden);
	PistolBox->SetVisibility(ESlateVisibility::Hidden);
	BatBox->SetVisibility(ESlateVisibility::Visible);
}

void UJPlayerWidget::SetShotGunEquipped()
{
	ShotGunBox->SetVisibility(ESlateVisibility::Visible);
	FireBottleBox->SetVisibility(ESlateVisibility::Hidden);
	PistolBox->SetVisibility(ESlateVisibility::Hidden);
	BatBox->SetVisibility(ESlateVisibility::Hidden);

	SetShotGunBulletNum();
}

void UJPlayerWidget::SetPistolEquipped()
{
	ShotGunBox->SetVisibility(ESlateVisibility::Hidden);
	FireBottleBox->SetVisibility(ESlateVisibility::Hidden);
	PistolBox->SetVisibility(ESlateVisibility::Visible);
	BatBox->SetVisibility(ESlateVisibility::Hidden);

	SetPistolBulletNum();
}

void UJPlayerWidget::SetFireBottleEquipped()
{
	ShotGunBox->SetVisibility(ESlateVisibility::Hidden);
	FireBottleBox->SetVisibility(ESlateVisibility::Visible);
	PistolBox->SetVisibility(ESlateVisibility::Hidden);
	BatBox->SetVisibility(ESlateVisibility::Hidden);
}

void UJPlayerWidget::SetPistolBulletNum()
{
	if ( PlayerPistol && PistolBullet )
	{
		PistolBullet->SetText(FText::AsNumber(PlayerPistol->CurrentBulletNum));
	}
}

void UJPlayerWidget::SetShotGunBulletNum()
{
	if ( ShotGunGun && ShotGunBullet )
	{
		ShotGunBullet->SetText(FText::AsNumber(ShotGunGun->CurrentBulletNum));
	}
}

void UJPlayerWidget::PlayerChange(AJPlayer* NewPlayer)
{
	Player = NewPlayer;
	ShotGunGun = Cast<AJPlayerShotGun>(Player->Shotgun);
	PlayerPistol = Cast<APlayerGun>(Player->Gun);
}


