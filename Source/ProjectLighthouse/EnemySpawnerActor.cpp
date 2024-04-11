// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemySpawnerActor.h"

// Sets default values
AEnemySpawnerActor::AEnemySpawnerActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AEnemySpawnerActor::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AEnemySpawnerActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AEnemySpawnerActor::SpawnEnemy(TSubclassOf<AActor> EnemyClass)
{
	FActorSpawnParameters ActorSpawnParameters;
	ActorSpawnParameters.Owner = this;
	AActor* SpawnedEnemyActor = GetWorld()->SpawnActor<AActor>(EnemyClass, GetActorTransform(), ActorSpawnParameters);

	bSpawnerTaken = true;
}

