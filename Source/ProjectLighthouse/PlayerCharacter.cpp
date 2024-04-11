// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "EnhancedInputSubsystemInterface.h"
#include "Kismet/GameplayStatics.h"
#include "LighthousePawn.h"
#include "InteractableActor.h"
#include "Components/SplineComponent.h"
#include "GameFramework/PlayerController.h"

// Sets default values
APlayerCharacter::APlayerCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	// Create a first-person camera
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	Camera->SetupAttachment(GetCapsuleComponent());
	Camera->bUsePawnControlRotation = true;

	Spline = CreateDefaultSubobject<USplineComponent>(TEXT("Spline"));
}

void APlayerCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	// GetController()->SetControlRotation(StartingControlRotation);
}

// Called when the game starts or when spawned
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	// Find all actors with the tag "Window" and store them in the WindowActors array
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("Window"), WindowActors);

	// Find the actor with the parent class "LighthousePawn"
	AActor* LighthousePawnActor = UGameplayStatics::GetActorOfClass(GetWorld(), ALighthousePawn::StaticClass());
	LighthousePawn = Cast<ALighthousePawn>(LighthousePawnActor);
}

// Called every frame
void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
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
		EnhancedInputComponent->BindAction(OptionsAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Options);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Move);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Look);

		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Interact);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void APlayerCharacter::Options(const FInputActionValue& Value)
{
	if (OptionsWidget)
	{
		OptionsWidget->RemoveFromParent();
		if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
		{
			FInputModeGameOnly InputModeGameOnly;
			PlayerController->SetInputMode(InputModeGameOnly);
			PlayerController->bShowMouseCursor = false;
		}
	}
	else {
		OptionsWidget = CreateWidget<UUserWidget>(GetWorld(), OptionsWidgetClass);
		if (OptionsWidget)
		{
			OptionsWidget->AddToViewport();
			if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
			{
				FInputModeUIOnly InputModeUIOnly;
				PlayerController->SetInputMode(InputModeUIOnly);
				PlayerController->bShowMouseCursor = true;
			}
		}
	}
}

void APlayerCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void APlayerCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (!bIsRotatingCrank) {
		if (Controller != nullptr) {
			// add yaw and pitch input to controller
			AddControllerYawInput(LookAxisVector.X);
			AddControllerPitchInput(LookAxisVector.Y);
		}
	}
	else {
		// Check if the mouse is moving in a circular motion
		if (PrevLookAxisVector.SizeSquared() > 0.0f && LookAxisVector.SizeSquared() > 0.0f) {
			float CrossProduct = PrevLookAxisVector.X * LookAxisVector.Y - PrevLookAxisVector.Y * LookAxisVector.X;
			float RadiusSquared = PrevLookAxisVector.SizeSquared() * LookAxisVector.SizeSquared();

			if (CrossProduct > 0.0f && RadiusSquared > MinimumRadius * MinimumRadius) {
				// Clockwise rotation within the minimum radius
				double CurrentTime = GetWorld()->GetTimeSeconds();
				double TimeSinceLastRotation = CurrentTime - LastCrankRotationTime;

				if (TimeSinceLastRotation > 0.0) {
					CrankRotationSpeed = 1.0 / TimeSinceLastRotation;
					CrankRevolutions++;

					// Handle Lighthouse
					if (LighthousePawn->FlashCount <= LighthousePawn->MaxFlashCount) {
						LighthousePawn->FlashCount += LightIncreaseAmount;
					}

					LastCrankRotationTime = CurrentTime;
				}
			}
		}

		PrevLookAxisVector = LookAxisVector;
	}
}

void APlayerCharacter::Interact(const FInputActionValue& Value)
{
	if (bStopInteraction) {
		UE_LOG(LogTemp, Warning, TEXT("Stopped Interaction"));
		return;
	}

	if (bIsRotatingCrank) {
		GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);

		bIsRotatingCrank = false;
		return;
	}

	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (!PlayerController) {
		UE_LOG(LogTemp, Warning, TEXT("Null PlayerController"));
		return;
	}

	FHitResult HitResult;
	FVector Start = PlayerController->GetPawn()->GetActorLocation();
	FVector End = Start + PlayerController->GetControlRotation().Vector() * 300.0f; // Adjust the raycast length as needed

	// Log the start and end vectors of the raycast
	UE_LOG(LogTemp, Log, TEXT("Raycast Start: %s, End: %s"), *Start.ToString(), *End.ToString());

	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(this); // Ignore the player character

	if (GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, CollisionParams))
	{
		// Log a message indicating a hit was detected
		UE_LOG(LogTemp, Log, TEXT("Raycast hit: %s"), *HitResult.GetActor()->GetName());

		if (UPrimitiveComponent* HitComp = HitResult.GetComponent())
		{
			AInteractableActor* InteractableActor = Cast<AInteractableActor>(HitComp->GetOwner());
			if (InteractableActor)
			{
				UE_LOG(LogTemp, Log, TEXT("Interactable Actor: %s"), *InteractableActor->GetName());

				if (HitComp == InteractableActor->InteractSphereComponent)
				{
					// Log a message when the specific component is hit
					UE_LOG(LogTemp, Log, TEXT("InteractSphereComponent was hit."));

					if (InteractableActor->ActorHasTag(FName("Lighthouse"))) {
						CommandLightHouse();
						return;
					}

					if (InteractableActor->ActorHasTag(FName("Crank"))) {
						CommandCrank();
						return;
					}

					UE_LOG(LogTemp, Log, TEXT("No Valid Tag for interactable."));
				}
			}
		}
	}
	else
	{
		// Log when there's no hit detected
		UE_LOG(LogTemp, Warning, TEXT("No hit detected."));
	}

	// Optionally, visually debug the raycast in the editor
	DrawDebugLine(GetWorld(), Start, End, FColor::Green, false, 1, 0, 1);
}

