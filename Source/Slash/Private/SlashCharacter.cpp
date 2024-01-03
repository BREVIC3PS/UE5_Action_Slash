// Fill out your copyright notice in the Description page of Project Settings.


#include "SlashCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GroomComponent.h"
#include "MyItem.h"
#include "Weapons/Weapon.h"
#include "Animation/AnimMontage.h"
#include "Components/BoxComponent.h"
#include "HUD/SlashHUD.h"
#include "HUD/SlashOverlap.h"
#include "AttributeComponents.h"
#include "Soul.h"
#include "Treasure.h"
#include <Kismet/GameplayStatics.h>
#include <Enemy/Enemy.h>
#include <Kismet/KismetMathLibrary.h>
#include "HUD/SlashHUD.h"
#include "Weapons/Projectile.h"
#include "SlashAnimInstance.h"


// Sets default values
ASlashCharacter::ASlashCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0, 380.f, 0);

	PrimaryActorTick.bCanEverTick = true;
	GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	GetMesh()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Overlap);
	GetMesh()->SetGenerateOverlapEvents(true);

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->TargetArmLength = 365.f;
	CameraBoom->SocketOffset = FVector(0.0f, 100.f, 100.0f);

	//Camera Settings
	ViewCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("ViewCamera"));
	ViewCamera->SetupAttachment(CameraBoom);

	DefaultFOV = ViewCamera->FieldOfView;
	CurrentFOV = DefaultFOV;
	ViewCamera->PostProcessSettings.bOverride_DepthOfFieldFocalDistance = true;
	ViewCamera->PostProcessSettings.DepthOfFieldFocalDistance = 10000;
	ViewCamera->PostProcessSettings.bOverride_DepthOfFieldFstop = true;
	ViewCamera->PostProcessSettings.DepthOfFieldFstop = 4.0;


	Hair = CreateDefaultSubobject<UGroomComponent>(TEXT("Hair"));
	Hair->SetupAttachment(GetMesh());
	Hair->AttachmentName = FString("head");

	Eyebrows = CreateDefaultSubobject<UGroomComponent>(TEXT("Eyebrows"));
	Eyebrows->SetupAttachment(GetMesh());
	Eyebrows->AttachmentName = FString("head");

	EquippedWeapon = nullptr;

	TurningInPlace = ETurningInPlace::ETIP_NotTurning;

	AttachedProjectile = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("AttachedProjectile"));
	AttachedProjectile->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	AttachedProjectile->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	AttachedProjectile->SetupAttachment(GetRootComponent());
	AttachedProjectile->SetVisibility(false);

}

void ASlashCharacter::GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter)
{
	Super::GetHit_Implementation(ImpactPoint, Hitter);
	SetWeaponCollisionEnabled(ECollisionEnabled::NoCollision);
	if (IsAlive())
	{
		ActionState = EActionState::EAS_HitReaction;
	}
}

void ASlashCharacter::SetOverlappingItem(AMyItem* Item)
{
	OverlappingItem = Item;
}

void ASlashCharacter::AddSouls(ASoul* Soul)
{
	if (Attributes && SlashOverlay)
	{
		Attributes->AddSouls(Soul->GetSouls());
		SlashOverlay->SetSouls(Attributes->GetSouls());
	}
}

void ASlashCharacter::AddGold(ATreasure* Treasure)
{
	if (Attributes && SlashOverlay)
	{
		Attributes->AddGold(Treasure->GetGold());
		SlashOverlay->SetGold(Attributes->GetGold());
	}
}

void ASlashCharacter::AimOffset(float DeltaTime)
{
	FVector Velocity = GetVelocity();
	Velocity.Z = 0.f;
	float Speed = Velocity.Size();
	bool bIsInAir = GetCharacterMovement()->IsFalling();

	if (Speed == 0.f && !bIsInAir)
	{
		FRotator CurrentAimRotation = FRotator(0.f, GetBaseAimRotation().Yaw, 0.f);
		const FRotator DeltaAimRotation = UKismetMathLibrary::NormalizedDeltaRotator(CurrentAimRotation, StartingAimRotation);
		AO_Yaw = DeltaAimRotation.Yaw;
		if (TurningInPlace == ETurningInPlace::ETIP_NotTurning)
		{
			InterpAO_Yaw = AO_Yaw;
		}
		bUseControllerRotationYaw = true;
		TurnInPlace(DeltaTime);
	}
	if (Speed > 0.f || bIsInAir) //running jumping
	{
		StartingAimRotation = FRotator(0.f, GetBaseAimRotation().Yaw, 0.f);
		AO_Yaw = 0.f;
		bUseControllerRotationYaw = true;
		TurningInPlace = ETurningInPlace::ETIP_NotTurning;
	}

	AO_Pitch = GetBaseAimRotation().Pitch;
}

