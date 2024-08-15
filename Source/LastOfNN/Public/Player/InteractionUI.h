// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InteractionUI.generated.h"

/**
 * 
 */
UCLASS()
class LASTOFNN_API UInteractionUI : public UUserWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(meta = (BindWidget))
	class UImage* BorderBase;	
	UPROPERTY(meta = (BindWidget))
	class UImage* InnerSquare;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* KeyText;

	virtual void NativeConstruct() override;
	
	//애니메이션 변수 바인딩
	UPROPERTY(EditAnywhere, meta = (BindWidgetAnim), Transient)
	class UWidgetAnimation* ScaleFlash;

};
