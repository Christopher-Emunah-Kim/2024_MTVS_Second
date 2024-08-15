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
#include "Enemy/KBossZombieEnemy.h"
#include "Kismet/KismetMathLibrary.h"
#include "Enemy/KEnemyAnim.h"
#include "Runtime/AIModule/Classes/AIController.h"
#include "Components/SphereComponent.h"
#include "Engine/DamageEvents.h"
#include "Player/JPlayerBat.h"
#include "Player/JGunWidget.h"
#include "Camera/PlayerCameraManager.h"
#include "Components/SceneComponent.h"
#include "Camera/CameraActor.h"
#include "Player/InventoryWidget.h"
#include "Player/JPlayerShotGun.h"
#include "Enemy/KBeginnerZombieEnemy.h"
#include <Perception/AISense_Sight.h>
#include "JPlayerWidget.h"
#include "Player/InteractionUI.h"


void AJPlayer::SetStateReversed()
{
	CharaterState = ECharacterState::ECS_Reversed;

}

void AJPlayer::MakeNewPlayerUI()
{
	if ( PlayerUI )
	{
		PlayerUI->RemoveFromParent();
	}
	PlayerUI = CreateWidget<UJPlayerWidget>(GetWorld(), PlayerUIFactory);
	PlayerUI->AddToViewport();
	if ( Inventory )
	{
		Inventory->RemoveFromParent();
	}
	Inventory = CreateWidget<UInventoryWidget>(GetWorld(), InventoryUIFactory);
	Inventory->AddToViewport();
	Inventory->SetVisibility(ESlateVisibility::Hidden);

	GunWidget = CreateWidget<UJGunWidget>(GetWorld(), GunUIFactory);
	GunWidget->AddToViewport();
	GunWidget->SetVisibility(ESlateVisibility::Hidden);

	InteractionUI = CreateWidget<UInteractionUI>(GetWorld(), InteractionUIFactory);
	InteractionUI->AddToViewport();
	InteractionUI->SetVisibility(ESlateVisibility::Hidden);

	PlayerUI->SetHpBar(HealthPoints, MAXHP);

	PlayerController = Cast<APlayerController>(GetController());
}

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

	CameraPostion = CreateDefaultSubobject<USceneComponent>(TEXT("CameraPostion"));
	CameraPostion->SetupAttachment(RootComponent);
	CameraPostion->SetRelativeLocation(FVector(-81.5f, -36.f, 40.f));
	CameraPostion->SetRelativeRotation(FRotator(14.5f, 11.f, -6.5f));

	LockOnComp = CreateDefaultSubobject<UPlayerLockOn>(TEXT("LockOnComp"));
	LockOnComp->SetupAttachment(RootComponent);

	Box = CreateDefaultSubobject<UBoxComponent>(TEXT("AssassinBox"));
	Box->SetupAttachment(RootComponent);

	GetMesh()->SetRelativeRotation(FRotator(0, -90, 0));

	// AI Perception Stimuli Source Component 생성 및 초기화
	PerceptionStimuliSource = CreateDefaultSubobject<UAIPerceptionStimuliSourceComponent>(TEXT("PerceptionStimuliSource"));
	PerceptionStimuliSource->RegisterWithPerceptionSystem();
	PerceptionStimuliSource->RegisterForSense(TSubclassOf<UAISense_Hearing>());
	PerceptionStimuliSource->RegisterForSense(TSubclassOf<UAISense_Sight>());


	// 팀 타입 설정 (플레이어는 적)
	TeamType = ETeamType::ENEMY;
	//TeamID = FGenericTeamId(0);

	//GrabEnemy초기화
	GrabbedEnemy = nullptr;

	//Crouch함수 사용을 위한 등록
	GetMovementComponent()->GetNavAgentPropertiesRef().bCanCrouch = true;

	RightAttackSphere = CreateDefaultSubobject<USphereComponent>(TEXT("RightAttackSphere"));
	RightAttackSphere->SetupAttachment(GetMesh(), TEXT("mixamorig_RightHand"));
	RightAttackSphere->SetSphereRadius(10.f);

	LeftAttackSphere = CreateDefaultSubobject<USphereComponent>(TEXT("LeftAttackSphere"));
	LeftAttackSphere->SetupAttachment(GetMesh(), TEXT("mixamorig_LeftHand"));
	LeftAttackSphere->SetSphereRadius(10.f);

	//체력초기화
	HealthPoints = MAXHP;

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

void AJPlayer::StopForAttack()
{
	PlayerController->SetIgnoreMoveInput(false);

	GetWorld()->GetTimerManager().ClearTimer(TakeDownTimer);
}


void AJPlayer::CameraShake()
{
	APlayerCameraManager* CameraManager = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);
	if ( CameraManager )
	{
		// 클라이언트에서 카메라 흔들림 효과 시작
		if ( PlayerController )
		{
			PlayerController->ClientStartCameraShake(CamShake);
		}

		// 월드 공간에서 카메라 흔들림 효과 적용
		//CameraManager->PlayWorldCameraShake(GetWorld(), CamShake, GetActorLocation(), 0.0f, 5.0f, 1.0f);
	}
}

//FGenericTeamId AJPlayer::GetGenericTeamId() const
//{
//	return TeamID;
//}


