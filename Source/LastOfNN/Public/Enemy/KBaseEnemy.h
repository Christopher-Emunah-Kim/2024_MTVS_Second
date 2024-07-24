// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Enemy/KEnemyFSM.h"
#include "KBaseEnemy.generated.h"

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

	// Called to bind functionality to input
	//virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	//Enemy Status���� ���� ����
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Stats")
    float EnemyHealth;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Stats")
    float EnemyWalkSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Stats")
    float EnemyRunSpeed;

	//Enemy �̵�, ���� ��� Vector����
    FVector EnemyDirection;

	//Enemy ���� ��� ����
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Attack")
    float EnemyNoticeRange;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Attack")
    float EnemyDamage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Attack")
    class AJPlayer* target;

	//FSM �ν��Ͻ�
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "FSM")
    UKEnemyFSM* FSMComponent;

	//Base�⺻ �����Լ�
    virtual void EnemyIDLE();

    virtual void EnemyMove();

	virtual float TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

    virtual void EnemyAttack();

    virtual void EnemyDead();

};
