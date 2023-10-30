// Fill out your copyright notice in the Description page of Project Settings.


#include "MyItem.h"
#include "SlashCharacter.h"
#include "Components/SphereComponent.h"

// Sets default values
AMyItem::AMyItem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ItemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemMeshComponent"));
	RootComponent = ItemMesh;

	Sphere = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere"));
	Sphere->SetupAttachment(GetRootComponent());

}

float AMyItem::TransformedSin()
{
	return Amplitude * FMath::Sin(RunningTime * TimeConstant);
}

float AMyItem::TransformedCos()
{
	return Amplitude * FMath::Cos(RunningTime * TimeConstant);
}

// Called when the game starts or when spawned
void AMyItem::BeginPlay()
{
	Super::BeginPlay();

	Sphere->OnComponentBeginOverlap.AddDynamic(this, &AMyItem::OnSphereOverlap);
	Sphere->OnComponentEndOverlap.AddDynamic(this, &AMyItem::OnSphereEndOverlap);
	
}

void AMyItem::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	const FString OtherActorName = OtherActor->GetName();
	if (GEngine) {
		GEngine->AddOnScreenDebugMessage(1, 30.f, FColor::Red, OtherActorName);
	}

	ASlashCharacter* SlashCharater = Cast<ASlashCharacter>(OtherActor);
	if (SlashCharater) {
		SlashCharater->SetOverlappingItem(this);
	}

}

void AMyItem::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	const FString OtherActorName = FString("Ending Overlap with: ") + OtherActor->GetName();
	if (GEngine) {
		GEngine->AddOnScreenDebugMessage(1, 30.f, FColor::Red, OtherActorName);
	}

	ASlashCharacter* SlashCharater = Cast<ASlashCharacter>(OtherActor);
	if (SlashCharater) {
		SlashCharater->SetOverlappingItem(nullptr);	
	}
}
// Called every frame
void AMyItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	RunningTime += DeltaTime;

	if(ItemState==EItemState::EIS_Hovering)
	AddActorLocalOffset(FVector(0.f, 0.f, TransformedSin()));
}