void AJPlayer::TransferWeaponState(AJPlayer* FromPlayer, AJPlayer* ToPlayer)
{
	if ( FromPlayer && ToPlayer )
	{
		// 플레이어1의 샷건과 피스톨 상태를 가져옵니다.
		AJPlayerShotGun* FromShotgun = Cast<AJPlayerShotGun>(FromPlayer->Shotgun);
		AJPlayerShotGun* ToShotgun = Cast<AJPlayerShotGun>(ToPlayer->Shotgun);
		ToPlayer->HealthPoints =  FromPlayer->HealthPoints;
		if ( FromShotgun && ToShotgun )
		{
			int32 OldBulletNum = ToShotgun->CurrentBulletNum;
			int32 NewBulletNum = FromShotgun->CurrentBulletNum;

			ToShotgun->CurrentBulletNum = NewBulletNum;

			UE_LOG(LogTemp, Error, TEXT("Shotgun BulletNum before: %d, after: %d"), OldBulletNum, NewBulletNum);
		}

		// 피스톨 상태 복사
		APlayerGun* FromPistol = Cast<APlayerGun>(FromPlayer->Gun);
		APlayerGun* ToPistol = Cast<APlayerGun>(ToPlayer->Gun);

		if ( FromPistol && ToPistol )
		{
			int32 OldBulletNum = ToPistol->CurrentBulletNum;
			int32 NewBulletNum = FromPistol->CurrentBulletNum;

			ToPistol->CurrentBulletNum = NewBulletNum;

			UE_LOG(LogTemp, Error, TEXT("Pistol BulletNum before: %d, after: %d"), OldBulletNum, NewBulletNum);
		}
	}
}

void AJPlayer::BeginPlay()
{
	Super::BeginPlay();
	APlayerController* PC = Cast<APlayerController>(GetController());
	if ( PC )
	{
		Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer());
		if ( Subsystem )
		{
			Subsystem->AddMappingContext(IMC_Joel, 0);
		}
	}
	//LockOnComp->SetTargetLockTrue();
	CharacterMovement = GetCharacterMovement();
	CharacterMovement->MaxWalkSpeed = 400;

	// 소리 발생 소스로 등록
	PerceptionStimuliSource->RegisterWithPerceptionSystem();
	AttackEndComboState();

	Gun = GetWorld()->SpawnActor<APlayerGun>(GunClass);
	Gun->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, TEXT("GunSocket"));
	Gun->SetActorHiddenInGame(true);

	Bat = GetWorld()->SpawnActor<AJPlayerBat>(BatClass);
	Bat->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, TEXT("mixamorig_RightHand"));
	Bat->SetActorHiddenInGame(true);
	Bat->SetActorEnableCollision(false);
	Bat->SetOwner(this);

	Shotgun = GetWorld()->SpawnActor<AJPlayerShotGun>(ShotGunClass);
	Shotgun->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, TEXT("ShotgunSocket"));
	Shotgun->SetActorHiddenInGame(true);
	Shotgun->SetActorEnableCollision(false);

	_QTEUI = CreateWidget<UKEnemyQTEWidget>(GetWorld(), QTEUIFactory);
	_QTEUI->AddToViewport();
	_QTEUI->SetPositionInViewport(FVector2D(700, 400));
	_QTEUI->SetVisibility(ESlateVisibility::Hidden);

	GunWidget = CreateWidget<UJGunWidget>(GetWorld(), GunUIFactory);
	GunWidget->AddToViewport();
	GunWidget->SetVisibility(ESlateVisibility::Hidden);

	Inventory = CreateWidget<UInventoryWidget>(GetWorld(), InventoryUIFactory);
	Inventory->AddToViewport();
	Inventory->SetVisibility(ESlateVisibility::Hidden);

	PlayerUI = CreateWidget<UJPlayerWidget>(GetWorld(), PlayerUIFactory);
	PlayerUI->AddToViewport();

	InteractionUI = CreateWidget<UInteractionUI>(GetWorld(), InteractionUIFactory);
	InteractionUI->AddToViewport();
	InteractionUI->SetVisibility(ESlateVisibility::Hidden);

	if ( Box )
	{
		Box->OnComponentBeginOverlap.AddDynamic(this, &AJPlayer::ReadyToExcecute);
	}
	if ( RightAttackSphere )
	{
		RightAttackSphere->OnComponentBeginOverlap.AddDynamic(this, &AJPlayer::OverlapDamage);
	}	
	if ( LeftAttackSphere )
	{
		LeftAttackSphere->OnComponentBeginOverlap.AddDynamic(this, &AJPlayer::OverlapDamage);
	}

	

	//FSM얻어오기
	AActor* EnemyActor = UGameplayStatics::GetActorOfClass(this, AKNormalZombieEnemy::StaticClass());
	if ( EnemyActor )
	{
		// 액터가 존재하는 경우, 해당 액터에서 컴포넌트를 가져옵니다
		EnemyFSM = EnemyActor->GetComponentByClass<UKEnemyFSM>();
		if ( EnemyFSM  == nullptr)
		{
			return;
		}
	}
	AActor* BossEnemyActor = UGameplayStatics::GetActorOfClass(this, AKBossZombieEnemy::StaticClass());
	if ( BossEnemyActor )
	{
		// 액터가 존재하는 경우, 해당 액터에서 컴포넌트를 가져옵니다
		BossEnemyFSM = BossEnemyActor->GetComponentByClass<UKEnemyFSM>();
		if ( EnemyFSM == nullptr )
		{
			return;
		}
	}	
	AActor* BeginnerEnemyActor = UGameplayStatics::GetActorOfClass(this, AKBeginnerZombieEnemy::StaticClass());
	if ( BeginnerEnemyActor )
	{
		// 액터가 존재하는 경우, 해당 액터에서 컴포넌트를 가져옵니다
		NormalEnemyFSM = BeginnerEnemyActor->GetComponentByClass<UKEnemyFSM>();
		if ( NormalEnemyFSM == nullptr )
		{
			return;
		}
	}
	PlayerController = Cast<APlayerController>(GetController());

	RightAttackSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	LeftAttackSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	//카메라액터 얻어오기
	FieldCamera = Cast<ACameraActor>(UGameplayStatics::GetActorOfClass(this, ACameraActor::StaticClass()));

	//체력 초기화
	PlayerUI->SetHpBar(HealthPoints, MAXHP);
}

