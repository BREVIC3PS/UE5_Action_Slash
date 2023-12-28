// Fill out your copyright notice in the Description page of Project Settings.


#include "SlashAnimInstance.h"
#include"SlashCharacter.h"
#include"GameFramework/CharacterMovementComponent.h"
#include"Kismet/KismetMathLibrary.h"

void USlashAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	SlashCharacter = Cast<ASlashCharacter>(TryGetPawnOwner());

	if (SlashCharacter) {
		SlashCharacterMovement = (SlashCharacter->GetCharacterMovement());
	}
}

void USlashAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
	Super::NativeUpdateAnimation(DeltaTime);

	if (SlashCharacterMovement) {
		GroundSpeed = UKismetMathLibrary::VSizeXY(SlashCharacterMovement->Velocity);
		isFalling = SlashCharacterMovement->IsFalling();
		bIsAccelerating = SlashCharacterMovement->GetCurrentAcceleration().Size() > 0.f ? true : false;
		CharacterState = SlashCharacter->GetCharacterState();
		ActionState = SlashCharacter->GetActionState();
		MoveState = SlashCharacter->GetMoveState();
		DeathPose = SlashCharacter->GetDeathPose();

		// Offset Yaw for Strafing
		CalculateYawOffset(DeltaTime);
		CalculateLean(DeltaTime);
		AO_YawOffset = SlashCharacter->GetAO_Yaw();
		AO_Pitch = SlashCharacter->GetAO_Pitch();

		TurningInPlace = SlashCharacter->GetTurningInPlace();
	}
}

void USlashAnimInstance::CalculateLean(float DeltaTime)
{
	CharacterRotationLastFrame = CharacterRotation;
	CharacterRotation = SlashCharacter->GetActorRotation();
	const FRotator Delta = UKismetMathLibrary::NormalizedDeltaRotator(CharacterRotation, CharacterRotationLastFrame);
	const float Target = Delta.Yaw / DeltaTime;
	const float Interp = FMath::FInterpTo(Lean, Target, DeltaTime, 6.f);
	Lean = FMath::Clamp(Interp, -90.f, 90.f);
}

void USlashAnimInstance::CalculateYawOffset(float DeltaTime)
{
	FRotator AimRotation = SlashCharacter->GetBaseAimRotation();
	FRotator MovementRotation = UKismetMathLibrary::MakeRotFromX(SlashCharacter->GetVelocity());
	FRotator DeltaRot = UKismetMathLibrary::NormalizedDeltaRotator(MovementRotation, AimRotation);
	DeltaRotation = FMath::RInterpTo(DeltaRotation, DeltaRot, DeltaTime, 6.f);
	YawOffset = DeltaRotation.Yaw;
}
