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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Stats")
    float EnemyHealth;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Stats")
    float EnemyWalkSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Stats")
    float EnemyRunSpeed;

    FVector EnemyDirection;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Attack")
    float EnemyNoticeRange;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Attack")
    float EnemyDamage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Attack")
    class AActor* target;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "FSM")
    UKEnemyFSM* FSMComponent;

	UFUNCTION(BlueprintCallable, Category = "Enemy Actions")
    virtual void EnemyIDLE();

	UFUNCTION(BlueprintCallable, Category = "Enemy Actions")
    virtual void EnemyMove();

	UFUNCTION(BlueprintCallable, Category = "Enemy Actions")
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	UFUNCTION(BlueprintCallable, Category = "Enemy Actions")
    virtual void EnemyAttack();

	UFUNCTION(BlueprintCallable, Category = "Enemy Actions")
    virtual void EnemyDead();

};
