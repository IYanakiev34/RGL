// Fill out your copyright notice in the Description page of Project Settings.


#include "SDummyTarget.h"
#include "SAttributeComponent.h"

// Sets default values
ASDummyTarget::ASDummyTarget()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh Component"));
	RootComponent = MeshComp;

	AttributeComp = CreateDefaultSubobject<USAttributeComponent>(TEXT("Attribute Component"));
	AttributeComp->OnHealthChanged.AddDynamic(this, &ASDummyTarget::OnHealthChanged);
}

// Called when the game starts or when spawned
void ASDummyTarget::BeginPlay()
{
	Super::BeginPlay();
}

void ASDummyTarget::OnHealthChanged(AActor* InstigatorActor, USAttributeComponent* OwningComp, float NewHealth, float Delta)
{
	if (Delta < 0.f)
	{
		MeshComp->SetScalarParameterValueOnMaterials("TimeToHit", GetWorld()->GetTimeSeconds());
	}
}

// Called every frame
void ASDummyTarget::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