void ASlashCharacter::StartHitReaction()
{
	// 停顿动作的持续时间（秒）
	

	// 暂停动作
	//APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	//if (PlayerController)
	//{
	//	PlayerController->SetIgnoreLookInput(true);
	//	PlayerController->SetIgnoreMoveInput(true);
	//}

	// 设置计时器，用于在一段时间后恢复动作
	GetWorldTimerManager().SetTimer(HitReactionTimer, this, &ASlashCharacter::ResumeAction, HitReactionDuration, false);

	// 获取动画实例
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance)
	{
		// 保存原始动画播放速度
		OriginalAnimationSpeed = AnimInstance->Montage_GetPlayRate(AttackMontage);

		// 减缓动画播放速度
		AnimInstance->Montage_SetPlayRate(AttackMontage, 0.2f);
	}
}

void ASlashCharacter::ResumeAction()
{
	// 恢复原始动画播放速度
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance)
	{
		AnimInstance->Montage_SetPlayRate(AttackMontage, OriginalAnimationSpeed);
	}
}

// Called when the game starts or when spawned
void ASlashCharacter::BeginPlay()
{
	Super::BeginPlay();
	InitializeSlashOverlay();
	Tags.Add(FName("EngageableTarget"));
	FAttachmentTransformRules TransformRules(EAttachmentRule::SnapToTarget, true);
	AttachedProjectile->AttachToComponent(GetMesh(), TransformRules, FName("RightIndexSocket"));

}

void ASlashCharacter::InitializeSlashOverlay()
{
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer());
		if (Subsystem) {
			Subsystem->AddMappingContext(SlashContext, 0);
		}
		ASlashHUD* SlashHUD = Cast<ASlashHUD>(PlayerController->GetHUD());
		if (SlashHUD)
		{
			SlashOverlay = SlashHUD->GetSlashOverlay();
			if (SlashOverlay && Attributes)
			{
				SlashOverlay->SetHealthBarPercent(Attributes->GetHealthPercent());
				SlashOverlay->SetStaminaPercent(1.f);
				SlashOverlay->SetGold(0);
				SlashOverlay->SetSouls(0);
			}
		}
	}
}

void ASlashCharacter::Move(const FInputActionValue& Value)
{
	if (ActionState != EActionState::EAS_Unoccupied)return;
	const FVector2D MovementVector = Value.Get<FVector2D>();
	if (Controller) {

		const FRotator ControlRotation = GetControlRotation();
		const FRotator YawRotation(0.f, ControlRotation.Yaw, 0.f);

		const FVector DirectionForward = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);//forward

		//FVector Forward = GetActorForwardVector();
		AddMovementInput(DirectionForward, MovementVector.Y);

		const FVector DirectionRight = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);//right and left
		//FVector Right = GetActorRightVector();
		AddMovementInput(DirectionRight, MovementVector.X);
		
	}
}

void ASlashCharacter::Look(const FInputActionValue& Value)
{
	const FVector2D LookAxisValue = Value.Get<FVector2D>();
	if (Controller) {
		AddControllerYawInput(LookAxisValue.X);
		AddControllerPitchInput(LookAxisValue.Y);
	}
}

void ASlashCharacter::EKeyPressed(const FInputActionValue& Value)
{
	AWeapon* OverlappingWeapon = Cast<AWeapon>(OverlappingItem);
	if (OverlappingWeapon)
	{
		EquipWeapon(OverlappingWeapon);
	}
	else
	{
		if (bCanDisarm())
		{
			Disarm();
		}
		else if (bCanArm())
		{
			Arm();
		}
	}
}

