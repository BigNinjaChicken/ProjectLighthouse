// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Navigation/PathFollowingComponent.h"
#include "Engine/World.h"
#include "WaterEnemyCharacter.generated.h"

class UPathFollowingComponent;

UCLASS()
class PROJECTLIGHTHOUSE_API AWaterEnemyCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AWaterEnemyCharacter();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Game")
		UPathFollowingComponent* PathFollowingComponent;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

    UPROPERTY(EditAnywhere, Category = "Invasion")
    float DelayBeforeInvading = 5.0f;

    UPROPERTY(EditAnywhere, Category = "Invasion")
    float KillRange = 100.0f;

    FTimerHandle KillPlayerTimerHandle;

    void InvadePlayer();
    void CheckPlayerRange();
    void KillPlayer();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Game")
		TSoftObjectPtr<UWorld> GameOverWorld;
};
