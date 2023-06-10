// Fill out your copyright notice in the Description page of Project Settings.


#include "SCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"


// DEBUG
#include "DrawDebugHelpers.h"

// Sets default values
ASCharacter::ASCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;

	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera Component"));
	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("Spring Arm Component"));
	
	SpringArmComp->SetupAttachment(RootComponent);
	CameraComp->SetupAttachment(SpringArmComp);

	// Spring arm component
	SpringArmComp->bVisualizeComponent = true;
	SpringArmComp->TargetOffset = FVector{ 0.f,-30.f,60.f };
}

// Called when the game starts or when spawned
void ASCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

////////////////////////// MOVEMENT /////////////////////////////////
void ASCharacter::MoveForward(float Value)
{
	FRotator Rotation = GetControlRotation();
	Rotation.Pitch = 0.f;
	Rotation.Roll = 0.f;

	AddMovementInput(Rotation.Vector(), Value);
}

void ASCharacter::MoveRight(float Value)
{
	FRotator Forward = GetControlRotation(); // Forward
	Forward.Pitch = 0.f;
	Forward.Roll = 0.f;

	FVector const Right { FRotationMatrix(Forward).GetScaledAxis(EAxis::Y) };

	AddMovementInput(Right, Value);
}
////////////////////////// END MOVEMENT /////////////////////////////

void ASCharacter::PrimaryAttack()
{
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = this;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	FTransform SpawnTM = GetTransform();
	GetWorld()->SpawnActor<AActor>(ProjectileClass, SpawnTM, SpawnParams);
}


// Called every frame
void ASCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	////////////////////////////// DEBUG /////////////////////////////
	float const DrawScale = 100.f;
	float const Thickness = 5.f;

	FVector const LineStart = GetActorLocation() + GetActorRightVector() * 50.f;

	FVector const ActorLineEnd = LineStart + (GetActorForwardVector() * 100.f);
	FVector const ControllerLineEnd = LineStart + (GetControlRotation().Vector() * 100.f);

	FColor const ActorColor{ FColor::Yellow };
	FColor const ControllerColor{ FColor::Green };


	// Actor Rotation Arrow
	DrawDebugDirectionalArrow(GetWorld(), LineStart, ActorLineEnd, DrawScale, ActorColor, false, 0.f, 0, Thickness);

	// Controller Rotation Arrow
	DrawDebugDirectionalArrow(GetWorld(), LineStart, ControllerLineEnd, DrawScale, ControllerColor, false, 0.f, 0, Thickness);
}

// Called to bind functionality to input
void ASCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Movement
	PlayerInputComponent->BindAxis("Forward", this, &ASCharacter::MoveForward);
	PlayerInputComponent->BindAxis("Right", this, &ASCharacter::MoveRight);

	// Rotation
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);

	// Actions
	PlayerInputComponent->BindAction("PrimaryAttack", IE_Pressed, this, &ASCharacter::PrimaryAttack);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);

}

