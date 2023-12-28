// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/SlashOverlap.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

void USlashOverlap::SetHealthBarPercent(float Percent)
{
	if (HealthProgressBar)
	{
		HealthProgressBar->SetPercent(Percent);
	}
}

void USlashOverlap::SetStaminaPercent(float Percent)
{
	if (StaminaProgressBar)
	{
		StaminaProgressBar->SetPercent(Percent);
	}
}

void USlashOverlap::SetGold(int32 Gold)
{
	if (GoldText)
	{
		const FString String = FString::Printf(TEXT("%d"), Gold);
		const FText Text = FText::FromString(String);
		GoldText->SetText(Text);
	}
}

void USlashOverlap::SetSouls(int32 Souls)
{
	if (SoulsText)
	{
		const FString String = FString::Printf(TEXT("%d"), Souls);
		const FText Text = FText::FromString(String);
		SoulsText->SetText(Text);
	}
}


