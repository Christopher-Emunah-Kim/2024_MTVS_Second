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

void UJCharacterAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
	Super::NativeUpdateAnimation(DeltaTime);
	if (CharacterMovement )
	{
		GroundSpeed = UKismetMathLibrary::VSizeXY(CharacterMovement->Velocity);
		bIsFalling = CharacterMovement->IsFalling();
	}
}
