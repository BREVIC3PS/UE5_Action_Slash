// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MyItem.generated.h"

class USphereComponent;
class USoundBase;

enum class EItemState :uint8
{
	EIS_Hovering,
	EIS_Equipped
};


UCLASS()
class SLASH_API AMyItem : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMyItem();
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	virtual void OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	virtual void SpawnPickupSound();
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SineParameters")
	float Amplitude = 0.25f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SineParameters")
	float TimeConstant = 5.f;

	UFUNCTION(BlueprintPure)
	float TransformedSin();

	UFUNCTION(BlueprintPure)
	float TransformedCos();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* ItemMesh;

	EItemState ItemState = EItemState::EIS_Hovering;

	UPROPERTY(VisibleAnywhere)
	USphereComponent* Sphere;

	UPROPERTY(EditAnywhere)
	class UNiagaraComponent* ItemEffect;

	UPROPERTY(EditAnywhere)
	USoundBase* PickupSound;
private:	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float RunningTime;

	UPROPERTY(EditAnywhere)
	class UNiagaraSystem* PickupEffect;

};
