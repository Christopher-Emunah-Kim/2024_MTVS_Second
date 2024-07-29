// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/JPlayer.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/PlayerController.h"
#include "../../../../Plugins/EnhancedInput/Source/EnhancedInput/Public/EnhancedInputSubsystems.h"
#include "../../../../Plugins/EnhancedInput/Source/EnhancedInput/Public/EnhancedInputComponent.h"
#include "Player/PlayerLockOn.h"
#include "Enemy/KEnemyFSM.h"
#include "Enemy/KBaseEnemy.h"
#include "Player/PlayerGun.h"
#include "GameFramework/CharacterMovementComponent.h"

enum class ECharacterState : uint8
{
	ECS_Grabbed UMETA(DisplayName = "Grabbed"),
	ECS_Escape UMETA(DisplayName = "Escape"),
	ECS_NoGrabbed UMETA(DisplayName = "NoGrabbed")
};

enum class ECharacterEquipState : uint8
{
	ECES_UnEquipped UMETA(DisplayName = "UnEquipped"),
	ECES_GunEquipped UMETA(DisplayName = "GunEquipped"),
	ECES_BatEquipped UMETA(DisplayName = "BatEquipped"),
	ECES_ThrowWeaponEquipped UMETA(DisplayName = "ThrowWeaponEquipped"),
};


// Sets default values
AJPlayer::AJPlayer()
{

	PrimaryActorTick.bCanEverTick = true;
	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SprintArmComp"));
	SpringArmComp->SetupAttachment(RootComponent);
	SpringArmComp->SetRelativeLocation(FVector(0, 40, 80));
	SpringArmComp->TargetArmLength = 200;

	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
	CameraComp->SetupAttachment(SpringArmComp);

	LockOnComp = CreateDefaultSubobject<UPlayerLockOn>(TEXT("LockOnComp"));
	LockOnComp->SetupAttachment(RootComponent);

}
// Called when the game starts or when spawned
void AJPlayer::BeginPlay()
{
	Super::BeginPlay();
	APlayerController* PC = Cast<APlayerController>(GetController());
	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer());
	if (Subsystem)
	{
		Subsystem->AddMappingContext(IMC_Joel, 0);
	}
	LockOnComp->SetTargetLockTrue();

	Gun = GetWorld()->SpawnActor<APlayerGun>(GunClass);
	Gun->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, TEXT("mixamorig_RightHandRing2"));

	CharacterMovement = GetCharacterMovement();
	CharacterMovement->MaxWalkSpeed = 400;
}

// Called every frame
void AJPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void AJPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked< UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(IA_Move, ETriggerEvent::Triggered, this, &AJPlayer::Move);
		EnhancedInputComponent->BindAction(IA_Look, ETriggerEvent::Triggered, this, &AJPlayer::Look);
		EnhancedInputComponent->BindAction(IA_Fire, ETriggerEvent::Triggered, this, &AJPlayer::Fire);
		EnhancedInputComponent->BindAction(IA_Zoom, ETriggerEvent::Triggered, this, &AJPlayer::Zoom);
		EnhancedInputComponent->BindAction(IA_Run, ETriggerEvent::Started, this, &AJPlayer::Run);
		EnhancedInputComponent->BindAction(IA_Run, ETriggerEvent::Completed, this, &AJPlayer::Run);
	}
}

void AJPlayer::Move(const FInputActionValue& Value)
{
	const FVector2D Vector = Value.Get<FVector2D>();
	FRotator Rotation = GetController()->GetControlRotation(); //플레이어의 방향 읽어서 
	FRotator YawRotation(0, Rotation.Yaw, 0); //yaw사용

	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	AddMovementInput(ForwardDirection, Vector.X); //한글로 테스트 해봐요

	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
	AddMovementInput(RightDirection, Vector.Y);


}

void AJPlayer::Look(const FInputActionValue& Value)
{
	FVector2D LV = Value.Get<FVector2D>();
	AddControllerPitchInput(-LV.Y);
	AddControllerYawInput(LV.X);
}
void AJPlayer::Fire(const FInputActionValue& Value)
{
	Gun->PullTrigger();
}
void AJPlayer::Zoom(const FInputActionValue& Value)
{
	SpringArmComp->SetRelativeLocation(FVector(-72, 270, 80));
}
void AJPlayer::Run(const FInputActionValue& Value)
{
	UE_LOG(LogTemp, Error, TEXT("DFDF"));
	//안 달리는 중이면
	if ( !bIsRunning )
	{
		CharacterMovement->MaxWalkSpeed = 600;
	}
	//달리는 중이면
	else
	{
		CharacterMovement->MaxWalkSpeed = 400;
	}
	bIsRunning = !bIsRunning;
}
UCameraComponent* AJPlayer::GetCamera()
{
	return CameraComp;
}
