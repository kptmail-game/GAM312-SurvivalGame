// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PlayerCharacter.generated.h"

// Forward declaration keeps this header light. The full CameraComponent header is
// only included in the .cpp, where the component is actually constructed.
class UCameraComponent;

/**
 * First-person player character for the survival game.
 * Handles WASD movement, mouse look, and a sprint speed modifier.
 */
UCLASS()
class SNHU_312_SURVIVAL_API APlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	APlayerCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input. The axis and action names used here must
	// match the mappings defined in Project Settings > Input.
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	/** The camera the player sees through. Sits at head height so the view is first
	 *  person -- there is no spring arm, so the camera never pulls back behind the
	 *  character. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	UCameraComponent* FirstPersonCamera;

	/** Normal movement speed, in centimetres per second. Editable so the value can be
	 *  tuned from the editor without recompiling. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float WalkSpeed;

	/** Movement speed while the Sprint key is held down. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float SprintSpeed;

	/** Height of the camera above the character's origin, in centimetres.
	 *  Roughly eye level for the default 180cm capsule. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	float CameraEyeHeight;

	// --- Input handlers -------------------------------------------------------

	/** Moves the character forward (Value > 0, W) or backward (Value < 0, S). */
	void MoveForward(float Value);

	/** Moves the character right (Value > 0, D) or left (Value < 0, A). */
	void MoveRight(float Value);

	/** Raises the movement speed cap to SprintSpeed while the sprint key is held. */
	void StartSprint();

	/** Returns the movement speed cap to WalkSpeed when the sprint key is released. */
	void StopSprint();
};
