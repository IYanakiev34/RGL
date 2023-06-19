// Fill out your copyright notice in the Description page of Project Settings.

#include "SDashProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"

ASDashProjectile::ASDashProjectile()
{
    TeleportDelay = 0.2f;
    DetonationDelay = 0.2f;
    MovementComp->InitialSpeed = 6000.f;
}

void ASDashProjectile::Explode_Implementation()
{
    GetWorldTimerManager().ClearTimer(TimerHandle_DelayedDetonate);

    UGameplayStatics::SpawnEmitterAtLocation(this, ImpactEffect, GetActorLocation(), GetActorRotation());
    EffectComp->DeactivateSystem();
    MovementComp->StopMovementImmediately();
    SetActorEnableCollision(false);

    FTimerHandle TimerHandle_DelayedTeleport;
    GetWorldTimerManager().SetTimer(TimerHandle_DelayedTeleport, this, &ASDashProjectile::TeleportInstigator, TeleportDelay);
}

void ASDashProjectile::TeleportInstigator()
{
    AActor* ActorToTeleport = GetInstigator();
    if (ensure(ActorToTeleport))
    {
        ActorToTeleport->TeleportTo(GetActorLocation(), ActorToTeleport->GetActorRotation());
    }

    Destroy();
}

void ASDashProjectile::BeginPlay()
{
    Super::BeginPlay();

    GetWorldTimerManager().SetTimer(TimerHandle_DelayedDetonate, this, &ASDashProjectile::Explode, DetonationDelay);
}
