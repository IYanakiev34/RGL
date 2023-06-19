// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SBaseProjectile.h"
#include "SDashProjectile.generated.h"

/**
 * 
 */
UCLASS()
class ROGUE_API ASDashProjectile : public ASBaseProjectile
{
	GENERATED_BODY()

public:
	ASDashProjectile();
protected:
	UPROPERTY(EditDefaultsOnly, Category = "Delays")
	float TeleportDelay;

	UPROPERTY(EditDefaultsOnly, Category = "Delays")
	float DetonationDelay;

	FTimerHandle TimerHandle_DelayedDetonate;
	virtual void Explode_Implementation() override;

	void TeleportInstigator();

	virtual void BeginPlay() override;
};
