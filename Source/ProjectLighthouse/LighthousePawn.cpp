// Fill out your copyright notice in the Description page of Project Settings.


#include "LighthousePawn.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "TimerManager.h"
#include "Components/SplineComponent.h"
#include "Kismet/GameplayStatics.h"
#include "PlayerCharacter.h"
#include "WaterEnemyCharacter.h"
#include "CollisionShape.h"
#include "Engine/HitResult.h"
#include "EnemySpawnerActor.h"
#include "DrawDebugHelpers.h"

// Sets default values
ALighthousePawn::ALighthousePawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	SphereRoot = CreateDefaultSubobject<USphereComponent>(TEXT("SphereRoot"));
	RootComponent = SphereRoot;

    Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PawnMesh"));
	Mesh->SetupAttachment(RootComponent);

	LightMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LightMesh"));
	LightMesh->SetupAttachment(Mesh);

    Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("PawnCamera"));
    Camera->SetupAttachment(RootComponent);

	SpotLightComponent = CreateDefaultSubobject<USpotLightComponent>(TEXT("SpotLightComponent"));
	SpotLightComponent->SetupAttachment(LightMesh);

    SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
    SpringArm->SetupAttachment(RootComponent);
    SpringArm->TargetArmLength = 400.0f;
    SpringArm->bDoCollisionTest = false;
    Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);

    FloatingMovement = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("FloatingMovement"));

	Spline = CreateDefaultSubobject<USplineComponent>(TEXT("Spline"));
}

void ALighthousePawn::BeginPlay()
{
	Super::BeginPlay();

	UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("Window"), WindowActors);

	AActor* PlayerCharacterActor = UGameplayStatics::GetActorOfClass(GetWorld(), APlayerCharacter::StaticClass());
	PlayerCharacter = Cast<APlayerCharacter>(PlayerCharacterActor);
}

void ALighthousePawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ALighthousePawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
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
		EnhancedInputComponent->BindAction(ShootAction, ETriggerEvent::Triggered, this, &ALighthousePawn::Shoot);

		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ALighthousePawn::Move);

		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Triggered, this, &ALighthousePawn::Interact);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void ALighthousePawn::Move(const FInputActionValue& Value)
{
	if (bDisableMovement) return;

	FVector2D MovementVector = Value.Get<FVector2D>();
	MovementVector *= RotationSpeed;

	if (Controller)
	{
		// Calculate potential new rotation for SpringArm
		FRotator NewSpringArmRotation = SpringArm->GetRelativeRotation() + FRotator(MovementVector.Y, 0.0f, 0.0f);
		NewSpringArmRotation.Pitch = FMath::Clamp(NewSpringArmRotation.Pitch, MinAngle, MaxAngle);  // Clamping the pitch rotation

		// Update SpringArm rotation
		SpringArm->SetRelativeRotation(NewSpringArmRotation);

		// Calculate potential new rotation for LightMesh
		FRotator NewLightMeshRotation = LightMesh->GetRelativeRotation() + FRotator(MovementVector.Y, 0.0f, 0.0f);
		NewLightMeshRotation.Pitch = FMath::Clamp(NewLightMeshRotation.Pitch, MinAngle, MaxAngle);  // Clamping the pitch rotation

		// Update LightMesh rotation
		LightMesh->SetRelativeRotation(NewLightMeshRotation);

		// Rotate the actor itself
		AddActorLocalRotation(FRotator(0, MovementVector.X, 0.0f));
	}
}


