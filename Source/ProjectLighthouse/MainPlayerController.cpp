// Fill out your copyright notice in the Description page of Project Settings.


#include "MainPlayerController.h"
#include "PlayerCharacter.h"

void AMainPlayerController::OnPossess(APawn* aPawn)
{
	Super::OnPossess(aPawn);
	
	if (APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(aPawn)) {
		SetControlRotation(PlayerCharacter->StartingControlRotation);
	}
}
