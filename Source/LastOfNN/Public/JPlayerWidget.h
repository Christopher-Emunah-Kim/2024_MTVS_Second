// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "JPlayerWidget.generated.h"

/**
 * 
 */
UCLASS()
class LASTOFNN_API UJPlayerWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	//UI에서 사용한 이미지 위젯 인스턴스 선언
	UPROPERTY(meta = (BindWidget))
	class UImage* Shotgun;	
	UPROPERTY(meta = (BindWidget))
	class UImage* Pistol;	
	UPROPERTY(meta = (BindWidget))
	class UImage* FireBottle;	
	UPROPERTY(meta = (BindWidget))
	class UImage* Bat;	
	
	UPROPERTY(meta = (BindWidget))
	class UVerticalBox* ShotGunBox;	
	UPROPERTY(meta = (BindWidget))
	class UVerticalBox* PistolBox;	
	UPROPERTY(meta = (BindWidget))
	class UVerticalBox* FireBottleBox;
	UPROPERTY(meta = (BindWidget))
	class UVerticalBox* BatBox;	
	
	UPROPERTY(meta = (BindWidget))
	class UProgressBar* HpBar;	
	
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* ShotGunBullet;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* PistolBullet;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* FireBottleNum;

	virtual void NativeConstruct() override;

	UFUNCTION(BlueprintCallable)
	void SetHpBar(float Hp, float MaxHp);

	UFUNCTION(BlueprintCallable)
	void SetBatEquipped();	
	UFUNCTION(BlueprintCallable)
	void SetShotGunEquipped();	
	UFUNCTION(BlueprintCallable)
	void SetPistolEquipped();	
	UFUNCTION(BlueprintCallable)
	void SetFireBottleEquipped();



};
