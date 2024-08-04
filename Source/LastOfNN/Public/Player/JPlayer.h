// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "Enemy/KBaseEnemy.h"
#include "CharacterTypes.h"
#include "JPlayer.generated.h"


class UCameraComponent;
class USpringArmComponent;
class UInputAction;
class UInputMappingContext;
class UPlayerLockOn;
class APlayerGun;
class UAnimMontage;

UCLASS()
class LASTOFNN_API AJPlayer : public ACharacter
{
	GENERATED_BODY()

public:

	// AI Perception Stimuli Source Component
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI", meta = (AllowPrivateAccess = "true"))
    class UAIPerceptionStimuliSourceComponent* PerceptionStimuliSource;

    // 팀 타입
    ETeamType TeamType;
    
    // 팀 타입 반환 함수
    ETeamType GetTeamType() const;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	ECharacterState CharaterState = ECharacterState::ECS_UnGrabbed;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	ECharacterEquipState CharacterEquipState = ECharacterEquipState::ECES_UnEquipped;

	// Sets default values for this character's properties
	AJPlayer();

	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	//이동 방향

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UCameraComponent* CameraComp;	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USpringArmComponent* SpringArmComp;
	UPROPERTY(EditAnywhere)
	class UBoxComponent* Box;
	UPROPERTY(EditAnywhere)
	class USphereComponent* RightAttackSphere;	
	UPROPERTY(EditAnywhere)
	class USphereComponent* LeftAttackSphere;


	UPROPERTY(EditAnywhere, Category = "Input")
	UInputMappingContext* IMC_Joel;
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* IA_Move;

	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* IA_Look;	
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* IA_Fire;
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* IA_Zoom;	
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* IA_Run;	
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* IA_Crouch;
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* IA_Grab;	
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* IA_TakeDown;
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* IA_EquipGun;	
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* IA_EquipThrowWeapon;	
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* IA_UnEquipped;	
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* IA_BatEquipped;

	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void Fire(const FInputActionValue& Value);
	void Zoom(const FInputActionValue& Value);
	void ZoomOut(const FInputActionValue& Value);
	void Run(const FInputActionValue& Value);

	void Crouching(const FInputActionValue& Value);
	//토글 체크 변수
	bool bCrouched = false;

	//암살 함수
	//플레이어 컨트롤러
	APlayerController* PlayerController;
	
	void NewTakeDown(const FInputActionValue& Value);
	//암살 후에
	void AfterTakeDown();
	FTimerHandle TakeDownTimer;
	FTimerHandle SetCameraBackTimer;

	void SetCameraBack();
	void SetCameraBoomToCharacter(bool bSetCameraBoom);
	//상태변경함수
	void SetStateEquipGun();
	void SetStateEquipThrowWeapon();
	void SetStateUnEquipped();
	void SetStateBatEquipped();
	UCameraComponent* GetCamera();

	//총 기
	UPROPERTY()
	APlayerGun* Gun;

	UPlayerLockOn* LockOnComp;
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class APlayerGun> GunClass;

	//배트
	UPROPERTY()
	class AJPlayerBat* Bat;
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class AJPlayerBat> BatClass;

	//이동 관련
	FVector direction;
	UCharacterMovementComponent* CharacterMovement;
	bool bIsRunning = false;

	//연속공격
	class UJCharacterAnimInstance* CharacterAnimInstance;
	bool bIsComboInputOn;
	bool bIsAttacking;
	bool bCanNextCombo;
	float CurrentCombo;
	float MaxCombo = 4;

	UPROPERTY(EditAnywhere)
	UAnimMontage* AttackMontage;

	void PostInitializeComponents() override;
	
	//데미지를 받을 액터들
	TArray<AActor*> HitActors;
	UFUNCTION()
	void OverlapDamage(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION(BlueprintCallable)
	ECharacterState GetCharaterState() const;
	UFUNCTION(BlueprintCallable)
	ECharacterEquipState GetCharacterEquipState() const;


	//Grab QTE이벤트 사용내용
	bool bIsGrabbed = false;
	int32 RequiredKeyPresses = 5; // 플레이어가 QTE에서 벗어나기 위해 필요한 E키 입력 횟수
	int32 CurrentKeyPresses = 0;

	// 현재 Player를 잡고 있는 Enemy의 참조
	class AKBaseEnemy* GrabbedEnemy;

	void StartGrabbedState(class AKNormalZombieEnemy* Enemy); // Grab 상태 시작 함수
	void StopGrabbedState(bool bSuccess);  // Grab 상태 종료 함수, 성공 여부에 따라 다르게 처리
	void HandleQTEInput();    // QTE 입력 처리 함수

	// QTE UI 시작 및 종료 함수
	void StartQTEGrabEvent();
	void StopQTEGrabEvent(bool bSuccess);

	UPROPERTY(EditDefaultsOnly, Category=UI)
	TSubclassOf<class UKEnemyQTEWidget> QTEUIFactory;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite)
	//class UUserWidget* QTEWidget;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UKEnemyQTEWidget* _QTEUI;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bEscapeSuccess;

	// 현재 Player를 잡고 있는 Enemy의 참조 반환 함수
	AKBaseEnemy* GetGrabbedEnemy() const { return GrabbedEnemy; }

	
	//위젯 바인딩 키 진행도
	//UFUNCTION(BlueprintCallable)
	float GetKeyProcessPercent();

	//암살박스 닿으면
	UFUNCTION(BlueprintCallable)
	void ReadyToExcecute(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	class AKNormalZombieEnemy* FSMOwner;
	//암살가능
	class AKNormalZombieEnemy* ExecutionTarget;
	bool bCanExecute;
	bool bIsExecuting = false;
	bool GetIsExecuting();

	class UKEnemyFSM* EnemyFSM;
	class UKEnemyFSM* BossEnemyFSM;

	class AKBossZombieEnemy* Boss;
	bool GetIsGrabbed();
	void EnemyIsDead();

	//체력
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HP")
	float HealthPoints;
	float MAXHP = 100;

	//향상된 입력
	class UEnhancedInputLocalPlayerSubsystem* Subsystem;

	//데미지 받을때
	float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
	
	//카메라 줌
	float TargetFOV = 90;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted);
	UFUNCTION()
	void AttackStartComboState();
	UFUNCTION()
	void AttackEndComboState();

};