void ASlashCharacter::Dodge(const FInputActionValue& Value)
{
	if (ActionState != EActionState::EAS_Unoccupied)return;
	if (Attributes && Attributes->GetStamina() > Attributes->GetDodgeCost())
	{
		ActionState = EActionState::EAS_Dodge;
		GetCharacterMovement()->Velocity;
		FRotator NewRotation = UKismetMathLibrary::Conv_VectorToRotator(GetCharacterMovement()->GetLastInputVector()); 
		SetActorRotation(NewRotation);
		PlayDodgeMontage();
		Attributes->UseStamina(Attributes->GetDodgeCost());
		if (SlashOverlay)
		{
			SlashOverlay->SetStaminaPercent(Attributes->GetStaminaPercent());
		}
	}
}

void ASlashCharacter::ChangeToLockedControl()
{
	bUseControllerRotationYaw = true;
	GetCharacterMovement()->bOrientRotationToMovement = false;
	GetCharacterMovement()->bUseControllerDesiredRotation = true;
}

void ASlashCharacter::ChangeToUnlockedControl()
{
	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->bUseControllerDesiredRotation = false;
}

void ASlashCharacter::Lock(const FInputActionValue& Value)
{
	if (MoveState == ECharacterMoveState::ECMS_Unlocked)
	{
		MoveState = ECharacterMoveState::ECMS_Locked;
		ChangeToLockedControl();
		if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
		{
			PlayerController->SetIgnoreLookInput(true);
		}
		GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
		FindNearestEnemy();
	}
	else if (MoveState == ECharacterMoveState::ECMS_Locked)
	{
		MoveState = ECharacterMoveState::ECMS_Unlocked;
		ChangeToUnlockedControl();
		if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
		{
			Controller->SetIgnoreLookInput(false);
		}
		if (LockedEnemy)
		{
			LockedEnemy->CancelSelect();
		}
		NearestEnemy = nullptr;
		LockedEnemy = nullptr;
		GetCharacterMovement()->MaxWalkSpeed = RunSpeed;
	}
}

void ASlashCharacter::AimButtonPressed(const FInputActionValue& Value)
{
	if (CharacterState == ECharacterState::ECS_EquippedBow)
	{
		MoveState = ECharacterMoveState::ECMS_Aiming;
		BowState = EBowState::EBS_Aiming;
		GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
		//ChangeToLockedControl();
	}
}

void ASlashCharacter::DrawArrow()
{
	BowState = EBowState::EBS_DrawingArrow;
	PlayMontageSection(ShootMontage, FName("DrawArrow"));
	AttachArrow();
}

void ASlashCharacter::AimButtonReleased(const FInputActionValue& Value)
{
	if (CharacterState == ECharacterState::ECS_EquippedBow)
	{
		MoveState = ECharacterMoveState::ECMS_Unlocked;
		ActionState = EActionState::EAS_Unoccupied;
		ChangeToUnlockedControl();
		GetCharacterMovement()->MaxWalkSpeed = RunSpeed;
	}
	AttachedProjectile->SetVisibility(false);
}

void ASlashCharacter::Attack(const FInputActionValue& Value)
{
	Super::Attack(Value);
	if (CanAttack())
	{
		FindNearestEnemy();
		CombatTarget = NearestEnemy;
		PlayAttackMontage();
		ActionState = EActionState::EAS_Attacking;
	}
	else if (CanShoot())
	{
		PlayShootMontage();
		BowState = EBowState::EBS_Shooting;
		EquippedWeapon->Fire(HitTarget, GetMesh(), FName("RightIndexSocket"));
		if (AttachedProjectile)AttachedProjectile->SetVisibility(false);
	}
}

void ASlashCharacter::WallRun(const FInputActionValue& Value)
{
}

void ASlashCharacter::EquipWeapon(AWeapon* Weapon)
{
	switch (Weapon->WeaponType)
	{
	case(EWeaponType::EWT_ShortSword):
	case(EWeaponType::EWT_LongSword):
	{
		Weapon->Equip(GetMesh(), FName("RightHandSocket"), this, this);
		CharacterState = ECharacterState::ECS_EquippedOneHandedWeapon;
	}break;
	case(EWeaponType::EWT_Bow):
	{
		Weapon->Equip(GetMesh(), FName("LeftHandSocket"), this, this);
		CharacterState = ECharacterState::ECS_EquippedBow;
	}
	default:
		break;
	}
	OverlappingItem = nullptr;
	EquippedWeapon = Weapon;
}

