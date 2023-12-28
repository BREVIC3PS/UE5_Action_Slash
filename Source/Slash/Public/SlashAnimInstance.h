// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "CharacterTypes.h"
#include "SlashAnimInstance.generated.h"

/**
 *
 */
UCLASS()
class SLASH_API USlashAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaTime) override;

	void CalculateLean(float DeltaTime);

	void CalculateYawOffset(float DeltaTime);

	FRotator CharacterRotationLastFrame;
	FRotator CharacterRotation;
	FRotator DeltaRotation;

	UPROPERTY(BlueprintReadOnly)
	class ASlashCharacter* SlashCharacter;

	UPROPERTY(BlueprintReadOnly, Category = Movement)
	class UCharacterMovementComponent* SlashCharacterMovement;

	UPROPERTY(BlueprintReadOnly, Category = Movement)
	float GroundSpeed;

	UPROPERTY(BlueprintReadOnly, Category = Movement)
	bool isFalling;

	UPROPERTY(BlueprintReadOnly, Category = Movement)
	bool bIsAccelerating;

	UPROPERTY(BlueprintReadOnly, Category = Movement)
	float YawOffset;

	UPROPERTY(BlueprintReadOnly, Category = Movement)
	float Lean;

	UPROPERTY(BlueprintReadOnly, Category = Movement)
	ECharacterState CharacterState;

	UPROPERTY(BlueprintReadOnly, Category = Movement)
	EActionState ActionState;

	UPROPERTY(BlueprintReadOnly, Category = Movement)
	ECharacterMoveState MoveState;

	UPROPERTY(BlueprintReadOnly, Category = Movement)
	TEnumAsByte<EDeathPose> DeathPose;

	UPROPERTY(BlueprintReadOnly, Category = Movement)
	float AO_YawOffset;

	UPROPERTY(BlueprintReadOnly, Category = Movement)
	float AO_Pitch;

	UPROPERTY(BlueprintReadOnly, Category = Movement)
	ETurningInPlace TurningInPlace;
};
