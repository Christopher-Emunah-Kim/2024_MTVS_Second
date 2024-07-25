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

enum class ECharacterState : uint8
{
	ECS_Grabbed UMETA(DisplayName = "Grabbed"),
	ECS_Escape UMETA(DisplayName = "Escape"),
	ECS_NoGrabbed UMETA(DisplayName = "NoGrabbed")
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
	Gun->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, TEXT("hand_r"));
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
UCameraComponent* AJPlayer::GetCamera()
{
	return CameraComp;
}