void AJPlayer::OverlapDamage(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//if ( OtherActor && OtherActor != this )
	//{
	//	HitActors.AddUnique(OtherActor); //데미지를 입을 액터들
	//}
	//for ( AActor* Actor : HitActors )
	//{
		//if ( Actor )
		//{
			//일단 타격데미지 10으로 설정
	Boss = Cast<AKBossZombieEnemy>(OtherActor);
	if ( Boss  && OtherComp == Boss->GetCapsuleComponent())
	{
		UE_LOG(LogTemp, Log, TEXT("Boss  Applied"));
		FPointDamageEvent DamageEvent(10, FHitResult(), GetActorForwardVector(), nullptr);
		AController* ActorController = nullptr;
		//애들 컨트롤러 얻어와서 데미지 주기
		APawn* ActorPawn = Cast<APawn>(OtherActor);
		if ( ActorPawn )
		{
			ActorController = ActorPawn->GetController();
		}
		ActorPawn->TakeDamage(10, DamageEvent, ActorController, this);

	}
	FSMOwner = Cast<AKNormalZombieEnemy>(OtherActor);

	if (FSMOwner && OtherComp == FSMOwner->GetCapsuleComponent() )
	{
		UE_LOG(LogTemp, Log, TEXT("Applied"));
		FPointDamageEvent DamageEvent(10, FHitResult(), GetActorForwardVector(), nullptr);
		AController* ActorController = nullptr;
		//애들 컨트롤러 얻어와서 데미지 주기
		APawn* ActorPawn = Cast<APawn>(OtherActor);
		if ( ActorPawn )
		{
			ActorController = ActorPawn->GetController();
		}
		ActorPawn->TakeDamage(10, DamageEvent, ActorController, this);
		UE_LOG(LogTemp, Log, TEXT("Applied 10 damage to %s"), *ActorPawn->GetName());
	}
}
void AJPlayer::StartInteractionEvent()
{
	InteractionUI->SetVisibility(ESlateVisibility::Visible);
	CurrentKeyPresses = 0;
}
float AJPlayer::GetKeyProcessPercent()
{
	return (float)CurrentKeyPresses / RequiredKeyPresses;
}
bool AJPlayer::GetIsExecuting()
{
	return bIsExecuting;
}

bool AJPlayer::GetIsGrabbed()
{
	return bIsGrabbed;
}

float AJPlayer::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float DamageToApply = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser); //늘 부모를 오버라이드 해주자
	DamageToApply = FMath::Min(HealthPoints, DamageToApply); //남은 체력보다 입을 데미지가 더 크면(체력이 0이면) 데미지는 0이 된다.
	HealthPoints -= DamageToApply;
	CharacterAnimInstance->PlayHitMontage();
	UE_LOG(LogTemp, Warning, TEXT("%f"), HealthPoints);
	bIsAttacking = false;
	bInventoryOn = false;
	CharacterAnimInstance->bChangingWeapon = false;
	PlayerUI->SetHpBar(HealthPoints, MAXHP);

	if ( HealthPoints <= 0 ) //체력이 0이하가 되면 
	{	
		DetachFromControllerPendingDestroy(); //컨트롤러 떼버림
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision); //캡슐컴포넌트 떼어내서
		CharacterAnimInstance->PlayDieMontage();
		// 게임 멈추기
		UGameplayStatics::SetGamePaused(GetWorld(), true);

		
		// 입력 모드 설정
		//APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
		//if ( PlayerController )
		//{
		//	// 마우스 커서 보이게 하기
		//	FInputModeUIOnly InputMode;
		//	InputMode.SetWidgetToFocus(_restartUI->TakeWidget());
		//	PlayerController->SetInputMode(InputMode);
		//	PlayerController->bShowMouseCursor = true;
		//}
	}

	return DamageToApply;	
}

// Called when the game starts or when spawned

void AJPlayer::ReadyToExcecute(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ExecutionTarget1 = Cast<AKNormalZombieEnemy>(OtherComp->GetOwner());
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
	CharacterAnimInstance->bChangingWeapon = false;

	if ( CharacterEquipState == ECharacterEquipState::ECES_BatEquipped && CharacterAnimInstance->bChangeSpringArmComp == false )
	{
			bSprintArmCompBack = true;
			UE_LOG(LogTemp, Error, TEXT("ADSFBZCXV"));
	}
	else
	{
		bSprintArmCompBack = false;
	}
}