void APlayerCharacter::CommandLightHouse()
{
	if (WindowActors.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("No WindowActors."));
		return;
	}

	if (!LighthousePawn)
	{
		UE_LOG(LogTemp, Warning, TEXT("No LighthousePawn."));
		return;
	}

	bStopInteraction = true;

	// Initialize with a maximum possible distance and a null actor
	float MinDistance = FLT_MAX;
	AActor* ClosestWindowActor = nullptr;

	// Find the closest window actor to the lighthouse camera
	for (AActor* WindowActor : WindowActors)
	{
		if (WindowActor)
		{
			float Distance = FVector::Dist(LighthousePawn->Camera->GetComponentLocation(), WindowActor->GetActorLocation());
			if (Distance < MinDistance)
			{
				MinDistance = Distance;
				ClosestWindowActor = WindowActor;
			}
		}
	}

	// Proceed only if a closest window actor was found
	if (ClosestWindowActor)
	{
		// Clear existing spline points
		Spline->ClearSplinePoints();

		// Add new spline points
		Spline->AddSplinePoint(Camera->GetComponentLocation(), ESplineCoordinateSpace::World);
		Spline->AddSplinePoint(ClosestWindowActor->GetActorLocation(), ESplineCoordinateSpace::World);
		Spline->AddSplinePoint(LighthousePawn->Camera->GetComponentLocation(), ESplineCoordinateSpace::World);

		// Disable player movement and camera control
		GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);
		GetController()->SetIgnoreLookInput(true);

		// Start the timer
		ElapsedTime = 0;
		StartingCameraRotation = Camera->GetComponentRotation();
		StartingCameraLocation = Camera->GetComponentLocation();
		GetWorld()->GetTimerManager().SetTimer(MovementTimerHandle, this, &APlayerCharacter::MoveCameraAlongSpline, GetWorld()->GetDeltaSeconds(), true);
	}
}


void APlayerCharacter::MoveCameraAlongSpline() {
	float Alpha = ElapsedTime / LightHouseTransitionDuration;
	if (Alpha >= 1.0f)
	{
		// Movement complete, stop the timer and re-enable player movement and camera control
		GetWorld()->GetTimerManager().ClearTimer(MovementTimerHandle);
		GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
		GetController()->SetIgnoreLookInput(false);
		Camera->bUsePawnControlRotation = true;

		APlayerController* PlayerController = Cast<APlayerController>(GetController());
		if (!PlayerController) {
			UE_LOG(LogTemp, Warning, TEXT("Null PlayerController"));
			return;
		}

		PlayerController->Possess(LighthousePawn);

		Camera->SetWorldLocation(StartingCameraLocation);
		Camera->SetWorldRotation(StartingCameraRotation);

		bStopInteraction = false;
		return;
	}

	FTransform Transform = Spline->GetTransformAtDistanceAlongSpline(Spline->GetSplineLength() * Alpha, ESplineCoordinateSpace::World);
	Camera->SetWorldLocation(Transform.GetLocation());

	StartingControlRotation = GetControlRotation();
	Camera->bUsePawnControlRotation = false;

	FRotator NewRotation = FMath::Lerp(StartingCameraRotation, LighthousePawn->Camera->GetComponentRotation(), Alpha);
	Camera->SetWorldRotation(NewRotation);

	ElapsedTime += GetWorld()->GetDeltaSeconds();
}

void APlayerCharacter::CommandCrank() {
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);
	bIsRotatingCrank = true;

	ElapsedTime = 0;
	CrankRotationSpeed = 0.0f;
	PrevLookAxisVector = FVector2D::ZeroVector;
	LastCrankRotationTime = 0.0;
	CrankRevolutions = 0;
	GetWorld()->GetTimerManager().SetTimer(MovementTimerHandle, this, &APlayerCharacter::MoveCrank, GetWorld()->GetDeltaSeconds(), true);
}

void APlayerCharacter::MoveCrank() 
{
	UE_LOG(LogTemp, Log, TEXT("FlashCount: %f"), LighthousePawn->FlashCount);

	ElapsedTime += GetWorld()->GetDeltaSeconds();
}