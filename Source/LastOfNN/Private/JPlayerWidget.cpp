// Fill out your copyright notice in the Description page of Project Settings.


#include "JPlayerWidget.h"
#include "Components/ProgressBar.h"
#include "Components/VerticalBox.h"
#include "Components/TextBlock.h"
#include "Components/Widget.h"

void UJPlayerWidget::NativeConstruct()
{
	Super::NativeConstruct();

	ShotGunBox->SetVisibility(ESlateVisibility::Hidden);
	FireBottleBox->SetVisibility(ESlateVisibility::Hidden);
	PistolBox->SetVisibility(ESlateVisibility::Hidden);
	BatBox->SetVisibility(ESlateVisibility::Hidden);
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
}

void UJPlayerWidget::SetPistolEquipped()
{
	ShotGunBox->SetVisibility(ESlateVisibility::Hidden);
	FireBottleBox->SetVisibility(ESlateVisibility::Hidden);
	PistolBox->SetVisibility(ESlateVisibility::Visible);
	BatBox->SetVisibility(ESlateVisibility::Hidden);
}

void UJPlayerWidget::SetFireBottleEquipped()
{
	ShotGunBox->SetVisibility(ESlateVisibility::Hidden);
	FireBottleBox->SetVisibility(ESlateVisibility::Visible);
	PistolBox->SetVisibility(ESlateVisibility::Hidden);
	BatBox->SetVisibility(ESlateVisibility::Hidden);
}