void ALighthousePawn::Shoot(const FInputActionValue& Value)
{
	UE_LOG(LogTemp, Log, TEXT("Shoot function called"));

	if (FlashCount < 1) {
		UE_LOG(LogTemp, Warning, TEXT("No Flashes Left"));
		return;
	}

	if (GetWorld()->GetTimerManager().IsTimerActive(LightTimerHandle)) {
		UE_LOG(LogTemp, Warning, TEXT("Timer is active, cannot shoot"));
		return;
	}

	// Perform cylinder ray cast
	FVector Start = Mesh->GetComponentLocation();
	FVector End = Start + Mesh->GetForwardVector() * RaycastRange;

	// Draw debug line for the raycast direction
	DrawDebugLine(GetWorld(), Start, End, FColor::Green, false, -1.0f, 0, 2.0f);

	TArray<FHitResult> HitResults;
	FCollisionShape CollisionShape = FCollisionShape::MakeCapsule(RaycastRadius, RaycastRange);

	UE_LOG(LogTemp, Log, TEXT("Performing cylinder ray cast from %s to %s"), *Start.ToString(), *End.ToString());

	bool bHit = GetWorld()->SweepMultiByObjectType(
		HitResults,
		Start,
		End,
		FQuat::Identity,
		FCollisionObjectQueryParams(ECollisionChannel::ECC_Pawn),
		CollisionShape
	);

	// Draw debug cylinder for the raycast shape
	DrawDebugCylinder(GetWorld(), Start, End, RaycastRadius, 32, FColor::Red);

	// Logging the ray cast result
	UE_LOG(LogTemp, Log, TEXT("Ray cast hit count: %d"), HitResults.Num());

	// Check if any AWaterEnemyCharacter actors were hit
	for (const FHitResult& HitResult : HitResults) {
		if (AWaterEnemyCharacter* WaterEnemy = Cast<AWaterEnemyCharacter>(HitResult.GetActor())) {
			UE_LOG(LogTemp, Log, TEXT("Hit AWaterEnemyCharacter: %s"), *WaterEnemy->GetName());

			// Draw debug point for the hit location
			DrawDebugPoint(GetWorld(), HitResult.ImpactPoint, 10.0f, FColor::Yellow, false, -1.0f, 0);

			if (AEnemySpawnerActor* Spawner = Cast<AEnemySpawnerActor>(WaterEnemy->Owner)) {
				Spawner->bSpawnerTaken = false;
				UE_LOG(LogTemp, Log, TEXT("AEnemySpawnerActor found and bSpawnerTaken set to false"));
			}

			WaterEnemy->Destroy();
			UE_LOG(LogTemp, Log, TEXT("AWaterEnemyCharacter destroyed"));
		}
	}

	FlashCount--;

	GetWorld()->GetTimerManager().SetTimer(LightTimerHandle, this, &ALighthousePawn::TurnOnLight, GetWorld()->GetDeltaSeconds(), true);
	UE_LOG(LogTemp, Log, TEXT("Light timer set"));
}


void ALighthousePawn::TurnOnLight() {
	float Alpha = ElapsedTime / LightFlashDuration;
	if (Alpha >= 1.0f)
	{
		GetWorld()->GetTimerManager().ClearTimer(LightTimerHandle);
		ElapsedTime = 0;

		SpotLightComponent->SetIntensity(0.0f);
		return;
	}

	SpotLightComponent->SetIntensity(10000.0f);

	ElapsedTime += GetWorld()->GetDeltaSeconds();
}

void ALighthousePawn::Interact(const FInputActionValue& Value) {
	if (bDisableMovement) return;

	if (WindowActors.Num() > 0 && PlayerCharacter)
	{
		// Initialize with a maximum possible distance and a null actor
		float MinDistance = FLT_MAX;
		AActor* ClosestWindowActor = nullptr;

		// Find the closest window actor to the lighthouse camera
		for (AActor* WindowActor : WindowActors)
		{
			if (WindowActor)
			{
				float Distance = FVector::Dist(Camera->GetComponentLocation(), WindowActor->GetActorLocation());
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
			Spline->AddSplinePoint(PlayerCharacter->Camera->GetComponentLocation(), ESplineCoordinateSpace::World);

			bDisableMovement = true;
			GetController()->SetIgnoreLookInput(true);

			// Start the timer
			ElapsedTime = 0;
			StartingCameraRotation = Camera->GetComponentRotation();
			StartingCameraLocation = Camera->GetComponentLocation();
			GetWorld()->GetTimerManager().SetTimer(MovementTimerHandle, this, &ALighthousePawn::MoveCameraAlongSpline, GetWorld()->GetDeltaSeconds(), true);
		}
	}
}

void ALighthousePawn::MoveCameraAlongSpline() {
	float Alpha = ElapsedTime / LightHouseTransitionDuration;
	if (Alpha >= 1.0f)
	{
		// Movement complete, stop the timer and re-enable player movement and camera control
		GetWorld()->GetTimerManager().ClearTimer(MovementTimerHandle);
		bDisableMovement = false;
		GetController()->SetIgnoreLookInput(false);

		APlayerController* PlayerController = Cast<APlayerController>(GetController());
		if (!PlayerController) {
			UE_LOG(LogTemp, Warning, TEXT("Null PlayerController"));
			return;
		}

		PlayerController->Possess(PlayerCharacter);

		Camera->SetWorldLocation(StartingCameraLocation);
		Camera->SetWorldRotation(StartingCameraRotation);
		return;
	}

	FTransform Transform = Spline->GetTransformAtDistanceAlongSpline(Spline->GetSplineLength() * Alpha, ESplineCoordinateSpace::World);
	Camera->SetWorldLocation(Transform.GetLocation());

// 	if (!PlayerCharacter->StartingControlRotation) {
// 		UE_LOG(LogTemp, Warning, TEXT("Null StartingControlRotation"));
// 		return;
// 	}

	FRotator NewRotation = FMath::Lerp(StartingCameraRotation, PlayerCharacter->StartingControlRotation, Alpha);
	Camera->SetWorldRotation(NewRotation);

	ElapsedTime += GetWorld()->GetDeltaSeconds();
}