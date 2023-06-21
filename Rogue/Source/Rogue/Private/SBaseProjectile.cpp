// Fill out your copyright notice in the Description page of Project Settings.


#include "SBaseProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/AudioComponent.h"
#include "Sound/SoundCue.h"

// Sets default values
ASBaseProjectile::ASBaseProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("Collision Component"));
	CollisionComp->SetCollisionProfileName("Projectile");
	RootComponent = CollisionComp;

	EffectComp = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Effect Component"));
	EffectComp->SetupAttachment(CollisionComp);

	FlightAudioComp = CreateDefaultSubobject<UAudioComponent>(TEXT("Flight Audio Component"));
	FlightAudioComp->SetupAttachment(CollisionComp);
	FlightAudioComp->bStopWhenOwnerDestroyed = true;

	MovementComp = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Projectile Movement"));
	MovementComp->InitialSpeed = 4000.f;
	MovementComp->ProjectileGravityScale = 0.f;
	MovementComp->bRotationFollowsVelocity = true;
	MovementComp->bInitialVelocityInLocalSpace = true;

	ImpactShakeInnnerRadius = 0.f;
	ImpactShakeOuterRadius = 1500.f;
}

// Called when the game starts or when spawned
void ASBaseProjectile::BeginPlay()
{
	Super::BeginPlay();
}

void ASBaseProjectile::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	CollisionComp->OnComponentHit.AddDynamic(this, &ASBaseProjectile::OnActorHit);
}

void ASBaseProjectile::OnActorHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
    Explode();
}

void ASBaseProjectile::Explode_Implementation()
{
    if (ensure(IsValid(this)))
    {
        UGameplayStatics::SpawnEmitterAtLocation(this, ImpactEffect, GetActorLocation(), GetActorRotation());

		if (ImpactSound)
		{
			UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation());
		}

		UGameplayStatics::PlayWorldCameraShake(this, ImpactCameraShakeClazz, GetActorLocation(), ImpactShakeInnnerRadius, ImpactShakeOuterRadius);
		Destroy();
    }
}
