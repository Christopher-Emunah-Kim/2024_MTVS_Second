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

	GetMesh()->SetRelativeRotation(FRotator(0, -90, 0));

	// AI Perception Stimuli Source Component 생성 및 초기화
	PerceptionStimuliSource = CreateDefaultSubobject<UAIPerceptionStimuliSourceComponent>(TEXT("PerceptionStimuliSource"));
	PerceptionStimuliSource->RegisterForSense(TSubclassOf<UAISense_Hearing>());

	// 팀 타입 설정 (플레이어는 적)
	TeamType = ETeamType::ENEMY;

	//GrabEnemy초기화
	GrabbedEnemy = nullptr;

	//Crouch함수 사용을 위한 등록
	GetMovementComponent()->GetNavAgentPropertiesRef().bCanCrouch = true;

	RightAttackSphere = CreateDefaultSubobject<USphereComponent>(TEXT("RightAttackSphere"));
	RightAttackSphere->SetupAttachment(GetMesh(), TEXT("mixamorig_RightHand"));
	RightAttackSphere->SetSphereRadius(3.f);

	LeftAttackSphere = CreateDefaultSubobject<USphereComponent>(TEXT("LeftAttackSphere"));
	LeftAttackSphere->SetupAttachment(GetMesh(), TEXT("mixamorig_LeftHand"));
	LeftAttackSphere->SetSphereRadius(3.f);

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

void AJPlayer::BeginPlay()
{
	Super::BeginPlay();
	APlayerController* PC = Cast<APlayerController>(GetController());
	Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer());
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
	if ( RightAttackSphere )
	{
		RightAttackSphere->OnComponentBeginOverlap.AddDynamic(this, &AJPlayer::OverlapDamage);
	}	
	if ( LeftAttackSphere )
	{
		LeftAttackSphere->OnComponentBeginOverlap.AddDynamic(this, &AJPlayer::OverlapDamage);
	}

	Gun = GetWorld()->SpawnActor<APlayerGun>(GunClass);
	Gun->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, TEXT("GunSocket"));
	Gun->SetActorHiddenInGame(true);

	Bat = GetWorld()->SpawnActor<AJPlayerBat>(BatClass);
	Bat->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, TEXT("mixamorig_RightHand"));
	Bat->SetActorHiddenInGame(true);
	Bat->SetActorEnableCollision(false);

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
	PlayerController = Cast<APlayerController>(GetController());

	HP = MAXHP;
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
	}
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
	DamageToApply = FMath::Min(HP, DamageToApply); //남은 체력보다 입을 데미지가 더 크면(체력이 0이면) 데미지는 0이 된다.
	HP -= DamageToApply;
	UE_LOG(LogTemp, Warning, TEXT("%f"), HP);

	if ( HP <= 0 ) //체력이 0이하가 되면 
	{	
		DetachFromControllerPendingDestroy(); //컨트롤러 떼버림
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision); //캡슐컴포넌트 떼어내서
	}

	return DamageToApply;	
}

// Called when the game starts or when spawned