void AJPlayer::AttackStartComboState()
{
	if ( CharacterEquipState == ECharacterEquipState::ECES_BatEquipped )
	{
		MaxCombo = 3;
	}
	else if ( CharacterEquipState == ECharacterEquipState::ECES_UnEquipped )
	{
		MaxCombo = 4;
	}

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

	CameraComp->FieldOfView = FMath::Lerp(CameraComp->FieldOfView, TargetFOV, DeltaTime * 5);

	if ( CharacterAnimInstance->bChangeSpringArmComp )
	{
		SetCameraForBatAction(DeltaTime);
	}
	if ( bSprintArmCompBack )
	{
		SetCameraBackForBatAction(DeltaTime);
	}
	GEngine->AddOnScreenDebugMessage(3, 1.0f, FColor::Green, FString::Printf(TEXT("Player HP : %f"), HealthPoints));

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
		EnhancedInputComponent->BindAction(IA_Zoom, ETriggerEvent::Started, this, &AJPlayer::Zoom);
		EnhancedInputComponent->BindAction(IA_Zoom, ETriggerEvent::Completed, this, &AJPlayer::ZoomOut);
		EnhancedInputComponent->BindAction(IA_Run, ETriggerEvent::Started, this, &AJPlayer::Run);
		EnhancedInputComponent->BindAction(IA_Run, ETriggerEvent::Completed, this, &AJPlayer::Run);
		EnhancedInputComponent->BindAction(IA_Crouch, ETriggerEvent::Completed, this, &AJPlayer::Crouching);
		EnhancedInputComponent->BindAction(IA_Grab, ETriggerEvent::Triggered, this, &AJPlayer::HandleQTEInput);
		EnhancedInputComponent->BindAction(IA_TakeDown, ETriggerEvent::Triggered, this, &AJPlayer::NewTakeDown);
		EnhancedInputComponent->BindAction(IA_EquipGun, ETriggerEvent::Triggered, this, &AJPlayer::SetStateEquipGun);
		EnhancedInputComponent->BindAction(IA_EquipThrowWeapon, ETriggerEvent::Triggered, this, &AJPlayer::SetStateEquipThrowWeapon);
		EnhancedInputComponent->BindAction(IA_UnEquipped, ETriggerEvent::Triggered, this, &AJPlayer::SetStateUnEquipped);
		EnhancedInputComponent->BindAction(IA_BatEquipped, ETriggerEvent::Triggered, this, &AJPlayer::SetStateBatEquipped);
		EnhancedInputComponent->BindAction(IA_DevelopeMode, ETriggerEvent::Triggered, this, &AJPlayer::GunSuperMode);
		EnhancedInputComponent->BindAction(IA_Inventory, ETriggerEvent::Triggered, this, &AJPlayer::InventoryOn);
		EnhancedInputComponent->BindAction(IA_ShotgunEquipped, ETriggerEvent::Triggered, this, &AJPlayer::SetStateShotgunEquipped);

	}
}

void AJPlayer::Move(const FInputActionValue& Value)
{
	if ( bIsGrabbed || bIsAttacking || CharacterAnimInstance->GetChangingWeapon())
	{
		UE_LOG(LogTemp, Warning, TEXT("grab : %d, attack : %d, changeweapon : %d"), bIsGrabbed, bIsAttacking, CharacterAnimInstance->GetChangingWeapon());
		return;
	}
	const FVector2D Vector = Value.Get<FVector2D>();
	FRotator Rotation = GetController()->GetControlRotation(); //플레이어의 방향 읽어서 
	FRotator YawRotation(0, Rotation.Yaw, 0); //yaw사용

	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	AddMovementInput(ForwardDirection, Vector.X); 

	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
	AddMovementInput(RightDirection, Vector.Y);
	

}

