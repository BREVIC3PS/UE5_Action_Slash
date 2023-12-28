// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SlashOverlap.generated.h"

/**
 * 
 */
UCLASS()
class SLASH_API USlashOverlap : public UUserWidget
{
	GENERATED_BODY()
private:

	UPROPERTY(meta = (BindWidget))
	class UProgressBar* HealthProgressBar;

	UPROPERTY(meta = (BindWidget))
	class UProgressBar* StaminaProgressBar;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* GoldText;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* SoulsText;
	
public:
	void SetHealthBarPercent(float Percent);
	void SetStaminaPercent(float Percent);
	void SetGold(int32 Gold);
	void SetSouls(int32 Soul);
};
