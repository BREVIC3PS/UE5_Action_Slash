// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AttributeComponents.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SLASH_API UAttributeComponents : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UAttributeComponents();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	// Current Health
	UPROPERTY(EditAnywhere,Category = "Actor Attributes")
	float Health = 100.f;

	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	float MaxHealth = 100.f;
public:
	void ReceiveDamage(float Damage);

	float GetHealthPercent();
		
	bool IsAlive();
};
