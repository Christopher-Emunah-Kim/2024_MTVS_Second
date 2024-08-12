// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Enemy/KEnemyFSM.h"
#include "KBaseEnemy.generated.h"

//팀타입설정 Enum
UENUM(BlueprintType)
enum class ETeamType : uint8
{
	ENEMY UMETA(DisplayName = "Enemy"),
	FRIENDLY UMETA(DisplayName = "Friendly"),
	NEUTRAL UMETA(DisplayName = "Neutral")
};

UCLASS()
class LASTOFNN_API AKBaseEnemy : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AKBaseEnemy();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//FSM 인스턴스
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "FSM")
    UKEnemyFSM* FSMComponent;

	//AI Perception 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI", meta = (AllowPrivateAccess = "true"))
	class UAIPerceptionComponent* AIPerceptionComp;

	//데미지 처리 위한 손 충돌체 형성
	UPROPERTY(EditAnywhere)
	class USphereComponent* RightAttackSphere;	
	UPROPERTY(EditAnywhere)
	class USphereComponent* LeftAttackSphere;

	//플레이어 Target 정보 인스턴스
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FSM")
    class AJPlayer* target;

	//사용중인 에니메이션 BP 인스턴스
	UPROPERTY()
	class UKEnemyAnim* anim;

	//Enemy를 소유한 AI Controller 설정
	UPROPERTY()
	class AAIController* ai;

	//팀타입 추가 인스턴스
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Team")
	ETeamType TeamType;


#pragma region virtual function with properties
	
	//기본 가상함수 및 필요 속성
	
	//State Setting과 Animation동기화 처리
	virtual void EnemySetState(EEnemyState newstate);

	//=======================================================================================
    //**대기상태처리함수
	virtual void EnemyIDLE();
	//대기시간
	UPROPERTY(EditDefaultsOnly, Category = "FSM")
	float IdleDelayTime = 2.0f;
	//경과시간
	float CurrentTime = 0;

	//=======================================================================================
	//** AI Perception
	
	//주의도 임계값
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AIPerception")
	float AttentionThreshold = 100.0f;

	//어그로 수치 관리
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AIPerception")
	float EnemyAttentionDegree;

	//AI 감지 감각에 따른 처리함수
	UFUNCTION()
	virtual void OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);

	//소음 감지거리
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AIPerception")
	float EnemySoundDetectionRadius;
	//소음 발생시 이동거리
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AIPerception")
	float EnemyMoveDistanceOnSound;
	//소음 발생위치
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AIPerception")
	FVector SoundLocation;
	//소음에 의한 위치이동여부
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AIPerception")
	bool bShouldMoveToSound;
	//AI Hearing 정보 Config
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AIPerception")
	class UAISenseConfig_Hearing* HearingConfig;
	//소리 감지 처리함수
	UFUNCTION()
	virtual void OnEnemyNoiseHeard(AActor* Actor, FAIStimulus Stimulus);

	//AI Sight 정보 Config
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AIPerception")
	class UAISenseConfig_Sight* SightConfig;
	//시야 감지 처리함수
	UFUNCTION()
	virtual void OnEnemySightVision(const TArray<AActor*>& UpdatedActors);
	//시야 노출시간 추적타이머
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AIPerception")
	FTimerHandle EnemySeePlayerTimerHandle;
	//시야 감지에 의한 위치이동여부
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AIPerception")
	bool bShoutMoveToSight = false;
	//시야 감지 위치
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AIPerception")
	FVector ShownLocation;

	//=======================================================================================
	// 
	//**이동상태처리함수
    virtual void EnemyMove();
	//걷기속도
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FSM")
    float EnemyWalkSpeed;
	//뛰기속도
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FSM")
    float EnemyRunSpeed;
	
	//방향
    FVector EnemyDirection;

	//길찾기 수행시 랜덤 위치
	FVector EnemyRandomPos;
	//랜덤위치가져오기 함수
	bool GetRandomPositionInNavMesh(FVector centerLocation, float radius, FVector& dest);

	//=======================================================================================
	
	//**공격상태처리함수
    virtual void EnemyAttack();
	UFUNCTION()
	virtual void EnemyOverlapDamage(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	//Normla Grab / Boss Grenade 특수공격 이벤트함수
	virtual void EnemySpecialAttack();
	// QTE 이벤트 진행 중 다른 Enemy들을 IDLE 상태로 유지
	virtual void SetAllEnemiesToIdle();

	//Enemy탐지범위
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FSM")
    float EnemyNoticeRange;
	//Enemy데미지
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FSM")
    float EnemyAttackDamage;
	//Enemy공격범위
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FSM")
    float EnemyAttackRange;
	//Enemy공격대기시간
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FSM")
    float EnemyAttackDelayTime;
	//EnemySpecialAttack데미지
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FSM")
    float EnemySpecialAttackDamage;
	//EnemySpecialAttack범위
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FSM")
    float EnemySpecialAttackRange;
	// QTE 이벤트 시작 시 Player를 잡았는지 확인하기 위한 변수
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FSM")
	bool bIsPlayerGrabbed = false;

	//=======================================================================================
	//피격알림이벤트함수
	float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	virtual void OnEnemyDamageProcess(float damage);
	//Enemy HP
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FSM")
    int32 EnemyHP;

	//배트 피격 애니메이션 재생을 위한 인덱스
	int32 BatIndex = 2;

	//**피격상태처리함수
	virtual void EnemyTakeDamage();
	//피격 후 IDLE전환 대기시간
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FSM")
    float EnemyTDamageDelayTime = 1.5f;

	//암살상태처리함수
	virtual void EnemyExecuted();

	//=======================================================================================
	//**죽음상태처리함수
    virtual void EnemyDead();
	//죽음 후 메시 내려가는 속도
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FSM")
    float DieDownfallSpeed = 50.0f;



#pragma endregion
};