void AJPlayer::ReadyToExcecute(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
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
	if ( CharacterEquipState == ECharacterEquipState::ECES_BatEquipped )
	{
		MaxCombo = 3;
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
		EnhancedInputComponent->BindAction(IA_TakeDown, ETriggerEvent::Triggered, this, &AJPlayer::NewTakeDown);
		EnhancedInputComponent->BindAction(IA_EquipGun, ETriggerEvent::Triggered, this, &AJPlayer::SetStateEquipGun);
		EnhancedInputComponent->BindAction(IA_EquipThrowWeapon, ETriggerEvent::Triggered, this, &AJPlayer::SetStateEquipThrowWeapon);
		EnhancedInputComponent->BindAction(IA_UnEquipped, ETriggerEvent::Triggered, this, &AJPlayer::SetStateUnEquipped);
		EnhancedInputComponent->BindAction(IA_BatEquipped, ETriggerEvent::Triggered, this, &AJPlayer::SetStateBatEquipped);
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
	if ( bIsGrabbed )	return;

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
	else if ( CharacterEquipState == ECharacterEquipState::ECES_UnEquipped || CharacterEquipState == ECharacterEquipState::ECES_BatEquipped )
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
			CharacterAnimInstance->PlayAttackMontage();
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
void AJPlayer::NewTakeDown(const FInputActionValue& Value)
{
	FHitResult HitResult;
	FVector Start = GetActorLocation();
	FVector End = Start + (GetActorForwardVector() * 250);
	ECollisionChannel TraceChannel = ECC_Visibility;  // Trace channel 설정

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
	if (bHit && ExecutionTarget)
	{
		bIsExecuting = true;
		SpringArmComp->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, TEXT("mixamorig_Spine1"));
		SpringArmComp->SetRelativeRotation(FRotator(0, 180, 0));
		CameraComp->SetupAttachment(SpringArmComp);
		SpringArmComp->TargetArmLength = 200;
		ExecutionTarget = Cast<AKNormalZombieEnemy>(HitResult.GetActor());
	//움직임 빼기
		UKEnemyFSM* FFSSMM = ExecutionTarget->GetComponentByClass<UKEnemyFSM>();
		if ( FFSSMM && bCanExecute )
		{
			FFSSMM->SetState(EEnemyState::EXECUTED);
			//좀비가 자꾸 움직임..
		}
		FTransform t = ExecutionTarget->GetAttackerTransform();
		SetActorLocation(t.GetLocation());
		//컨트롤러 방향
		PlayerController->AController::SetControlRotation(t.GetRotation().Rotator());

		if ( Subsystem )
		{
			//컨트롤러 떼기
			Subsystem->RemoveMappingContext(IMC_Joel);
		}
		CharaterState = ECharacterState::ECS_Crouching;

		CharacterAnimInstance->PlayExecuteMontage();
		//몽타주 끝나면 상태 바꾸기
		GetWorldTimerManager().SetTimer(TakeDownTimer, this, &AJPlayer::AfterTakeDown, 6.f, false);

		UKEnemyAnim* EnemyAnim = Cast<UKEnemyAnim>(ExecutionTarget->GetMesh()->GetAnimInstance());
		if ( EnemyAnim )
		{
			EnemyAnim->PlayEnemyTDamageAnim(TEXT("Executed"));
			FTimerHandle ExcecuteTimer;
			GetWorldTimerManager().SetTimer(ExcecuteTimer, this, &AJPlayer::EnemyIsDead, 6.5f, false);
		}
	}
}

void AJPlayer::EnemyIsDead()
{
	SetActorLocation(GetActorForwardVector() * 175 + GetActorLocation());
	ExecutionTarget->Destroy();
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
void AJPlayer::SetCameraBack()
{
	bUseControllerRotationYaw = true;
}
void AJPlayer::SetCameraBoomToCharacter(bool bSetCameraBoom)
{
	if ( SpringArmComp && bSetCameraBoom)
	{
		SpringArmComp->SetWorldLocation(GetCapsuleComponent()->GetComponentLocation());
		SpringArmComp->SetWorldRotation(GetCapsuleComponent()->GetComponentRotation());
	}
}
void AJPlayer::SetStateEquipGun()
{
	//총기 활성화 
	Gun->SetActorHiddenInGame(false);
	Gun->SetActorEnableCollision(true);
	Bat->SetActorHiddenInGame(true);
	Bat->SetActorEnableCollision(false);
	CharacterEquipState = ECharacterEquipState::ECES_GunEquipped;
}
void AJPlayer::SetStateEquipThrowWeapon()
{
	//투척무기 활성화 총, 배트 둘다 안보이고 콜리전 비활
	Gun->SetActorHiddenInGame(true);
	Gun->SetActorEnableCollision(false);
	Bat->SetActorHiddenInGame(true);
	Bat->SetActorEnableCollision(false);
	CharacterEquipState = ECharacterEquipState::ECES_ThrowWeaponEquipped;
}
void AJPlayer::SetStateUnEquipped()
{
	//노 무기 총, 배트 둘다 안보이고 콜리전 비활
	Gun->SetActorHiddenInGame(true);
	Gun->SetActorEnableCollision(false);
	Bat->SetActorHiddenInGame(true);
	Bat->SetActorEnableCollision(false);
	CharacterEquipState = ECharacterEquipState::ECES_UnEquipped;
}
void AJPlayer::SetStateBatEquipped()
{
	//배트 장착, 총 안보이고 콜리전 비활성화
	Bat->SetActorHiddenInGame(false);
	Gun->SetActorHiddenInGame(true);
	Gun->SetActorEnableCollision(false);
	CharacterEquipState = ECharacterEquipState::ECES_BatEquipped;
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

	FLatentActionInfo LatentInfo;
	LatentInfo.CallbackTarget = this;

	bUseControllerRotationYaw = false;	
	FRotator rot = UKismetMathLibrary::FindLookAtRotation(this->GetActorLocation(), GrabbedEnemy->GetActorLocation());

	UKismetSystemLibrary::MoveComponentTo(
	GetCapsuleComponent(),              // 이동할 컴포넌트
	GetActorLocation(),                   // 목표 위치
	rot,         // 목표 회전
	false,                              // 즉시 스냅
	true,
	0.5f,                            // 텔레포트하지 않음
	false,
	EMoveComponentAction::Type::Move,
	LatentInfo
		);

	// 저항 애니메이션 재생 (블루프린트에서 설정된 ResistanceMontage)
	if ( CharacterAnimInstance )
	{
		CharacterAnimInstance->PlayResistanceMontage();
	}

	// QTE UI 표시
	StartQTEGrabEvent();
}

void AJPlayer::StopGrabbedState(bool bSuccess)
{
	bIsGrabbed = false;

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