void AJPlayer::Look(const FInputActionValue& Value)
{
	if ( bIsGrabbed || bInventoryOn )	return;

	FVector2D LV = Value.Get<FVector2D>();
	AddControllerPitchInput(-LV.Y);
	AddControllerYawInput(LV.X);
}
void AJPlayer::Fire(const FInputActionValue& Value)
{
	if ( bIsGrabbed ) return;
	//if ( Gun->CurrentBulletNum == 0 )
	//{
	//	CharacterAnimInstance->PlayGunShotMontage();
	//	CharacterAnimInstance->PlayGunShotMontageSection(TEXT("Reload"));
	//	return;
	//}
	if ( CharacterEquipState == ECharacterEquipState::ECES_GunEquipped)
	{
		if ( CharaterState == ECharacterState::ECS_Crouching ) return;
		if ( Gun->CurrentBulletNum == 0 )
		{
			CharacterAnimInstance->PlayGunShotMontage();
			CharacterAnimInstance->PlayGunShotMontageSection(TEXT("Reload"));
			Gun->ReLoad();
			return;
		}
		CharacterAnimInstance->PlayGunShotMontage();
		CharacterAnimInstance->PlayGunShotMontageSection(TEXT("Shot"));
		Gun->PullTrigger();
		PlayerUI->SetPistolBulletNum();
		CameraShake();
	}
	else if ( CharacterEquipState == ECharacterEquipState::ECES_UnEquipped || CharacterEquipState == ECharacterEquipState::ECES_BatEquipped )
	{
		if ( CharaterState == ECharacterState::ECS_Crouching ) return;
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
			CharacterAnimInstance->PlayAttackMontage();
			bIsAttacking = true;
		}
	}
	else if ( CharacterEquipState == ECharacterEquipState::ECES_ShotgunEquipped )
	{
		if ( CharaterState == ECharacterState::ECS_Crouching ) return;
		if ( Shotgun->CurrentBulletNum == 0 ) return;
		CharacterAnimInstance->PlayShotgunMontage();
		Shotgun->PullTrigger();
		PlayerUI->SetShotGunBulletNum();
	}
}
void AJPlayer::Zoom(const FInputActionValue& Value)
{
	if ( bIsGrabbed ) return;
	/*SpringArmComp->SetRelativeLocation(FVector(-72, 270, 80));*/
	if ( CharacterEquipState == ECharacterEquipState::ECES_GunEquipped)
	{
		TargetFOV = 30;
		LockOnComp->SetTargetLockTrue();
		CharacterAnimInstance->PlayGunShotMontage();
		CharacterAnimInstance->PlayGunShotMontageSection(TEXT("Idle"));
	}
	else if ( CharacterEquipState == ECharacterEquipState::ECES_ThrowWeaponEquipped )
	{
		TargetFOV = 60;
	}
	else if ( CharacterEquipState == ECharacterEquipState::ECES_ShotgunEquipped )
	{
		TargetFOV = 30;
		LockOnComp->SetTargetLockTrue();
	}

}
void AJPlayer::ZoomOut(const FInputActionValue& Value)
{
	if ( bIsGrabbed ) return;
	/*SpringArmComp->SetRelativeLocation(FVector(-72, 270, 80));*/
	if ( CharacterEquipState == ECharacterEquipState::ECES_GunEquipped || CharacterEquipState == ECharacterEquipState::ECES_ThrowWeaponEquipped ||
		CharacterEquipState == ECharacterEquipState::ECES_ShotgunEquipped )
	{
		TargetFOV = 90;
	}
	CharacterAnimInstance->StopGunshotMontage();
	LockOnComp->SetTargetLockFalse();

}

void AJPlayer::Run(const FInputActionValue& Value)
{
	if ( CharaterState == ECharacterState::ECS_Crouching || bIsGrabbed ) return;
	//크라우칭중이 아니라면

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
	//쉬프트 <> 크라우칭 버그 있음
	
}

void AJPlayer::InventoryOn(const FInputActionValue& Value)
{
	bInventoryOn = !bInventoryOn;
	if ( bInventoryOn )
	{
		Inventory->SetVisibility(ESlateVisibility::Visible);
		Inventory->SetBulletNums();
		PlayerController->bShowMouseCursor = true;
	}
	else
	{
		Inventory->SetVisibility(ESlateVisibility::Hidden);
		PlayerController->bShowMouseCursor = false;
	}
}

