// Fill out your copyright notice in the Description page of Project Settings.


#include "MainGameState.h"
#include "LighthouseGameInstance.h"
#include "Kismet/GameplayStatics.h"

void AMainGameState::BeginPlay()
{
    Super::BeginPlay();

    // Set a timer to call StartNextLevel() after 5 seconds
    GetWorldTimerManager().SetTimer(TimerHandle, this, &AMainGameState::StartNextLevel, TimeUntilNextLevel, false);
}

void AMainGameState::StartNextLevel()
{
    if (ULighthouseGameInstance* GI = GetGameInstance<ULighthouseGameInstance>()) {
        GI->CurrentLevelIndex++;
        
        if (GI->GameLevels.Num() <= GI->CurrentLevelIndex) {
            UGameplayStatics::OpenLevelBySoftObjectPtr(GetWorld(), GameCompleteLevel);
        }
        else {
            UGameplayStatics::OpenLevelBySoftObjectPtr(GetWorld(), GI->GameLevels[GI->CurrentLevelIndex]);
        }
    }
}