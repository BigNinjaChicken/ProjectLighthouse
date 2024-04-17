// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/SpotLightComponent.h"
#include "Engine/TimerHandle.h"
#include "Components/SplineComponent.h"
#include "Components/SphereComponent.h"
#include "LighthousePawn.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
class USpotLightComponent;
class USphereComponent;
class APlayerCharacter;
struct FInputActionValue;

UCLASS()
class PROJECTLIGHTHOUSE_API ALighthousePawn : public APawn
{
	GENERATED_BODY()

public:
	// Mesh component for the pawn
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh")
		UStaticMeshComponent* Mesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh")
		UStaticMeshComponent* LightMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh")
		USphereComponent* SphereRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh")
		USpotLightComponent* SpotLightComponent;

	// Camera component for the pawn
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
		UCameraComponent* Camera;

	// Spring arm component for the camera
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
		USpringArmComponent* SpringArm;

	// Floating pawn movement component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
		UFloatingPawnMovement* FloatingMovement;

	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		UInputAction* ShootAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		UInputAction* InteractAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		USplineComponent* Spline;

	// Sets default values for this pawn's properties
	ALighthousePawn();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void Move(const FInputActionValue& Value);

	void Shoot(const FInputActionValue& Value);

	void TurnOnLight();

	void Interact(const FInputActionValue& Value);

	void MoveCameraAlongSpline();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lighthouse")
		float RotationSpeed = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lighthouse")
		float FlashCount = 2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lighthouse")
		float MaxFlashCount = 4.2;

	FTimerHandle LightTimerHandle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
		float LightFlashDuration = 1.2f; // Adjust the duration as needed

	UPROPERTY(BlueprintReadOnly, Category = "Lighthouse Pawn")
		float ElapsedTime = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Window Actors")
		TArray<AActor*> WindowActors;

	UPROPERTY(BlueprintReadOnly, Category = "Window Actors")
		APlayerCharacter* PlayerCharacter;

	UPROPERTY()
		FTimerHandle MovementTimerHandle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
		float LightHouseTransitionDuration = 3.0f;

	FRotator StartingCameraRotation;

	FVector StartingCameraLocation;

	bool bDisableMovement = false;

	UPROPERTY(EditAnywhere, Category = "Raycast")
		float RaycastRadius = 10.0f;

	UPROPERTY(EditAnywhere, Category = "Raycast")
		float RaycastRange = 1000.0f;

	UPROPERTY(EditAnywhere, Category = "Rotation Limits")
		float MinAngle = -45.0f; // Minimum angle in degrees

	UPROPERTY(EditAnywhere, Category = "Rotation Limits")
		float MaxAngle = 45.0f; // Maximum angle in degrees
};
