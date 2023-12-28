// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/WorldUserWidget.h"
#include "Kismet/GameplayStatics.h"
#include <Blueprint/WidgetLayoutLibrary.h>
void UWorldUserWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (!IsValid(AttachedActor))
	{
		RemoveFromParent();

		return;
	}
	FVector2d ScreenPosition;
	if (UGameplayStatics::ProjectWorldToScreen(GetOwningPlayer(), AttachedActor->GetActorLocation() + WorldOffset, ScreenPosition))
	{
		float Scale = UWidgetLayoutLibrary::GetViewportScale(this);
		ScreenPosition /= Scale;
		if (ParentSizeBox)
		{
			ParentSizeBox->SetRenderTranslation(ScreenPosition);
		}
	}

}
