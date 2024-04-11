// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Blueprint/UserWidget.h"
#include "InteractionComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/SplineComponent.h"
#include "LighthousePawn.h"
#include "CrankActor.h"
#include "PlayerCharacter.generated.h"

class UInputMappingContext;
class UInputAction;
class UInteractionComponent;
class UCameraComponent;
class ALighthousePawn;
class USplineComponent;
struct FInputActionValue;

UCLASS()
class PROJECTLIGHTHOUSE_API APlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	APlayerCharacter();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		UInputMappingContext* DefaultMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		UInputAction* OptionsAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		UInputAction* LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		UInputAction* InteractAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		UInteractionComponent* InteractionComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		USplineComponent* Spline;

	virtual void PossessedBy(AController* NewController) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintCallable)
		void Options(const FInputActionValue& Value);

	UFUNCTION()
		void Move(const FInputActionValue& Value);

	UFUNCTION()
		void Look(const FInputActionValue& Value);

	UFUNCTION()
		void Interact(const FInputActionValue& Value);

	UFUNCTION()
		void CommandLightHouse();

	UFUNCTION()
		void MoveCameraAlongSpline();

	UFUNCTION()
		void CommandCrank();

	UFUNCTION()
		void MoveCrank();

	UPROPERTY()
		FTimerHandle MovementTimerHandle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
		float LightHouseTransitionDuration = 3.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Lighthouse Pawn")
		float ElapsedTime = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
		TSubclassOf<UUserWidget> OptionsWidgetClass;

	UPROPERTY()
		UUserWidget* OptionsWidget;

	// Array to store window actors
	UPROPERTY(BlueprintReadOnly, Category = "Window Actors")
		TArray<AActor*> WindowActors;

	// Lighthouse pawn
	UPROPERTY(BlueprintReadOnly, Category = "Lighthouse Pawn")
		ALighthousePawn* LighthousePawn;

	FRotator StartingCameraRotation;

	UPROPERTY(BlueprintReadOnly, Category = "Lighthouse Pawn")
		ACrankActor* CrankActor;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		UCameraComponent* Camera;

	bool bIsRotatingCrank = false;
	float CrankRotationSpeed = 0.0f;
	FVector2D PrevLookAxisVector = FVector2D::ZeroVector;
	double LastCrankRotationTime = 0.0;
	int CrankRevolutions = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crank")
	float MinimumRadius = 60.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crank")
	float LightIncreaseAmount = 0.02f;

	FVector StartingCameraLocation;
	
	FRotator StartingControlRotation;

	bool bStopInteraction = false;
};
