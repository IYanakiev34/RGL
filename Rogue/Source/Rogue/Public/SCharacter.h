// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "SBaseProjectile.h"
#include "SCharacter.generated.h"

class UCameraComponent;
class UCapsuleComponent;
class USpringArmComponent;
class USInteractionComponent;
class UInputMappingContext;
class UInputAction;
class USAttributeComponent;

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
	virtual void PostInitializeComponents() override;

	UFUNCTION()
	void OnHealthChanged(AActor* InstigatorActor, USAttributeComponent* OwningComp, float NewHealth, float Delta);

private:
	void Move(FInputActionValue const& Value);
	void Look(FInputActionValue const& Value);
	void PrimaryAttack(FInputActionValue const& Value);
	void PrimaryAttack_TimeElapsed();
	void PrimaryInteract(FInputActionValue const& Value);
	void UltimateAttack(FInputActionValue const& Value);
	void UltimateAttack_Elapsed();
	void DashAttack(FInputActionValue const& Value);
	void DashAttack_Elapsed();


	void SpawnProjectileFromClass(TSubclassOf<AActor> Clazz);
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:

	// Character Components ///////////////////////////////////////////
	UPROPERTY(EditAnywhere, Category = "Components")
	UCameraComponent* CameraComp;

	UPROPERTY(EditAnywhere, Category = "Components")
	USpringArmComponent* SpringArmComp;

	UPROPERTY(EditDefaultsOnly, Category = "Attack")
	TSubclassOf<ASBaseProjectile> ProjectileClass;

	UPROPERTY(EditDefaultsOnly, Category = "Attack")
	TSubclassOf<ASBaseProjectile> UltimateAttackClass;

	UPROPERTY(EditDefaultsOnly, Category = "Attack")
	TSubclassOf<ASBaseProjectile> DashAttackClass;

	UPROPERTY(EditDefaultsOnly, Category = "Components")
	USInteractionComponent* InteractionComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components")
	USAttributeComponent* AttributesComponent;
	///////////////////////////////////////////////////////////////////

	// Times /////////////////////////////////////////////////////////
	FTimerHandle TimeHandle_Attack; 


	// Animations ////////////////////////////////////////////////////
	UPROPERTY(EditAnywhere, Category = Attack)
	UAnimMontage* AttackAnim;

	UPROPERTY(EditDefaultsOnly, Category = "Attack")
	UParticleSystem* CastingEffect;

	// Movement Properties ////////////////////////////////////////////
	// The character mapping context that will be used for its input and actions
	UPROPERTY(EditAnywhere, Category = "Input Mapping")
	UInputMappingContext* CharacterMapping;

	// Movement actions in all 4 directions
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input Mapping")
	UInputAction* MoveAction;

	// Handles jumping
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input Mapping")
	UInputAction* JumpAction;

	// Handles the primary attack of the character
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input Mapping")
	UInputAction* PrimaryAttackAction;

	// Handles interaction with other objects such as chest, items, etc
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input Mapping")
	UInputAction* InteractionAction;

	// Handles camera looking via 2D Axis
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input Mapping")
	UInputAction* LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input Mapping")
	UInputAction* UltimateAttackAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input Mapping")
	UInputAction* DashAttackAction;

	////////////////////////////////////////////////////////////////////
};
