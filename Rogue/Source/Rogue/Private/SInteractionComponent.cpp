// Fill out your copyright notice in the Description page of Project Settings.


#include "SInteractionComponent.h"
#include "SGameplayInterface.h"

// Sets default values for this component's properties
USInteractionComponent::USInteractionComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

void USInteractionComponent::PrimaryInteract()
{
	FCollisionObjectQueryParams ObjectQueryParams;
	ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldDynamic);
	FVector StartPoint;
	FRotator StartRotation;
	GetOwner()->GetActorEyesViewPoint(StartPoint, StartRotation);
	
	FVector const End = StartPoint + (600.f * StartRotation.Vector());

	// Line trace example
	//FHitResult Hit{};
	//GetWorld()->LineTraceSingleByObjectType(Hit, StartPoint, End, ObjectQueryParams);

	float const Thickness = 3.f;
	float Radius = 30.f;
	FCollisionShape Shape;
	Shape.SetSphere(Radius);

	TArray<FHitResult> HitResults;
	bool bBlockingHit = GetWorld()->SweepMultiByObjectType(HitResults, StartPoint, End, FQuat::Identity, ObjectQueryParams, Shape);

	FColor const LineColor = bBlockingHit ? FColor::Green : FColor::Red;

	DrawDebugLine(GetWorld(), StartPoint, End, LineColor, false, 2.f, 0, Thickness);
	for (FHitResult const& Hit : HitResults)
	{
		AActor* HitActor = Hit.GetActor();
		if (HitActor)
		{
			bool bIsInteractable = HitActor->Implements<USGameplayInterface>();
			if (bIsInteractable)
			{
				ISGameplayInterface::Execute_Interact(HitActor, Cast<APawn>(GetOwner()));
				return; // We do not want to interact with multiple actors only with the closest one
			}
		}
	}
}


// Called when the game starts
void USInteractionComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void USInteractionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

