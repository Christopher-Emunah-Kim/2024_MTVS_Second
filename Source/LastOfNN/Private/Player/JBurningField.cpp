// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/JBurningField.h"
#include "Particles/ParticleSystemComponent.h"
#include "Components/BoxComponent.h"
#include "Kismet/KismetArrayLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/DamageEvents.h"
#include "Perception/AISense_Hearing.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "Enemy/KBaseEnemy.h"

// Sets default values
AJBurningField::AJBurningField()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);
	FireParticle = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("FireParticle"));
	FireParticle->SetupAttachment(GetRootComponent());
	Box = CreateDefaultSubobject<UBoxComponent>(TEXT("Box"));
	Box->SetupAttachment(GetRootComponent());
	Box->SetBoxExtent(FVector(50, 50, 20));

	// AI Perception Stimuli Source Component 생성 및 초기화
	PerceptionStimuliSource = CreateDefaultSubobject<UAIPerceptionStimuliSourceComponent>(TEXT("PerceptionStimuliSource"));
	PerceptionStimuliSource->RegisterForSense(TSubclassOf<UAISense_Hearing>());

	// 팀 타입 설정 (플레이어는 적)
	TeamType = ETeamType::ENEMY;
}

ETeamType AJBurningField::GetTeamType() const
{
	return TeamType;
}

// Called when the game starts or when spawned
void AJBurningField::BeginPlay()
{
	// 소리 발생 소스로 등록
	PerceptionStimuliSource->RegisterWithPerceptionSystem();

	MakeSound();

	SetLifeSpan(5);
	//오버랩되게 함
	Box->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);

	Super::BeginPlay();
	//던졌을때 그 위에 있는애들
	Box->GetOverlappingActors(HitActors);
	//불필드 안에 들어온 애들 배열 만들어서
	Box->OnComponentBeginOverlap.AddDynamic(this, &AJBurningField::BeginOverlap);
	//불필드 밖으로 나가면 배열에서 빼주고
	Box->OnComponentEndOverlap.AddDynamic(this, &AJBurningField::EndOverlap);
	//배열 안에 있는 액터들 데미지 가함
	if ( HitActors.Num() != 0 )
	{
		GetWorld()->GetTimerManager().SetTimer(DamageTimerHandle, this, &AJBurningField::DamageTick, DamageInterval, true);

	}
}

void AJBurningField::DamageTick()
{
	UE_LOG(LogTemp, Log, TEXT("DamageTick called. Number of actors in HitActors: %d"), HitActors.Num());
	for ( AActor* Actor : HitActors )
	{
		if ( Actor )
		{
			FPointDamageEvent DamageEvent(FireDamage, FHitResult(), GetActorForwardVector(), nullptr);
			AController* ActorController = nullptr;
			//불 필드 위에 있는 애들 컨트롤러 얻어와서 데미지 주기
			APawn* ActorPawn = Cast<APawn>(Actor);
			if ( ActorPawn )
			{
				ActorController = ActorPawn->GetController();
			}
			Actor->TakeDamage(FireDamage, DamageEvent, ActorController, this);

			UE_LOG(LogTemp, Log, TEXT("Applied %f damage to %s"), FireDamage, *Actor->GetName());
		}
	}
}

void AJBurningField::BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if ( OtherActor && OtherActor != this )
	{
		HitActors.AddUnique(OtherActor); //데미지를 입을 액터들
		UE_LOG(LogTemp, Log, TEXT("Applied"));
	}
}

void AJBurningField::EndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	//불필드 밖으로 나가면 데미지를 안 입게 하고 싶다.
	if ( OtherActor && OtherActor != this )
	{
		int32 Index = HitActors.Find(OtherActor);
		if ( Index != INDEX_NONE )
		{
			HitActors.RemoveAt(Index); 
		}
	}

}
// Called every frame
void AJBurningField::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AJBurningField::MakeSound()
{
	// 소리 자극 발생시키기
	FVector NoiseLocation = GetActorLocation();
	float Loudness = 102.f;  // 소리 강도 (예시 값)
	UGameplayStatics::PlaySoundAtLocation(this, LandingSound, NoiseLocation); // 착지 소리 재생
	UAISense_Hearing::ReportNoiseEvent(GetWorld(), NoiseLocation, Loudness, this, 10.f, TEXT("ObjectLanding"));
	// 로그를 통해 이벤트 발생 확인
	UE_LOG(LogTemp, Warning, TEXT("Noise event reported at location: %s with loudness: %f"), *NoiseLocation.ToString(), Loudness);
}
