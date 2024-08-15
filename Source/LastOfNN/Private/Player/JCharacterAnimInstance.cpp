// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/JCharacterAnimInstance.h"
#include "Player/JPlayer.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/TimelineComponent.h"

UJCharacterAnimInstance::UJCharacterAnimInstance()
{

}

void UJCharacterAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	Player = Cast<AJPlayer>(TryGetPawnOwner());
	if ( Player )
	{
		CharacterMovement = Player->GetCharacterMovement();
	}

}

void UJCharacterAnimInstance::JumpToAttackMontageSection(int32 NewSection)
{
	UE_LOG(LogTemp, Error, TEXT("Jumptoattack"));
	if ( AnimCharacterEquipState == ECharacterEquipState::ECES_UnEquipped )
	{
		Montage_JumpToSection(GetAttackMontageSectionName(NewSection), AttackMontage);
	}
	else if(AnimCharacterEquipState ==ECharacterEquipState::ECES_BatEquipped)
	{
		Montage_JumpToSection(GetAttackMontageSectionName(NewSection), BatMontage);
		if ( GetAttackMontageSectionName(NewSection) == TEXT("Attack3") )
		{
			Player->CameraShake();
			//피 튀기는 거 살짝 보여주게
		}
	}
}

FName UJCharacterAnimInstance::GetAttackMontageSectionName(int32 Section)
{
	FName SectionName = FName(*FString::Printf(TEXT("Attack%d"), Section));
	UE_LOG(LogTemp, Log, TEXT("Section Name: %s"), *SectionName.ToString());
	return SectionName; 
}

void UJCharacterAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
	Super::NativeUpdateAnimation(DeltaTime);
	if ( Player == nullptr )
		return;
	if (CharacterMovement )
	{
		GroundSpeed = UKismetMathLibrary::VSizeXY(CharacterMovement->Velocity);
		bIsFalling = CharacterMovement->IsFalling();
		FTransform t = TryGetPawnOwner()->GetActorTransform();
		FRotator AngleRotator = UKismetMathLibrary::Conv_VectorToRotator(UKismetMathLibrary::InverseTransformDirection(t, TryGetPawnOwner()->GetVelocity()));
		Angle = AngleRotator.Yaw;
	}
	AnimCharacterState = Player->GetCharaterState();
	AnimCharacterEquipState = Player->GetCharacterEquipState();
}

void UJCharacterAnimInstance::PlayAttackMontage()
{
	if (AnimCharacterEquipState == ECharacterEquipState::ECES_UnEquipped)
	{
		Montage_Play(AttackMontage, 1.0f);
	}
	else if ( AnimCharacterEquipState == ECharacterEquipState::ECES_BatEquipped )
	{
		Montage_Play(BatMontage, 1.0f);
	}
}

void UJCharacterAnimInstance::PlayResistanceMontage()
{
	Montage_Play(ResistanceMontage);
}

void UJCharacterAnimInstance::StopResistanceMontage()
{
	Montage_Stop(0.2f, ResistanceMontage);
}

void UJCharacterAnimInstance::PlayResistanceReleaseSection()
{
	Montage_JumpToSection(TEXT("Release"), ResistanceMontage);
}

void UJCharacterAnimInstance::PlayExecuteMontage()
{
	Montage_Play(ExecuteMontage);
}

void UJCharacterAnimInstance::PlayGunShotMontage()
{
	Montage_Play(GunShotMontage);
}

void UJCharacterAnimInstance::PlayGunShotMontageSection(FName Section)
{
	Montage_JumpToSection(Section, GunShotMontage);
}

void UJCharacterAnimInstance::StopGunshotMontage()
{
	Montage_Stop(0.2f, GunShotMontage);;
}

void UJCharacterAnimInstance::PlayDieMontage()
{
	Montage_Play(DieMontage);
}

void UJCharacterAnimInstance::PlayHitMontage()
{
	Montage_Play(HitMontage);
}

void UJCharacterAnimInstance::PlayEquipAnimMontage(FName Section)
{
	Montage_Play(EquipMontage);
	Montage_JumpToSection(Section);
}

bool UJCharacterAnimInstance::GetChangingWeapon()
{
	return bChangingWeapon;
}

void UJCharacterAnimInstance::PlayShotgunMontage()
{
	Montage_Play(ShotgunMontage);
	Montage_JumpToSection(TEXT("Shoot"));
}

void UJCharacterAnimInstance::PlayFallingMontage()
{
	Montage_Play(FallingMontage);
}

void UJCharacterAnimInstance::AnimNotify_NextAttackCheck()
{
	OnNextAttackCheck.Broadcast();
}

void UJCharacterAnimInstance::AnimNotify_AttackHitCheck()
{
	OnAttackHitCheck.Broadcast();
}

void UJCharacterAnimInstance::AnimNotify_StopMove()
{
	bChangingWeapon = true;
}

void UJCharacterAnimInstance::AnimNotify_Move()
{
	bChangingWeapon = false;
}

void UJCharacterAnimInstance::AnimNotify_MoveCamera()
{
	bChangeSpringArmComp = true;
}

void UJCharacterAnimInstance::AnimNotify_CameraBack()
{
}

