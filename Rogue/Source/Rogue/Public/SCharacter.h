// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SCharacter.generated.h"

class UCameraComponent;
class UCapsuleComponent;
class USpringArmComponent;

UCLASS()
class ROGUE_API ASCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ASCharacter();



protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


protected:
	void MoveForward(float Value);
	void MoveRight(float Value);

private:
	void PrimaryAttack();
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	UPROPERTY(EditAnywhere)
	UCameraComponent* CameraComp;
	UPROPERTY(EditAnywhere)
	USpringArmComponent* SpringArmComp;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AActor> ProjectileClass;
};