void AJPlayer::Crouching(const FInputActionValue& Value)
{
	if ( bIsGrabbed ) return;
	bCrouched = !bCrouched;
	if ( bCrouched )
	{
		CharaterState = ECharacterState::ECS_Crouching;
		CharacterMovement->MaxWalkSpeed = 300;
		Crouch();
	}
	else
	{
		CharaterState = ECharacterState::ECS_UnGrabbed;
		CharacterMovement->MaxWalkSpeed = 400;
		UnCrouch();
	}
}
void AJPlayer::NewTakeDown(const FInputActionValue& Value)
{
	if ( bIsGrabbed ) return;
	//camera ignore 해놓기
	//pawn이기
	FHitResult HitResult;
	FVector Start = GetActorLocation();
	FVector End = Start + (GetActorForwardVector() * 250);
	ECollisionChannel TraceChannel = ECC_Pawn;  // Trace channel 설정

	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(this); // 자기 자신을 무시

	bool bHit = GetWorld()->LineTraceSingleByChannel(
		HitResult,
		Start,
		End,
		TraceChannel,
		CollisionParams
	);

	// 디버그 라인 그리기
#if WITH_EDITOR
	DrawDebugLine(
		GetWorld(),
		Start,
		End,
		bHit ? FColor::Green : FColor::Red, // 히트 여부에 따라 색상 변경
		false, // 두드러진 레이아웃
		1.0f,  // 지속 시간 (초)
		0,     // 선 두께
		1.0f   // 선의 깊이 (디버그 라인의 두께)
	);

#endif
	if ( bHit )
	{
		ExecutionTarget1 = Cast<AKNormalZombieEnemy>(HitResult.GetActor());
		ExecutionTarget2 = Cast<AKBeginnerZombieEnemy>(HitResult.GetActor());
		if ( ExecutionTarget1 )
		{
			CharaterState = ECharacterState::ECS_Crouching;
			FTransform t = ExecutionTarget1->GetAttackerTransform();
			SetActorLocation(ExecutionTarget1->GetAttackerTransform().GetLocation() + FVector(10, 0, 0));
			bIsExecuting = true;
			SpringArmComp->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, TEXT("mixamorig_Spine1"));
			//SpringArmComp->bUsePawnControlRotation = false;
			//SpringArmComp->SetRelativeRotation(FRotator(237, -84, -18));
			CameraComp->SetupAttachment(SpringArmComp);
			SpringArmComp->TargetArmLength = 150;
			//움직임 빼기
			UKEnemyFSM* FFSSMM = ExecutionTarget1->GetComponentByClass<UKEnemyFSM>();
			if ( FFSSMM && bCanExecute )
			{
				FFSSMM->SetState(EEnemyState::EXECUTED);
				//좀비가 자꾸 움직임..
			}
			//컨트롤러 방향
			PlayerController->AController::SetControlRotation(t.GetRotation().Rotator());

			if ( Subsystem )
			{
				//컨트롤러 떼기
				Subsystem->RemoveMappingContext(IMC_Joel);
			}

			CharacterAnimInstance->PlayExecuteMontage();
			//몽타주 끝나면 상태 바꾸기
			GetWorldTimerManager().SetTimer(TakeDownTimer, this, &AJPlayer::AfterTakeDown, 6.f, false);

			UKEnemyAnim* EnemyAnim = Cast<UKEnemyAnim>(ExecutionTarget1->GetMesh()->GetAnimInstance());
			if ( EnemyAnim )
			{
				UE_LOG(LogTemp, Error, TEXT("ENEMYANIM"));
				EnemyAnim->PlayEnemyTDamageAnim(TEXT("Executed"));
				FTimerHandle ExcecuteTimer;
				GetWorldTimerManager().SetTimer(ExcecuteTimer, this, &AJPlayer::EnemyIsDead, 6.5f, false);
			}
		}
		else if ( ExecutionTarget2 )
		{
			CharaterState = ECharacterState::ECS_Crouching;
			FTransform t = ExecutionTarget2->GetAttackerTransform();
			SetActorLocation(ExecutionTarget2->GetAttackerTransform().GetLocation() + FVector(10, 0, 0));
			bIsExecuting = true;
			SpringArmComp->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, TEXT("mixamorig_Spine1"));
			//SpringArmComp->bUsePawnControlRotation = false;
			//SpringArmComp->SetRelativeRotation(FRotator(237, -84, -18));
			CameraComp->SetupAttachment(SpringArmComp);
			SpringArmComp->TargetArmLength = 150;
			//움직임 빼기
			UKEnemyFSM* FFSSMM = ExecutionTarget2->GetComponentByClass<UKEnemyFSM>();
			if ( FFSSMM && bCanExecute )
			{
				FFSSMM->SetState(EEnemyState::EXECUTED);
				//좀비가 자꾸 움직임..
			}
			//컨트롤러 방향
			PlayerController->AController::SetControlRotation(t.GetRotation().Rotator());

			if ( Subsystem )
			{
				//컨트롤러 떼기
				Subsystem->RemoveMappingContext(IMC_Joel);
			}

			CharacterAnimInstance->PlayExecuteMontage();
			//몽타주 끝나면 상태 바꾸기
			GetWorldTimerManager().SetTimer(TakeDownTimer, this, &AJPlayer::AfterTakeDown, 6.f, false);

			UKEnemyAnim* EnemyAnim = Cast<UKEnemyAnim>(ExecutionTarget2->GetMesh()->GetAnimInstance());
			if ( EnemyAnim )
			{
				UE_LOG(LogTemp, Error, TEXT("ENEMYANIM"));
				EnemyAnim->PlayEnemyTDamageAnim(TEXT("Executed"));
				FTimerHandle ExcecuteTimer;
				GetWorldTimerManager().SetTimer(ExcecuteTimer, this, &AJPlayer::EnemyIsDead, 6.5f, false);
			}
		}
	}
}

void AJPlayer::EnemyIsDead()
{
	SetActorLocation(GetActorForwardVector() * 175 + GetActorLocation());

	if(ExecutionTarget1) ExecutionTarget1->Destroy();

	if ( ExecutionTarget2 ) ExecutionTarget2->Destroy();
	SpringArmComp->AttachToComponent(
			GetCapsuleComponent(),
			FAttachmentTransformRules::KeepRelativeTransform
		);
	SpringArmComp->SetRelativeLocation(FVector(0, 40, 80));
	SpringArmComp->SetRelativeRotation(FRotator::ZeroRotator);
	CameraComp->SetupAttachment(SpringArmComp);
	
	bCanExecute = false;

	bIsExecuting = false;

}

