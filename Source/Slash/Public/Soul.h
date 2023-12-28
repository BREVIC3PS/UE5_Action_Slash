// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyItem.h"
#include "Soul.generated.h"

/**
 * 
 */
UCLASS()
class SLASH_API ASoul : public AMyItem
{
	GENERATED_BODY()
	
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

	UPROPERTY(EditAnywhere)
	int32 Souls;
public:
	FORCEINLINE int32 GetSouls() const { return Souls; }
	FORCEINLINE void SetSouls(int32 NumberOfSouls) { Souls = NumberOfSouls; }
};