void ASlashCharacter::EquipBow()
{
	CharacterState = ECharacterState::ECS_EquippedBow;
}

bool ASlashCharacter::CanAttack()
{
	return ActionState == EActionState::EAS_Unoccupied &&
		CharacterState == ECharacterState::ECS_EquippedOneHandedWeapon;
}

bool ASlashCharacter::CanShoot()
{
	return
		MoveState == ECharacterMoveState::ECMS_Aiming;
		BowState == EBowState::EBS_Ready &&
		CharacterState == ECharacterState::ECS_EquippedBow;
}

bool ASlashCharacter::bCanDisarm()
{
	return ActionState == EActionState::EAS_Unoccupied &&
		CharacterState != ECharacterState::ECS_Unequipped;
}

bool ASlashCharacter::bCanArm()
{
	return ActionState == EActionState::EAS_Unoccupied &&
		CharacterState == ECharacterState::ECS_Unequipped &&
		EquippedWeapon;
}

void ASlashCharacter::PlayEquipMontage(FName SectionName)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && EquipMontage) {
		AnimInstance->Montage_Play(EquipMontage);
		AnimInstance->Montage_JumpToSection(SectionName, EquipMontage);
	}
}

void ASlashCharacter::PlayShootMontage()
{
	PlayMontageSection(ShootMontage, FName("Shoot"));
}

void ASlashCharacter::Die()
{
	Super::Die();

	ActionState = EActionState::EAS_Dead;
}

void ASlashCharacter::LockOnToNearestEnemy(float DeltaTime)
{
	// Find the nearest enemy
	if (LockedEnemy == nullptr)return;

	// Adjust camera and character rotation to focus on the locked enemy
	if (LockedEnemy)
	{
		// Calculate the look at rotation
		FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), LockedEnemy->GetActorLocation());

		// Interpolate the rotation smoothly
		FRotator SmoothRotation = FMath::RInterpTo(GetControlRotation(), LookAtRotation, DeltaTime, 100.0f);

		// Set the new rotation
		//SetActorRotation(SmoothRotation);
		GetController()->SetControlRotation(SmoothRotation);
		// (Optionally) Set camera rotation if using a SpringArmComponent
		if (CameraBoom)
		{
			CameraBoom->SetRelativeRotation(SmoothRotation);
		}
		
	}
}

void ASlashCharacter::FindNearestEnemy()
{
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEnemy::StaticClass(), FoundActors);

	float NearestDistance = MAX_FLT;

	AActor* NearestActor = nullptr;

	for (AActor* Enemy : FoundActors)
	{
		float Distance = FVector::DistSquared(GetActorLocation(), Enemy->GetActorLocation());
		if (Distance < NearestDistance)
		{
			NearestActor = Enemy;
			NearestDistance = Distance;
		}
	}
	if (NearestActor != nullptr)
	{
		NearestEnemy = Cast<AEnemy>(NearestActor);
		if (NearestEnemy)
		{
			UE_LOG(LogTemp, Warning, TEXT("Found the nearestEnemy!"));
			LockedEnemy = NearestEnemy;
			LockedEnemy->Select();
		}
	}
}

void ASlashCharacter::AttackEnd()
{
	ActionState = EActionState::EAS_Unoccupied;
}

void ASlashCharacter::DodgeEnd()
{
	Super::DodgeEnd();
	ActionState = EActionState::EAS_Unoccupied;
}

void ASlashCharacter::ShootEnd()
{
	BowState = EBowState::EBS_Aiming;
}

void ASlashCharacter::AttachArrow()
{
	AttachedProjectile->SetVisibility(true);
}

void ASlashCharacter::DrawEnd()
{
	BowState = EBowState::EBS_Ready;
}

void ASlashCharacter::FinishEquipping()
{
	ActionState = EActionState::EAS_Unoccupied;
}
void ASlashCharacter::AttachWeaponToBack()
{
	if (EquippedWeapon)
	{
		if (EquippedWeapon->WeaponType == EWeaponType::EWT_ShortSword)
			EquippedWeapon->AttachMeshToSocket(GetMesh(), FName("SpineSocket"));
		if (EquippedWeapon->WeaponType == EWeaponType::EWT_Bow)
			EquippedWeapon->AttachMeshToSocket(GetMesh(), FName("SpineBowSocket"));
	}
}

