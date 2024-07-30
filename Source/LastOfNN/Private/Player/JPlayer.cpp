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
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "Player/JCharacterAnimInstance.h"
#include "Perception/AISense_Hearing.h"

ETeamType AJPlayer::GetTeamType() const
{
	return TeamType;
}

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

	// AI Perception Stimuli Source Component 생성 및 초기화
	PerceptionStimuliSource = CreateDefaultSubobject<UAIPerceptionStimuliSourceComponent>(TEXT("PerceptionStimuliSource"));
	PerceptionStimuliSource->RegisterForSense(TSubclassOf<UAISense_Hearing>());

	// 팀 타입 설정 (플레이어는 적)
	TeamType = ETeamType::ENEMY;


}
void AJPlayer::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	CharacterAnimInstance = Cast<UJCharacterAnimInstance>(GetMesh()->GetAnimInstance());

	CharacterAnimInstance->OnMontageEnded.AddDynamic(this, &AJPlayer::OnAttackMontageEnded);

	CharacterAnimInstance->OnNextAttackCheck.AddLambda([this]() -> void
	{
		bCanNextCombo = false;

		if ( bIsComboInputOn )
		{
			AttackStartComboState();
			UE_LOG(LogTemp, Error, TEXT("Lamda Currentcombo %f"), CurrentCombo);
			CharacterAnimInstance->JumpToAttackMontageSection(CurrentCombo);
		}
	});
	CharacterAnimInstance->OnAttackHitCheck.AddLambda([this]() -> void
	{

	});

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
	Gun->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, TEXT("GunSocket"));

	CharacterMovement = GetCharacterMovement();
	CharacterMovement->MaxWalkSpeed = 400;

	// 소리 발생 소스로 등록
	PerceptionStimuliSource->RegisterWithPerceptionSystem();
	AttackEndComboState();
}

void AJPlayer::OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	UE_LOG(LogTemp, Error, TEXT(" Montage Ended"));
	if ( CurrentCombo >= MaxCombo )
	{
		bIsAttacking = false;
		AttackEndComboState();

	}
	else if ( bCanNextCombo == false || bIsComboInputOn == false )
	{
		bIsAttacking = false;
		AttackEndComboState();
	}
}

void AJPlayer::AttackStartComboState()
{
	bCanNextCombo = true;
	bIsComboInputOn = false;
	CurrentCombo = FMath::Clamp<int32>(CurrentCombo + 1, 1, MaxCombo);
	UE_LOG(LogTemp, Error, TEXT(" Currentcombo = %f"), CurrentCombo);
}

void AJPlayer::AttackEndComboState()
{
	bIsComboInputOn = false;
	bCanNextCombo = false;
	CurrentCombo = 0;
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
	if ( CharacterEquipState == ECharacterEquipState::ECES_GunEquipped )
	{
		Gun->PullTrigger();
	}
	else if ( CharacterEquipState == ECharacterEquipState::ECES_UnEquipped )
	{
		if ( bIsAttacking )
		{

			if ( bCanNextCombo )
			{
				bIsComboInputOn = true;
			}
		}
		else
		{
			AttackStartComboState();
			CharacterAnimInstance->JumpToAttackMontageSection(CurrentCombo);
			CharacterAnimInstance->Montage_Play(AttackMontage);
			bIsAttacking = true;
		}

	}
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
