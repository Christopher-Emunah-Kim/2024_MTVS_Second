// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventoryWidget.generated.h"

class UButton;
class UTextBlock;
/**
 * 
 */
UCLASS()
class LASTOFNN_API UInventoryWidget : public UUserWidget
{
	GENERATED_BODY()
public:

	UPROPERTY(meta = (BindWidget))
	UButton* FireButton;	
	UPROPERTY(meta = (BindWidget))
	UButton* PistolButton;		
	UPROPERTY(meta = (BindWidget))
	UButton* BatButton;		
	UPROPERTY(meta = (BindWidget))
	UButton* ShotgunButton;		
	
	UPROPERTY(meta = (BindWidget))
	UTextBlock* FireBullet;	
	UPROPERTY(meta = (BindWidget))
	UTextBlock* ShotgunBullet;	
	UPROPERTY(meta = (BindWidget))
	UTextBlock* PistolBullet;	

	class AJPlayerShotGun* Shotgun;
	class APlayerGun* Pistol;

	virtual void NativeConstruct();

	UFUNCTION(BlueprintCallable)
	void PistolEquip();	
	UFUNCTION(BlueprintCallable)
	void BatEquip();	
	UFUNCTION(BlueprintCallable)
	void FireEquip();	
	UFUNCTION(BlueprintCallable)
	void ShotgunEquip();

	class AJPlayer* Player;

	UFUNCTION(BlueprintCallable)
	void SetBulletNums();
};
