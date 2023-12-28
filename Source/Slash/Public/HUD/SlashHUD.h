// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "SlashHUD.generated.h"

/**
 * 
 */
class USlashOverlap;
UCLASS()
class SLASH_API ASlashHUD : public AHUD
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;
private:
	UPROPERTY(EditDefaultsOnly, Category = Slash)
	TSubclassOf<USlashOverlap> SlashOverlayClass;
	
	UPROPERTY()
	USlashOverlap* SlashOverlay;

public:
	FORCEINLINE USlashOverlap* GetSlashOverlay() const { return SlashOverlay; }
};
