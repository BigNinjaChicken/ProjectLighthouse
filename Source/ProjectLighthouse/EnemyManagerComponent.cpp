// Fill out your copyright notice in the Description page of Project Settings.

#include "EnemyManagerComponent.h"
#include "Kismet/GameplayStatics.h"
#include "EnemySpawnerActor.h"

UEnemyManagerComponent::UEnemyManagerComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}


void UEnemyManagerComponent::BeginPlay()
{
	Super::BeginPlay();

	TArray<AActor*> OutActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEnemySpawnerActor::StaticClass(), OutActors);

	for (AActor* OutActor : OutActors) {
		SpawnLocations.Add(Cast<AEnemySpawnerActor>(OutActor));
	}

	CurrentSpawnRate = InitialSpawnRate;
	GetWorld()->GetTimerManager().SetTimer(SpawnTimerHandle, this, &UEnemyManagerComponent::StartSpawning, SpawnDelay, false);
	GetWorld()->GetTimerManager().SetTimer(SpawnRateIncreaseTimerHandle, this, &UEnemyManagerComponent::IncreaseSpawnRate, SpawnRateIncreaseInterval, true);
}

void UEnemyManagerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UEnemyManagerComponent::StartSpawning()
{
    GetWorld()->GetTimerManager().SetTimer(SpawnTimerHandle, this, &UEnemyManagerComponent::SpawnEnemy, CurrentSpawnRate, true);
}

void UEnemyManagerComponent::SpawnEnemy()
{
    TArray<AEnemySpawnerActor*> AvailableSpawners;
    for (AEnemySpawnerActor* Spawner : SpawnLocations)
    {
        if (!Spawner->bSpawnerTaken)
        {
            AvailableSpawners.Add(Spawner);
        }
    }

    if (AvailableSpawners.Num() > 0)
    {
        int32 RandomIndex = FMath::RandRange(0, AvailableSpawners.Num() - 1);
        AEnemySpawnerActor* SelectedSpawner = AvailableSpawners[RandomIndex];
        SelectedSpawner->SpawnEnemy(WaterEnemyCharacterClass);
        SelectedSpawner->bSpawnerTaken = true;
    }
}

void UEnemyManagerComponent::IncreaseSpawnRate()
{
    CurrentSpawnRate = FMath::Max(CurrentSpawnRate - SpawnRateIncreaseAmount, 2.0f); // Limit minimum spawn rate to 2 seconds
    GetWorld()->GetTimerManager().ClearTimer(SpawnTimerHandle);
    GetWorld()->GetTimerManager().SetTimer(SpawnTimerHandle, this, &UEnemyManagerComponent::SpawnEnemy, CurrentSpawnRate, true);
}