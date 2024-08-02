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
#include "Enemy/KNormalZombieEnemy.h"
#include "Enemy/KEnemyAnim.h"
#include "Player/PlayerGun.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "Player/JCharacterAnimInstance.h"
#include "Perception/AISense_Hearing.h"
#include "Blueprint/UserWidget.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"
#include "Components/BoxComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Enemy/KEnemyQTEWidget.h"
#include "Kismet/GameplayStatics.h"



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

	Box = CreateDefaultSubobject<UBoxComponent>(TEXT("AssassinBox"));
	Box->SetupAttachment(RootComponent);

	// AI Perception Stimuli Source Component 생성 및 초기화
	PerceptionStimuliSource = CreateDefaultSubobject<UAIPerceptionStimuliSourceComponent>(TEXT("PerceptionStimuliSource"));
	PerceptionStimuliSource->RegisterForSense(TSubclassOf<UAISense_Hearing>());

	// 팀 타입 설정 (플레이어는 적)
	TeamType = ETeamType::ENEMY;

	//GrabEnemy초기화
	GrabbedEnemy = nullptr;

	//Crouch함수 사용을 위한 등록
	GetMovementComponent()->GetNavAgentPropertiesRef().bCanCrouch = true;

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
	PlayerController = Cast<APlayerController>(GetController());
}
float AJPlayer::GetKeyProcessPercent()
{
	return (float)CurrentKeyPresses / RequiredKeyPresses;
}
bool AJPlayer::GetIsGrabbed()
{
	return bIsGrabbed;
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
	//LockOnComp->SetTargetLockTrue();
	CharacterMovement = GetCharacterMovement();
	CharacterMovement->MaxWalkSpeed = 400;

	// 소리 발생 소스로 등록
	PerceptionStimuliSource->RegisterWithPerceptionSystem();
	AttackEndComboState();

	_QTEUI = CreateWidget<UKEnemyQTEWidget>(GetWorld(), QTEUIFactory);
	_QTEUI->AddToViewport();
	_QTEUI->SetPositionInViewport(FVector2D(700, 400));
	_QTEUI->SetVisibility(ESlateVisibility::Hidden);

	if ( Box )
	{
		Box->OnComponentBeginOverlap.AddDynamic(this, &AJPlayer::ReadyToExcecute);
	}

	Gun = GetWorld()->SpawnActor<APlayerGun>(GunClass);
	Gun->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, TEXT("GunSocket"));
	Gun->SetActorHiddenInGame(true);

	//FSM얻어오기
	EnemyFSM = UGameplayStatics::GetActorOfClass(this, AKNormalZombieEnemy::StaticClass())->GetComponentByClass<UKEnemyFSM>();
}

void AJPlayer::ReadyToExcecute(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UE_LOG(LogTemp, Warning, TEXT("DFDF"));
	ExecutionTarget = Cast<AKNormalZombieEnemy>(OtherComp->GetOwner());
	bCanExecute = true; //움직이는거 커버 못함.. move가 너무 빨라유
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
		EnhancedInputComponent->BindAction(IA_Crouch, ETriggerEvent::Completed, this, &AJPlayer::Crouching);
		EnhancedInputComponent->BindAction(IA_Grab, ETriggerEvent::Triggered, this, &AJPlayer::HandleQTEInput);
		EnhancedInputComponent->BindAction(IA_TakeDown, ETriggerEvent::Triggered, this, &AJPlayer::TakeDown);
		EnhancedInputComponent->BindAction(IA_EquipGun, ETriggerEvent::Triggered, this, &AJPlayer::SetStateEquipGun);
		EnhancedInputComponent->BindAction(IA_EquipThrowWeapon, ETriggerEvent::Triggered, this, &AJPlayer::SetStateEquipThrowWeapon);
		EnhancedInputComponent->BindAction(IA_UnEquipped, ETriggerEvent::Triggered, this, &AJPlayer::SetStateUnEquipped);
	}
}

