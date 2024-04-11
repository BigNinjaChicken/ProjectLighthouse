// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "MainGameState.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTLIGHTHOUSE_API AMainGameState : public AGameStateBase
{
    GENERATED_BODY()

public:
    virtual void BeginPlay() override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game")
        TSoftObjectPtr<UWorld> GameCompleteLevel;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game")
        float TimeUntilNextLevel = 60.0f;

private:
    FTimerHandle TimerHandle;
    void StartNextLevel();
};
