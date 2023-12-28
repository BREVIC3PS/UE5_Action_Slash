// Fill out your copyright notice in the Description page of Project Settings.


#include "MyItem.h"
#include "Interfaces/PickupInterface.h"
#include "Components/SphereComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AMyItem::AMyItem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ItemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemMeshComponent"));
	ItemMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RootComponent = ItemMesh;

	Sphere = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere"));
	Sphere->SetupAttachment(GetRootComponent());

	ItemEffect = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Embers"));
	ItemEffect->SetupAttachment(GetRootComponent());
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

	IPickupInterface* PickupInterface = Cast<IPickupInterface>(OtherActor);
	if (PickupInterface) {
		PickupInterface->SetOverlappingItem(this);
	}
	if (PickupEffect)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, PickupEffect, GetActorLocation());
	}

}

void AMyItem::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	IPickupInterface* PickupInterface = Cast<IPickupInterface>(OtherActor);
	if (PickupInterface) {
		PickupInterface->SetOverlappingItem(nullptr);
	}
}
void AMyItem::SpawnPickupSound()
{
	if (PickupSound)
	{
		UGameplayStatics::SpawnSoundAtLocation(this, PickupSound, GetActorLocation());
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

