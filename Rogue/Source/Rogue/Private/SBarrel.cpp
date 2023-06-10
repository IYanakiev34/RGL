// Fill out your copyright notice in the Description page of Project Settings.


#include "SBarrel.h"
#include "PhysicsEngine/RadialForceComponent.h"
#include "Components/StaticMeshComponent.h"
#include "DrawDebugHelpers.h"

// Sets default values
ASBarrel::ASBarrel()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	BarrelComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Barrel Comp"));
	RootComponent = BarrelComp;
	BarrelComp->SetSimulatePhysics(true);
	BarrelComp->SetCollisionProfileName("PhysicsActor");


	ForceComp = CreateDefaultSubobject<URadialForceComponent>(TEXT("Force Comp"));
	ForceComp->SetupAttachment(BarrelComp);
	ForceComp->Radius = 1000.f;
	ForceComp->ImpulseStrength = 10'000.f;
	ForceComp->bImpulseVelChange = true;
	ForceComp->AddCollisionChannelToAffect(ECC_WorldDynamic);
}

// Called when the game starts or when spawned
void ASBarrel::BeginPlay()
{
	Super::BeginPlay();
}

void ASBarrel::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	BarrelComp->OnComponentHit.AddDynamic(this, &ASBarrel::OnActorHit);
}

void ASBarrel::OnActorHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	UE_LOG(LogTemp,Warning,TEXT("We are here"));
	ForceComp->FireImpulse();

	// Logging
	UE_LOG(LogTemp, Log, TEXT("OnActorHit in Explosive Barrel C++"));
	UE_LOG(LogTemp, Warning, TEXT("OtherActor: %s, at game time: %f"), *GetNameSafe(OtherActor), GetWorld()->TimeSeconds);

	// Debug String Draw
	FString CombinedString = FString::Printf(TEXT("Hit at location: %s"), *Hit.ImpactPoint.ToString());
	DrawDebugString(GetWorld(), Hit.ImpactPoint, CombinedString, nullptr, FColor::Green, 2.f);
}

// Called every frame
void ASBarrel::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

