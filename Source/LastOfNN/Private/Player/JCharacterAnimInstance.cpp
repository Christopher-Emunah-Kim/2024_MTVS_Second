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
	Montage_JumpToSection(GetAttackMontageSectionName(NewSection), AttackMontage);
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
	Montage_Play(AttackMontage, 1.0f);
}

void UJCharacterAnimInstance::AnimNotify_NextAttackCheck()
{
	OnNextAttackCheck.Broadcast();
}

void UJCharacterAnimInstance::AnimNotify_AttackHitCheck()
{
	OnAttackHitCheck.Broadcast();
}
