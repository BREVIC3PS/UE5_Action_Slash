// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include <Components/SizeBox.h>
#include "WorldUserWidget.generated.h"

/**
 * 
 */
UCLASS()
class SLASH_API UWorldUserWidget : public UUserWidget
{
	GENERATED_BODY()
protected:
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	UPROPERTY(EditAnywhere)
	USizeBox* ParentSizeBox;

public:
	UPROPERTY(EditAnywhere, Category="UI")
	FVector WorldOffset;

	UPROPERTY(BlueprintReadOnly, Category="UI")
	AActor* AttachedActor;
	
};
