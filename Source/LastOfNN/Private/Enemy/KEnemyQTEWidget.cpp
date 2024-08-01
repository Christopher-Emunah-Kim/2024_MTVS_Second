// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/KEnemyQTEWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Animation/WidgetAnimation.h"
#include "Components/Image.h"
#include "Materials/MaterialInstanceDynamic.h"

void UKEnemyQTEWidget::NativeConstruct()
{
	Super::NativeConstruct();

}

//애니메이션 재생 함수

void UKEnemyQTEWidget::PlayScaleFlash()
{
	if ( ScaleFlash )
	{
		PlayAnimation(ScaleFlash, 0, 1); //1번 반복재생
	}
}

	
void UKEnemyQTEWidget::PlayQTEPassed()
{
	if ( QTEPassed )
	{
		PlayAnimation(QTEPassed, 0, 1); //1번 반복재생
	}
}

	
void UKEnemyQTEWidget::PlayQTEFailed()
{
	if ( QTEFailed )
	{
		PlayAnimation(QTEFailed, 0, 1); //1번 반복재생
	}
}

void UKEnemyQTEWidget::UpdateMaterialProgress(float QTEProgress)
{
	if ( RadialProgressBorder && RadialProgressBorder->Brush.GetResourceObject() )
	{
		//이미지를 설정한 브러시에서 Material Instance가져오기
		UMaterialInstanceDynamic* TempMateiralInstance = Cast<UMaterialInstanceDynamic>(RadialProgressBorder->Brush.GetResourceObject());
		if ( TempMateiralInstance )
		{
			// Material Instance가 유효하면, 파라미터 값 설정
			TempMateiralInstance->SetScalarParameterValue(FName("Progress"), QTEProgress);
		}
	}
}