void AJPlayer::Move(const FInputActionValue& Value)
{
	if ( !bIsGrabbed )                                                                   
	{
		const FVector2D Vector = Value.Get<FVector2D>();
		FRotator Rotation = GetController()->GetControlRotation(); //플레이어의 방향 읽어서 
		FRotator YawRotation(0, Rotation.Yaw, 0); //yaw사용

		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(ForwardDirection, Vector.X); //한글로 테스트 해봐요

		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(RightDirection, Vector.Y);
	}

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
		CharacterAnimInstance->PlayGunShotMontage();
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
	/*SpringArmComp->SetRelativeLocation(FVector(-72, 270, 80));*/
}
void AJPlayer::Run(const FInputActionValue& Value)
{
	//크라우칭중이 아니라면
	if ( CharaterState != ECharacterState::ECS_Crouching )
	{
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
}

void AJPlayer::Crouching(const FInputActionValue& Value)
{
	bCrouched = !bCrouched;
	if ( bCrouched )
	{
		CharaterState = ECharacterState::ECS_Crouching;
		Crouch();
	}
	else
	{
		CharaterState = ECharacterState::ECS_UnGrabbed;
		UnCrouch();
	}
}
void AJPlayer::TakeDown(const FInputActionValue& Value)
{
	CharaterState = ECharacterState::ECS_Crouching;
	CharacterAnimInstance->PlayResistanceMontage();
	if ( ExecutionTarget )
	{
		FTransform t = ExecutionTarget->GetAttackerTransform();


		FLatentActionInfo LatentInfo;
		LatentInfo.CallbackTarget = this;

		UKismetSystemLibrary::MoveComponentTo(
			GetCapsuleComponent(),              // 이동할 컴포넌트
			t.GetLocation(),                   // 목표 위치
			t.GetRotation().Rotator(),         // 목표 회전
			true,                              // 즉시 스냅
			false,
			0.2f,                            // 텔레포트하지 않음
			false,
			EMoveComponentAction::Type::Move,
			LatentInfo
		);
	}
	PlayerController = Cast<APlayerController>(GetController());
	if ( PlayerController )
	{
		PlayerController->SetIgnoreMoveInput(true);
		PlayerController->SetIgnoreLookInput(true);
		UE_LOG(LogTemp, Warning, TEXT("dd"));
	}
	CharacterAnimInstance->PlayExecuteMontage();
	//몽타주 끝나면 상태 바꾸기
	GetWorldTimerManager().SetTimer(TakeDownTimer, this, &AJPlayer::AfterTakeDown, 6.f, false);
	CameraComp->SetFieldOfView(60.f);
/////////////////////////////////////////////////////////////
	//좀비 상태 바꿔야함, OR 좀비 당하는 몽타주 나오면 상관없나?

}
void AJPlayer::AfterTakeDown()
{
	PlayerController = Cast<APlayerController>(GetController());
	CharaterState = ECharacterState::ECS_UnGrabbed;
	if ( PlayerController )
	{
		PlayerController->SetIgnoreMoveInput(false);
		PlayerController->SetIgnoreLookInput(false);
	}

}
void AJPlayer::SetStateEquipGun()
{
	CharacterEquipState = ECharacterEquipState::ECES_GunEquipped;
	Gun->SetActorHiddenInGame(false);
}
void AJPlayer::SetStateEquipThrowWeapon()
{
	CharacterEquipState = ECharacterEquipState::ECES_ThrowWeaponEquipped;
	Gun->SetActorHiddenInGame(true);
}
void AJPlayer::SetStateUnEquipped()
{
	CharacterEquipState = ECharacterEquipState::ECES_UnEquipped;
	Gun->SetActorHiddenInGame(true);
}
UCameraComponent* AJPlayer::GetCamera()
{
	return CameraComp;
}
ECharacterState AJPlayer::GetCharaterState() const
{
	return CharaterState;
}
ECharacterEquipState AJPlayer::GetCharacterEquipState() const
{
	return CharacterEquipState;
}

#pragma region Grab QTE Event Function

void AJPlayer::StartGrabbedState(AKNormalZombieEnemy* Enemy)
{
	bIsGrabbed = true;
	CurrentKeyPresses = 0;
	GrabbedEnemy = Enemy;

	// 저항 애니메이션 재생 (블루프린트에서 설정된 ResistanceMontage)
	if ( CharacterAnimInstance )
	{
		CharacterAnimInstance->PlayResistanceMontage();
	}

	//// E키 이외의 모든 입력을 차단
	//APlayerController* PlayerController = Cast<APlayerController>(GetController());
	//if ( PlayerController )
	//{
	//	PlayerController->SetIgnoreMoveInput(true);
	//	PlayerController->SetIgnoreLookInput(true);
	//}

	// QTE UI 표시
	StartQTEGrabEvent();
}

void AJPlayer::StopGrabbedState(bool bSuccess)
{
	bIsGrabbed = false;

	// 저항 애니메이션 정지
	if ( CharacterAnimInstance )
	{
		CharacterAnimInstance->PlayResistanceReleaseSection();
	}

	// QTE UI 제거 및 성공/실패 애니메이션 재생
	StopQTEGrabEvent(bSuccess);

	// QTE 실패 시 추가 로직 (예: Player가 피해를 입음)
	if ( !bSuccess )
	{
		// 실패 시 처리할 로직을 여기에 추가 (예: Player의 HP 감소)
		//필요하면 하셈
	}

	// Enemy의 상태를 IDLE로 전환하고, QTE가 끝났음을 알림
	if ( GrabbedEnemy )
	{
		GrabbedEnemy->FSMComponent->SetState(EEnemyState::IDLE);
		GrabbedEnemy->bIsPlayerGrabbed = false;
		if ( GrabbedEnemy->anim )
		{
			FString SectionName = FString::Printf(TEXT("Release"));
			GrabbedEnemy->anim->PlayEnemyGrabAnim(FName(*SectionName));
		}
	}

	// QTE 이벤트가 끝났음을 전역 변수에 표시
	 EnemyFSM->bIsQTEActive = false;
}

void AJPlayer::HandleQTEInput()
{
	if ( bIsGrabbed )
	{
		CurrentKeyPresses++;
		_QTEUI->PlayQTEPassed();
		_QTEUI->UpdateMaterialProgress(GetKeyProcessPercent());
		if ( CurrentKeyPresses >= RequiredKeyPresses )
		{
			// QTE 성공, Grab 상태 해제
			StopGrabbedState(true);
			bEscapeSuccess = true;
			GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Green, TEXT("Escaped from Grab!"));
		}
	}
}

void AJPlayer::StartQTEGrabEvent()
{
	if ( _QTEUI )
	{
		_QTEUI->SetVisibility(ESlateVisibility::Visible);

		// ScaleFlash 애니메이션 재생
		_QTEUI->PlayScaleFlash();
	}
}

void AJPlayer::StopQTEGrabEvent(bool bSuccess)
{
	if ( _QTEUI )
	{
		// QTE 성공 시 Passed 애니메이션, 실패 시 Failed 애니메이션 재생
		UWidgetAnimation* ResultAnim = nullptr;

		if ( bSuccess )
		{
			
		}
		else
		{
			_QTEUI->PlayQTEFailed();
		}
		// 일정 시간 후 UI 제거
		_QTEUI->SetVisibility(ESlateVisibility::Hidden);
	}
}

#pragma endregion