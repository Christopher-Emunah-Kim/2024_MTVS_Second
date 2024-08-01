// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "KEnemyQTEWidget.generated.h"

/**
 * 
 */
UCLASS()
class LASTOFNN_API UKEnemyQTEWidget : public UUserWidget
{
	GENERATED_BODY()
	

public:

	virtual void NativeConstruct() override;

	//애니메이션 변수 바인딩
	UPROPERTY(EditAnywhere, meta = (BindWidgetAnim), Transient)
	class UWidgetAnimation* ScaleFlash;

	UPROPERTY(EditAnywhere, meta = (BindWidgetAnim), Transient)
	class UWidgetAnimation* QTEPassed;

	UPROPERTY(EditAnywhere, meta = (BindWidgetAnim), Transient)
	class UWidgetAnimation* QTEFailed;

	//애니메이션 재생 함수
	UFUNCTION(BlueprintCallable, Category = "UI")
	void PlayScaleFlash();

	UFUNCTION(BlueprintCallable, Category = "UI")
	void PlayQTEPassed();

	UFUNCTION(BlueprintCallable, Category = "UI")
	void PlayQTEFailed();

	//UI에서 사용한 이미지 위젯 인스턴스 선언
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UImage* RadialProgressBorder;

	//ProgressSquare 값 업데이트 함수 선언
	UFUNCTION(BlueprintCallable, Category = "UI")
	void UpdateMaterialProgress(float QTEProgress);



};