void ASlashCharacter::AttachWeaponToHand()
{
	if (EquippedWeapon)
	{
		if(EquippedWeapon->WeaponType==EWeaponType::EWT_ShortSword)
			EquippedWeapon->AttachMeshToSocket(GetMesh(), FName("RightHandSocket"));
		if(EquippedWeapon->WeaponType == EWeaponType::EWT_Bow)
			EquippedWeapon->AttachMeshToSocket(GetMesh(), FName("LeftHandSocket"));
	}
}
void ASlashCharacter::HitReactEnd()
{
	ActionState = EActionState::EAS_Unoccupied;
}
void ASlashCharacter::Disarm()
{
	if (EquippedWeapon->WeaponType == EWeaponType::EWT_ShortSword || EquippedWeapon->WeaponType == EWeaponType::EWT_LongSword)
	{
		PlayEquipMontage(FName("Unequip"));
	}
	else if (EquippedWeapon->WeaponType == EWeaponType::EWT_Bow)
	{
		PlayEquipMontage(FName("UnequipBow"));
	}
	CharacterState = ECharacterState::ECS_Unequipped;
	ActionState = EActionState::EAS_EquippingWeapon;

}

void ASlashCharacter::Arm()
{
	if (EquippedWeapon->WeaponType == EWeaponType::EWT_ShortSword || EquippedWeapon->WeaponType == EWeaponType::EWT_LongSword)
	{
		PlayEquipMontage(FName("Equip"));
		CharacterState = ECharacterState::ECS_EquippedOneHandedWeapon;
	}
	else if (EquippedWeapon->WeaponType == EWeaponType::EWT_Bow)
	{
		PlayEquipMontage(FName("EquipBow"));
		CharacterState = ECharacterState::ECS_EquippedBow;
	}
	ActionState = EActionState::EAS_EquippingWeapon;
}

// Called every frame
void ASlashCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (Attributes && SlashOverlay)
	{
		Attributes->RegenStamina(DeltaTime);
		SlashOverlay->SetHealthBarPercent(Attributes->GetHealthPercent());
		SlashOverlay->SetStaminaPercent(Attributes->GetStaminaPercent());
	}
	
	if (MoveState == ECharacterMoveState::ECMS_Locked)
	{
		LockOnToNearestEnemy(DeltaTime);
	}

	else if (MoveState == ECharacterMoveState::ECMS_Aiming)
	{
		AimOffset(DeltaTime);
		FHitResult HitResult;
		TraceUnderCrosshairs(HitResult);
		if(BowState == EBowState::EBS_Aiming)DrawArrow();
		//SetHUDCrosshairs(DeltaTime);
	}
	InterpFOV(DeltaTime);
}

// Called to bind functionality to input
void ASlashCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(MovementAction, ETriggerEvent::Triggered, this, &ASlashCharacter::Move);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ASlashCharacter::Look);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ASlashCharacter::Jump);
		EnhancedInputComponent->BindAction(EKeyAction, ETriggerEvent::Triggered, this, &ASlashCharacter::EKeyPressed);
		EnhancedInputComponent->BindAction(DodgeAction, ETriggerEvent::Triggered, this, &ASlashCharacter::Dodge);
		EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Triggered, this, &ASlashCharacter::Attack);
		EnhancedInputComponent->BindAction(LockAction, ETriggerEvent::Triggered, this, &ASlashCharacter::Lock);
		EnhancedInputComponent->BindAction(AimStartAction, ETriggerEvent::Triggered, this, &ASlashCharacter::AimButtonPressed);
		EnhancedInputComponent->BindAction(AimEndAction, ETriggerEvent::Triggered, this, &ASlashCharacter::AimButtonReleased);
		EnhancedInputComponent->BindAction(WallRunAction, ETriggerEvent::Triggered, this, &ASlashCharacter::WallRun);
	}

}

void ASlashCharacter::Jump()
{
	if (IsUnoccupied())
	{
		Super::Jump();

	}
}

bool ASlashCharacter::IsUnoccupied()
{
	return ActionState == EActionState::EAS_Unoccupied;
}

float ASlashCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	HandleDamage(DamageAmount);
	SetHUDHealth();
	return DamageAmount;
}

void ASlashCharacter::SetHUDHealth()
{
	if (SlashOverlay)
	{
		SlashOverlay->SetHealthBarPercent(Attributes->GetHealthPercent());
	}
}

//void ASlashCharacter::SetHUDCrosshairs(float Deltatime)
//{
//	HUD = HUD == nullptr ? Cast<ASlashHUD>(Controller->GetHUD()) : HUD;
//	if (HUD)
//	{
//		HUD->DrawHUD();
//	}
//}

void ASlashCharacter::TraceUnderCrosshairs(FHitResult& TraceHitResult)
{
	FVector2D viewportSize;
	if (GEngine && GEngine->GameViewport)
	{
		GEngine->GameViewport->GetViewportSize(viewportSize);
	}

	FVector2D CrosshairLocation(viewportSize.X / 2.f, viewportSize.Y / 2.f);
	FVector CrosshairWorldPosition;
	FVector CrosshairWorldDirection;
	bool bScreenToWorld = UGameplayStatics::DeprojectScreenToWorld
	(UGameplayStatics::GetPlayerController(this, 0), CrosshairLocation, 
		CrosshairWorldPosition, CrosshairWorldDirection);
	if (bScreenToWorld)
	{
		FVector Start = CrosshairWorldPosition;

		float DistanceToCharacter = (GetActorLocation() - Start).Size();
		Start += CrosshairWorldDirection * (DistanceToCharacter + 100.f);

		FVector End = Start + CrosshairWorldDirection * TRACE_LENGTH;

		GetWorld()->LineTraceSingleByChannel(TraceHitResult, Start, End, ECollisionChannel::ECC_Visibility);
		if (!TraceHitResult.bBlockingHit)
		{
			TraceHitResult.ImpactPoint = End;
			HitTarget = End;
		}
		else
		{
			HitTarget = TraceHitResult.ImpactPoint;
			DrawDebugSphere(GetWorld(), TraceHitResult.ImpactPoint, 12.f, 12, FColor::Red);
		}
	}
}

void ASlashCharacter::HideCameraIfCharacterClose()
{
	if ((ViewCamera->GetComponentLocation() - GetActorLocation()).Size() < CameraThreshold)
	{
		GetMesh()->SetVisibility(false);
		if (EquippedWeapon && EquippedWeapon->GetMesh())
		{
			EquippedWeapon->GetMesh()->bOwnerNoSee = true;
		}
	}
	else
	{
		GetMesh()->SetVisibility(false);
		if (EquippedWeapon && EquippedWeapon->GetMesh())
		{
			EquippedWeapon->GetMesh()->bOwnerNoSee = false;
		}
	}
}

void ASlashCharacter::InterpFOV(float DeltaTime)
{
	if (EquippedWeapon == nullptr) return;

	if (MoveState == ECharacterMoveState::ECMS_Aiming)
	{
		CurrentFOV = FMath::FInterpTo(CurrentFOV, EquippedWeapon->GetZoomedFOV(), DeltaTime, EquippedWeapon->GetZoomInterpSpeed());
	}
	else
	{
		CurrentFOV = FMath::FInterpTo(CurrentFOV, DefaultFOV, DeltaTime, ZoomInterpSpeed);
	}
	if(ViewCamera)
		ViewCamera->SetFieldOfView(CurrentFOV);
	
}

void ASlashCharacter::TurnInPlace(float DeltaTime)
{
	if (AO_Yaw > 90.f)
	{
		TurningInPlace = ETurningInPlace::ETIP_Right;
	}
	else if (AO_Yaw < -70.f)
	{
		TurningInPlace = ETurningInPlace::ETIP_Left;
	}
	if (TurningInPlace != ETurningInPlace::ETIP_NotTurning)
	{
		InterpAO_Yaw = FMath::FInterpTo(InterpAO_Yaw, 0.f, DeltaTime, 5.f);
		AO_Yaw = InterpAO_Yaw;
		if (FMath::Abs(AO_Yaw) < 15.f)
		{
			TurningInPlace = ETurningInPlace::ETIP_NotTurning;
			StartingAimRotation = FRotator(0.f, GetBaseAimRotation().Yaw, 0.f);
		}
	}
}
