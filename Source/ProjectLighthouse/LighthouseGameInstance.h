// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Engine/Level.h"
#include "LighthouseGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTLIGHTHOUSE_API ULighthouseGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
		TArray<TSoftObjectPtr<UWorld>> GameLevels;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
		int CurrentLevelIndex = 0;
};