void AJPlayer::AfterTakeDown()
{
	UE_LOG(LogTemp, Warning, TEXT("ERER"));
	//if ( PlayerController )
	//{
	//	PlayerController->SetIgnoreMoveInput(false);
	//	PlayerController->SetIgnoreLookInput(false);
	//}
	if ( Subsystem )
	{
		Subsystem->AddMappingContext(IMC_Joel, 0);
	}
}
void AJPlayer::SetCameraBackForBatAction(float DeltaTime)
{
	UE_LOG(LogTemp, Error, TEXT("BATT2"));
	FVector InitialLocation = SpringArmComp->GetComponentLocation();
	FVector RootLocation = CameraInitialPostion;
	FVector NewLocation = FMath::Lerp(InitialLocation, RootLocation, DeltaTime * 40);
	SpringArmComp->SetWorldLocation(NewLocation);
	TargetFOV = 90;
	//스프링암이 목표 위치에 거의 도달했는지 확인
	if ( FVector::Dist(NewLocation, RootLocation) < 10.0f ) 
	{
		// 목표 위치에 도달했으므로 스프링암을 소켓에 부착
		SpringArmComp->AttachToComponent(
			GetCapsuleComponent(),
			FAttachmentTransformRules::SnapToTargetNotIncludingScale
		);
		UE_LOG(LogTemp, Error, TEXT("루트에 붙임"));
		bSprintArmCompBack = false;
	}
	SpringArmComp->SetRelativeLocation(FVector(0, 40, 80));
	SpringArmComp->SetRelativeRotation(FRotator::ZeroRotator);
	SpringArmComp->TargetArmLength = 200;
	//CameraComp->SetupAttachment(SpringArmComp);
	//SpringArmComp->bEnableCameraLag = false;
	//CameraComp->SetFieldOfView(90);
	UE_LOG(LogTemp, Error, TEXT("BATT"));
}

void AJPlayer::SetCameraForBatAction(float DeltaTime)
{
	if ( CharacterEquipState == ECharacterEquipState::ECES_BatEquipped )
	{
		//FVector SocketOffset(-55.f, -145.5f, -27.f); // 소켓에서 약간 위쪽으로 오프셋
		//FRotator SocketRotation(14.5f, 42.f, -194.5f);
		
		FVector InitialLocation = SpringArmComp->GetComponentLocation();
		CameraInitialPostion = InitialLocation;
		FVector BoneLocation = GetMesh()->GetBoneLocation(TEXT("mixamorig_RightShoulder"));
		FVector NewLocation = FMath::Lerp(InitialLocation, BoneLocation, DeltaTime * 40);
		SpringArmComp->SetWorldLocation(NewLocation);
		TargetFOV = 75;
		//스프링암이 목표 위치에 거의 도달했는지 확인
		if ( FVector::Dist(NewLocation, BoneLocation) < 1.0f ) // 거리를 적절하게 조정하세요
		{
			// 목표 위치에 도달했으므로 스프링암을 소켓에 부착
			SpringArmComp->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, TEXT("mixamorig_RightShoulder"));
			SpringArmComp->SetRelativeLocation(FVector::ZeroVector);
			SpringArmComp->SetRelativeRotation(FRotator::ZeroRotator);
			UE_LOG(LogTemp, Error, TEXT("본에 붙임"));
			CharacterAnimInstance->bChangeSpringArmComp = false;
		}
	}
	//CharacterAnimInstance->bChangeSpringArmComp = false;
}

