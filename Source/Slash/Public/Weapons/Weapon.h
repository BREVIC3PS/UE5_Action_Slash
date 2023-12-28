// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyItem.h"
#include "Weapon.generated.h"

class USoundBase;
class UBoxComponent;
/**
 *
 */


UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	EWT_ShortSword UMETA(DisplayName = "Sho rt Sword"),
	EWT_LongSword UMETA(DisplayName = "LongSword"),
	EWT_Bow UMETA(DisplayName = "Bow"),

	EWT_MAX UMETA(DisplayName = "DefaultMAX")
};


UCLASS()
class SLASH_API AWeapon : public AMyItem
{
	GENERATED_BODY()

public:
	AWeapon();
	void Equip(USceneComponent* InParent, FName InSocketName, AActor* NewOwner, APawn* NewInstigator);
	void DeactivateEmbers();
	void DisableSphereCollision();
	void PlayEquipSound();
	void AttachMeshToSocket(USceneComponent* InParent, const FName& InSocketName);

	TArray<AActor*> IgnoreActors;

	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	EWeaponType WeaponType;
protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	bool ActorIsSameType(AActor* OtherActor);

	void ExecuteGetHit(FHitResult& BoxHit);

	UFUNCTION(BlueprintImplementableEvent)
	void CreateFields(const FVector& FieldLocation);
private:
	void BoxTrace(FHitResult& BoxHit);

	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	FVector BoxTraceExtent = FVector(5.f);

	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	bool bShowBoxDebug = false;

	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	USoundBase* EquipSound;

	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	UBoxComponent* WeaponBox;

	UPROPERTY(VisibleAnywhere)
	USceneComponent* BoxTraceStart;

	UPROPERTY(VisibleAnywhere)
	USceneComponent* BoxTraceEnd;

	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	float Damage = 20.f;


public:
	FORCEINLINE UBoxComponent* GetWeaponBox() const { return WeaponBox; }
};
