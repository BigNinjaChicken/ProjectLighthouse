// Copyright Epic Games, Inc. All Rights Reserved.

#include "ProjectLighthouseGameMode.h"
#include "ProjectLighthouseCharacter.h"
#include "UObject/ConstructorHelpers.h"

AProjectLighthouseGameMode::AProjectLighthouseGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
