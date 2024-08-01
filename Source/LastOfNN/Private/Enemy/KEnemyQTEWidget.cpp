// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/KEnemyQTEWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Animation/WidgetAnimation.h"
#include "Components/Image.h"
#include "Materials/MaterialInstanceDynamic.h"

void UKEnemyQTEWidget::NativeConstruct()
{
	Super::NativeConstruct();

	//Image 위젯 유효한지 확인
	if ( RadialProgressBorder )
	{
		//기존 MI 가져오기
		UMaterialInterface* tempMI = Cast<UMaterialInterface>(RadialProgressBorder->Brush.GetResourceObject());

		if ( tempMI )
		{
			//Matieral Dynamic 생성
			TempMateiralInstance = UMaterialInstanceDynamic::Create(tempMI, this);
			//생성한 MD를 브러시에 적용
			RadialProgressBorder->SetBrushFromMaterial(TempMateiralInstance);
		}
	}
}

//애니메이션 재생 함수

void UKEnemyQTEWidget::PlayScaleFlash()
{
	if ( ScaleFlash )
	{
		PlayAnimation(ScaleFlash, 0, 0); //1번 반복재생
		UE_LOG(LogTemp, Warning, TEXT("ScaleFlash"));
	}
}

	
void UKEnemyQTEWidget::PlayQTEPassed()
{
	if ( QTEPassed )
	{
		PlayAnimation(QTEPassed, 0, 1); //1번 반복재생
		UE_LOG(LogTemp, Warning, TEXT("QTEPassed"));
	}
}

	
void UKEnemyQTEWidget::PlayQTEFailed()
{
	if ( QTEFailed )
	{
		PlayAnimation(QTEFailed, 0, 1); //1번 반복재생
		UE_LOG(LogTemp, Warning, TEXT("QTEFailed"));
	}
}

void UKEnemyQTEWidget::UpdateMaterialProgress(float QTEProgress)
{
	TempMateiralInstance->SetScalarParameterValue(FName("Progress"), QTEProgress);

	//if ( RadialProgressBorder && RadialProgressBorder->Brush.GetResourceObject() )
	//{
	//	//기존의 MI를 가져옴
	//	
	//	UE_LOG(LogTemp, Warning, TEXT("Get Material !!"));
	//	//이미지를 설정한 브러시에서 Material Instance가져오기
	//	TempMateiralInstance = Cast<UMaterialInstanceDynamic>(RadialProgressBorder->Brush.GetResourceObject());
	//	if ( TempMateiralInstance )
	//	{
	//		// Material Instance가 유효하면, 파라미터 값 설정
	//		UE_LOG(LogTemp, Warning, TEXT("QTE Progress Success!!"));
	//		TempMateiralInstance->SetScalarParameterValue(FName("Progress"), QTEProgress);
	//	}
	//}
}
