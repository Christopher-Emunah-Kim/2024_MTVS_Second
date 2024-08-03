// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/JCharacterAnimInstance.h"
#include "Player/JPlayer.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

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


void UJCharacterAnimInstance::AnimNotify_NextAttackCheck()
{
	OnNextAttackCheck.Broadcast();
}

void UJCharacterAnimInstance::AnimNotify_AttackHitCheck()
{
	OnAttackHitCheck.Broadcast();
}
