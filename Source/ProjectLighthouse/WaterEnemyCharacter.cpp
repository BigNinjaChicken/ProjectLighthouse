// Fill out your copyright notice in the Description page of Project Settings.


#include "WaterEnemyCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "Navigation/PathFollowingComponent.h"
#include "AIController.h"

// Sets default values
AWaterEnemyCharacter::AWaterEnemyCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    PathFollowingComponent = CreateDefaultSubobject<UPathFollowingComponent>(TEXT("PathFollowingComponent"));
}

// Called when the game starts or when spawned
void AWaterEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();

	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Flying);

	// Start a timer to call InvadePlayer after a delay
	FTimerHandle TimerHandle;
	GetWorldTimerManager().SetTimer(TimerHandle, this, &AWaterEnemyCharacter::InvadePlayer, DelayBeforeInvading, false);
}

// Called every frame
void AWaterEnemyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AWaterEnemyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AWaterEnemyCharacter::InvadePlayer()
{
    // Find the closest actor with the tag "Railing"
    TArray<AActor*> FoundActors;
    UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("Railing"), FoundActors);

    AActor* ClosestRailing = nullptr;
    float MinDistance = MAX_FLT;

    for (AActor* Actor : FoundActors)
    {
        float Distance = FVector::Dist(GetActorLocation(), Actor->GetActorLocation());
        if (Distance < MinDistance)
        {
            MinDistance = Distance;
            ClosestRailing = Actor;
        }
    }

    if (ClosestRailing)
    {
        // Teleport to the closest railing
        SetActorLocation(ClosestRailing->GetActorLocation());

        // Start walking towards the player
        ACharacter* PlayerCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
        if (PlayerCharacter)
        {
            GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
            if (PathFollowingComponent)
            {
                if (AAIController* AIController = Cast<AAIController>(GetController())) {
                    AIController->MoveToActor(PlayerCharacter);
                }
            }

            GetWorldTimerManager().SetTimer(KillPlayerTimerHandle, this, &AWaterEnemyCharacter::CheckPlayerRange, 0.1f, true);
        }
    }
}

void AWaterEnemyCharacter::CheckPlayerRange()
{
    ACharacter* PlayerCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
    if (PlayerCharacter)
    {
        float Distance = FVector::Dist(GetActorLocation(), PlayerCharacter->GetActorLocation());
        if (Distance <= KillRange)
        {
            GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_NavWalking);
            KillPlayer();
            GetWorldTimerManager().ClearTimer(KillPlayerTimerHandle);
        }
    }
}

void AWaterEnemyCharacter::KillPlayer()
{
    UGameplayStatics::OpenLevelBySoftObjectPtr(GetWorld(), GameOverWorld);
}