void AJPlayer::SetStateEquipGun()
{
	if ( CharaterState == ECharacterState::ECS_Crouching )
	{
		CharacterAnimInstance->PlayEquipAnimMontage(TEXT("Crouch"));
	}
	else
	{
		CharacterAnimInstance->PlayEquipAnimMontage(TEXT("Standing"));
	}
	//총기 활성화 
	Gun->SetActorHiddenInGame(false);
	Gun->SetActorEnableCollision(true);
	Bat->SetActorHiddenInGame(true);
	Bat->SetActorEnableCollision(false);
	Shotgun->SetActorHiddenInGame(true);
	CharacterEquipState = ECharacterEquipState::ECES_GunEquipped;
	GunWidget->SetVisibility(ESlateVisibility::Visible);
	PlayerUI->SetPistolEquipped();
}
void AJPlayer::SetStateEquipThrowWeapon()
{
	if ( CharaterState == ECharacterState::ECS_Crouching )
	{
		CharacterAnimInstance->PlayEquipAnimMontage(TEXT("Crouch"));
	}
	else
	{
		CharacterAnimInstance->PlayEquipAnimMontage(TEXT("Standing"));
	}
	//투척무기 활성화 총, 배트 둘다 안보이고 콜리전 비활
	Gun->SetActorHiddenInGame(true);
	Gun->SetActorEnableCollision(false);
	Bat->SetActorHiddenInGame(true);
	Bat->SetActorEnableCollision(false);
	Shotgun->SetActorHiddenInGame(true);
	CharacterEquipState = ECharacterEquipState::ECES_ThrowWeaponEquipped;
	GunWidget->SetVisibility(ESlateVisibility::Hidden);
	PlayerUI->SetFireBottleEquipped();
}
void AJPlayer::SetStateUnEquipped()
{
	//노 무기 총, 배트 둘다 안보이고 콜리전 비활
	Gun->SetActorHiddenInGame(true);
	Gun->SetActorEnableCollision(false);
	Bat->SetActorHiddenInGame(true);
	Bat->SetActorEnableCollision(false);
	Shotgun->SetActorHiddenInGame(true);
	CharacterEquipState = ECharacterEquipState::ECES_UnEquipped;
	GunWidget->SetVisibility(ESlateVisibility::Hidden);
}
void AJPlayer::SetStateBatEquipped()
{
	if ( CharaterState == ECharacterState::ECS_Crouching )
	{
		CharacterAnimInstance->PlayEquipAnimMontage(TEXT("Crouch"));
	}
	else
	{
		CharacterAnimInstance->PlayEquipAnimMontage(TEXT("Standing"));
	}
	//배트 장착, 총 안보이고 콜리전 비활성화
	Bat->SetActorHiddenInGame(false);
	Bat->SetActorEnableCollision(true);
	Gun->SetActorHiddenInGame(true);
	Gun->SetActorEnableCollision(false);
	Shotgun->SetActorHiddenInGame(true);
	CharacterEquipState = ECharacterEquipState::ECES_BatEquipped;
	GunWidget->SetVisibility(ESlateVisibility::Hidden);
	PlayerUI->SetBatEquipped();
}
void AJPlayer::SetStateShotgunEquipped()
{
	if ( CharaterState == ECharacterState::ECS_Crouching )
	{
		CharacterAnimInstance->PlayEquipAnimMontage(TEXT("Crouch"));
	}
	else
	{
		CharacterAnimInstance->PlayEquipAnimMontage(TEXT("Standing"));
	}
	CharacterEquipState = ECharacterEquipState::ECES_ShotgunEquipped;
	Gun->SetActorHiddenInGame(true);
	Gun->SetActorEnableCollision(false);
	Bat->SetActorHiddenInGame(true);
	Bat->SetActorEnableCollision(false);
	Shotgun->SetActorHiddenInGame(false);
	PlayerUI->SetShotGunEquipped();
}
void AJPlayer::GunSuperMode()
{
	Gun->GunDamage = 300;
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

void AJPlayer::MoveFieldCamera()
{
	FTransform t = CameraPostion->K2_GetComponentToWorld();
	FieldCamera->SetActorTransform(t);
	CameraComp->SetActive(false);
	FieldCamera->GetRootComponent()->SetActive(true);
	PlayerController->SetViewTargetWithBlend(FieldCamera, 1.f);

}

void AJPlayer::StartGrabbedState(AActor* Enemy)
{
	bIsGrabbed = true;
	CurrentKeyPresses = 0;
	GrabbedEnemy = Cast<AKBaseEnemy>(Enemy);

	FLatentActionInfo LatentInfo;
	LatentInfo.CallbackTarget = this;

	bUseControllerRotationYaw = false; //회전하게 하기(이거 꺼야 회전함) -> 나중에 다시 true로 돌려놓기
	FRotator rot = UKismetMathLibrary::FindLookAtRotation(this->GetActorLocation(), GrabbedEnemy->GetActorLocation());//서로 바라보는 방향
  
	UKismetSystemLibrary::MoveComponentTo(
	GetCapsuleComponent(),              // 이동할 컴포넌트
	GetActorLocation(),                   // 목표 위치
	rot,         // 목표 회전
	false,                              // 즉시 스냅
	true,
	1.f,                            // 텔레포트하지 않음
	false,
	EMoveComponentAction::Type::Move,
	LatentInfo
		);
	MoveFieldCamera();
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	if ( this )
	{
		//GetController()->SetControlRotation(rot);

		// 저항 애니메이션 재생 (블루프린트에서 설정된 ResistanceMontage)
		if ( CharacterAnimInstance )
		{
			CharacterAnimInstance->Montage_Stop(0.2f);
			CharacterAnimInstance->PlayResistanceMontage();
		}
		GetController()->SetIgnoreMoveInput(true);
		// QTE UI 표시
		StartQTEGrabEvent();
	}
}

void AJPlayer::StopGrabbedState(bool bSuccess)
{
	bIsGrabbed = false;
	bIsAttacking = false;
	CameraComp->SetActive(true);
	FieldCamera->GetRootComponent()->SetActive(false);
	PlayerController->SetViewTargetWithBlend(CameraComp->GetOwner(), 0.5f);

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	// 발로 차는 애니메이션 실행
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
		HealthPoints -= 30;
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

	SpringArmComp->AttachToComponent(
		GetCapsuleComponent(),
		FAttachmentTransformRules::KeepRelativeTransform
	);
	SpringArmComp->SetRelativeLocation(FVector(0, 40, 80));
	SpringArmComp->SetRelativeRotation(FRotator::ZeroRotator);
	SpringArmComp->TargetArmLength = 200;
	CameraComp->SetupAttachment(SpringArmComp);

	// QTE 이벤트가 끝났음을 전역 변수에 표시
	 if(EnemyFSM) EnemyFSM->bIsQTEActive = false;

	 if(NormalEnemyFSM) NormalEnemyFSM->bIsQTEActive = false; //위험함
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
			GEngine->AddOnScreenDebugMessage(5, 1, FColor::Green, TEXT("Escaped from Grab!"));
			GetController()->SetIgnoreMoveInput(false);
		}
	}
	else if ( CharaterState == ECharacterState::ECS_Reversed )
	{
		CurrentKeyPresses++;
		if ( CurrentKeyPresses >= 10)
		{
			//몽타주 재생하면서
			//정상화
			InteractionUI->SetVisibility(ESlateVisibility::Hidden);

			GetCapsuleComponent()->SetSimulatePhysics(false);
			SetActorRotation(FRotator(0, 0, 0));

			CharacterAnimInstance->PlayFallingMontage();

			CharaterState = ECharacterState::ECS_UnGrabbed;

			PlayerController->PlayerCameraManager->ViewPitchMin = -30;
			PlayerController->PlayerCameraManager->ViewPitchMin = 30;
		}
	}
	else
	{
		InteractionUI->SetVisibility(ESlateVisibility::Hidden);
		InteractionEnd = true;
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
		bUseControllerRotationYaw = true;
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