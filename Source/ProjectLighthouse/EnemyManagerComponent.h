// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EnemySpawnerActor.h"
#include "WaterEnemyCharacter.h"
#include "EnemyManagerComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTLIGHTHOUSE_API UEnemyManagerComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    // Sets default values for this component's properties
    UEnemyManagerComponent();

protected:
    // Called when the game starts
    virtual void BeginPlay() override;

public:
    // Called every frame
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy")
        TSubclassOf<AActor> WaterEnemyCharacterClass;

    TArray<AEnemySpawnerActor*> SpawnLocations;

private:
    // Delay before spawning starts (in seconds)
    UPROPERTY(EditAnywhere, Category = "Spawning")
        float SpawnDelay = 5.0f;

    // Initial spawn rate (in seconds)
    UPROPERTY(EditAnywhere, Category = "Spawning")
        float InitialSpawnRate = 10.0f;

    // Spawn rate increase interval (in seconds)
    UPROPERTY(EditAnywhere, Category = "Spawning")
        float SpawnRateIncreaseInterval = 30.0f;

    // Spawn rate increase amount
    UPROPERTY(EditAnywhere, Category = "Spawning")
        float SpawnRateIncreaseAmount = 2.0f;

    FTimerHandle SpawnTimerHandle;
    FTimerHandle SpawnRateIncreaseTimerHandle;

    float CurrentSpawnRate;
    float LastSpawnTime;

    void StartSpawning();
    void SpawnEnemy();
    void IncreaseSpawnRate();
};
