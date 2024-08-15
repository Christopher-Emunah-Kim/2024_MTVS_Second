// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/InteractionUI.h"

void UInteractionUI::NativeConstruct()
{
	Super::NativeConstruct();

	PlayAnimation(ScaleFlash, 0, 0, EUMGSequencePlayMode::PingPong);
}
