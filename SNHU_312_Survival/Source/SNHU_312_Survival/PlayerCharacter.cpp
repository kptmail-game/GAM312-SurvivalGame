// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"

#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
APlayerCharacter::APlayerCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// --- Tunable defaults -----------------------------------------------------
	// Set here rather than in the header so the values show up as the Blueprint
	// defaults for anything derived from this class.
	WalkSpeed       = 400.0f;
	SprintSpeed     = 800.0f;
	CameraEyeHeight = 64.0f;

	// --- First-person camera --------------------------------------------------
	// Components must be created in the constructor, never in BeginPlay.
	FirstPersonCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));

	// Attach the camera to the capsule instead of the skeletal mesh. A mesh-attached
	// camera inherits the walk animation's head bob, which makes the view lurch.
	FirstPersonCamera->SetupAttachment(GetCapsuleComponent());

	// Lift the camera to eye level. This offset is relative to the capsule's centre,
	// so it travels up the capsule's local Z axis.
	FirstPersonCamera->SetRelativeLocation(FVector(0.0f, 0.0f, CameraEyeHeight));

	// Rotate the camera with the controller's rotation, which is what the mouse
	// drives. This is the flag that makes mouse look actually move the view.
	FirstPersonCamera->bUsePawnControlRotation = true;

	// --- Rotation behaviour ---------------------------------------------------
	// In first person the body faces wherever the camera faces, so the pawn takes its
	// yaw from the controller, and the movement component must not rotate the
	// character toward its velocity instead.
	bUseControllerRotationYaw   = true;
	bUseControllerRotationPitch = false;  // pitch is camera-only; the body stays upright
	bUseControllerRotationRoll  = false;

	if (UCharacterMovementComponent* Movement = GetCharacterMovement())
	{
		Movement->bOrientRotationToMovement = false;
		Movement->MaxWalkSpeed = WalkSpeed;
	}

	// The inherited character mesh is a third-person body. Hide it from the owning
	// player so they are not looking at the inside of their own torso, while shadows
	// and other players still see it.
	if (GetMesh())
	{
		GetMesh()->SetOwnerNoSee(true);
	}
}

// Called when the game starts or when spawned
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	// Re-apply WalkSpeed at runtime as well as in the constructor, so a value changed
	// on a Blueprint child or on the placed actor is respected when play begins.
	if (UCharacterMovementComponent* Movement = GetCharacterMovement())
	{
		Movement->MaxWalkSpeed = WalkSpeed;
	}
}

// Called every frame
void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input.
// The string names below must match the mappings in Project Settings > Input exactly.
// A mismatch fails silently at runtime: no compile error, no movement.
void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Axis mappings report a value every frame from the keys bound to them.
	PlayerInputComponent->BindAxis("MoveForward", this, &APlayerCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight",   this, &APlayerCharacter::MoveRight);

	// Mouse look. These two handlers already exist on APawn, so they are bound
	// directly rather than being wrapped in functions of our own.
	PlayerInputComponent->BindAxis("Turn",   this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);

	// Action mappings fire once when the key goes down and once when it comes up.
	PlayerInputComponent->BindAction("Sprint", IE_Pressed,  this, &APlayerCharacter::StartSprint);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &APlayerCharacter::StopSprint);

	// Jump is inherited from ACharacter; it just needs the binding.
	PlayerInputComponent->BindAction("Jump", IE_Pressed,  this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
}

// W and S. Value arrives as +1 for forward and -1 for backward.
void APlayerCharacter::MoveForward(float Value)
{
	// Skip the work entirely when there is no input, or no controller possessing us.
	if (Controller == nullptr || Value == 0.0f)
	{
		return;
	}

	// Movement follows where the player is looking, but only the yaw part of it.
	// Zeroing pitch and roll stops the character walking into the floor when the
	// camera is aimed downward.
	const FRotator YawOnlyRotation(0.0f, Controller->GetControlRotation().Yaw, 0.0f);

	// Build a rotation matrix from that yaw and take its X axis. That gives the unit
	// vector pointing "forward" in world space for the direction the player faces.
	const FVector Direction = FRotationMatrix(YawOnlyRotation).GetUnitAxis(EAxis::X);

	// Hand the direction and magnitude to the movement component, which handles
	// acceleration, collision, and framerate independence for us.
	AddMovementInput(Direction, Value);
}

// D and A. Value arrives as +1 for right and -1 for left.
void APlayerCharacter::MoveRight(float Value)
{
	if (Controller == nullptr || Value == 0.0f)
	{
		return;
	}

	const FRotator YawOnlyRotation(0.0f, Controller->GetControlRotation().Yaw, 0.0f);

	// The Y axis of the same matrix is the vector 90 degrees to the right of forward.
	const FVector Direction = FRotationMatrix(YawOnlyRotation).GetUnitAxis(EAxis::Y);

	AddMovementInput(Direction, Value);
}

// Sprint key held: raise the movement component's speed cap.
void APlayerCharacter::StartSprint()
{
	if (UCharacterMovementComponent* Movement = GetCharacterMovement())
	{
		Movement->MaxWalkSpeed = SprintSpeed;
	}
}

// Sprint key released: drop back to the normal walking speed.
void APlayerCharacter::StopSprint()
{
	if (UCharacterMovementComponent* Movement = GetCharacterMovement())
	{
		Movement->MaxWalkSpeed = WalkSpeed;
	}
}
