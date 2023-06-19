// Fill out your copyright notice in the Description page of Project Settings.


#include "SMagicProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "SAttributeComponent.h"

ASMagicProjectile::ASMagicProjectile()
{
    CollisionComp->SetSphereRadius(20.f);
    InitialLifeSpan = 10.f;
    CollisionComp->IgnoreActorWhenMoving(GetInstigator(), true);
}

void ASMagicProjectile::OnActorBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    USAttributeComponent* AttributeComp = Cast<USAttributeComponent>(OtherActor->GetComponentByClass(USAttributeComponent::StaticClass()));
    if (AttributeComp)
    {
        Explode();
        AttributeComp->ApplyHealthChange(-20.f);
    }
}

void ASMagicProjectile::PostInitializeComponents()
{
    Super::PostInitializeComponents();

    CollisionComp->OnComponentBeginOverlap.AddDynamic(this, &ASMagicProjectile::OnActorBeginOverlap);
}
