// Fill out your copyright notice in the Description page of Project Settings.


#include "MainMenuPlayerPawn.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "EnhancedInputSubsystemInterface.h"

// Sets default values
AMainMenuPlayerPawn::AMainMenuPlayerPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AMainMenuPlayerPawn::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMainMenuPlayerPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AMainMenuPlayerPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}

	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// Moving
		EnhancedInputComponent->BindAction(OptionsAction, ETriggerEvent::Triggered, this, &AMainMenuPlayerPawn::Options);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void AMainMenuPlayerPawn::Options(const FInputActionValue& Value)
{
	if (bOpenMenuIsOpen) SetOptionsMenuVisability(false);
}

void AMainMenuPlayerPawn::SetOptionsMenuVisability(bool bOpen)
{
	bOpenMenuIsOpen = bOpen;

	if (bOpen)
	{
		OptionsWidget = CreateWidget<UUserWidget>(GetWorld(), OptionsWidgetClass);
		if (OptionsWidget)
		{
			OptionsWidget->AddToViewport();
		}
	}
	else
	{
		// Remove the Options widget from the viewport
		if (OptionsWidget)
		{
			OptionsWidget->RemoveFromParent();
		}
	}
}