// Fill out your copyright notice in the Description page of Project Settings.


#include "SCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Components/InputComponent.h"

#include "SInteractionComponent.h"
#include "SAttributeComponent.h"

// DEBUG
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"

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

	InteractionComponent = CreateDefaultSubobject<USInteractionComponent>(TEXT("Interaction Component"));
	AttributesComponent = CreateDefaultSubobject<USAttributeComponent>(TEXT("Attribute Component"));

	// Spring arm component
	SpringArmComp->bVisualizeComponent = true;
	SpringArmComp->TargetOffset = FVector{ 0.f,-30.f,60.f };

	GetCharacterMovement()->bOrientRotationToMovement = true;
	bUseControllerRotationYaw = false;

}

// Called when the game starts or when spawned
void ASCharacter::BeginPlay()
{
	Super::BeginPlay();

	APlayerController* PlayerController = Cast<APlayerController>(Controller);
	if (PlayerController)
	{
		if (UEnhancedInputLocalPlayerSubsystem* InputSystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			if (CharacterMapping)
			{
				InputSystem->AddMappingContext(CharacterMapping, 0);
			}
		}
	}
}

void ASCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	AttributesComponent->OnHealthChanged.AddDynamic(this, &ASCharacter::OnHealthChanged);
}

void ASCharacter::OnHealthChanged(AActor* InstigatorActor, USAttributeComponent* OwningComp, float NewHealth, float Delta)
{
	if (NewHealth <= 0.f && Delta < 0.f)
	{
		APlayerController* PC = Cast<APlayerController>(GetController());
		DisableInput(PC);
	}

	if (Delta < 0.f)
	{
		GetMesh()->SetScalarParameterValueOnMaterials("TimeToHit", GetWorld()->GetTimeSeconds());
	}
}

////////////////////////// MOVEMENT /////////////////////////////////

void ASCharacter::Move(FInputActionValue const& Value)
{
	FVector2D const movement = Value.Get<FVector2D>();
	{
		FRotator Rotation = GetControlRotation();
		Rotation.Pitch = 0.f;
		Rotation.Roll = 0.f;
		if (Controller && movement.X != 0.f) // Handles forward backward
		{
			AddMovementInput(Rotation.Vector(), movement.X);
			return;
		}

		if (Controller && movement.Y != 0.f) // Handles left right
		{
			FVector const Right{ FRotationMatrix(Rotation).GetScaledAxis(EAxis::Y) };
			AddMovementInput(Right, movement.Y);
			return;
		}
	}
}
void ASCharacter::Look(FInputActionValue const& Value)
{
	FVector2D const Mouse = Value.Get<FVector2D>();
	AddControllerPitchInput(Mouse.Y);
	AddControllerYawInput(Mouse.X);
}
////////////////////////// END MOVEMENT /////////////////////////////


// Primary Attack ///////////////////////////////////////////////
void ASCharacter::PrimaryAttack(FInputActionValue const& Value)
{
	PlayAnimMontage(AttackAnim);
	GetWorldTimerManager().SetTimer(TimeHandle_Attack, this, &ASCharacter::PrimaryAttack_TimeElapsed, 0.25f);
}

void ASCharacter::PrimaryAttack_TimeElapsed()
{
	SpawnProjectileFromClass(ProjectileClass);
}
//////////////////////////// End Primary Attack//////////////////


void ASCharacter::PrimaryInteract(FInputActionValue const& Value)
{
	if (InteractionComponent)
	{
		InteractionComponent->PrimaryInteract();
	}
}

// Ultimate Attack ////////////////////////////////////////////////
void ASCharacter::UltimateAttack(FInputActionValue const& Value)
{
	PlayAnimMontage(AttackAnim);
	GetWorldTimerManager().SetTimer(TimeHandle_Attack, this, &ASCharacter::UltimateAttack_Elapsed, 0.25f);
}

void ASCharacter::UltimateAttack_Elapsed()
{
	SpawnProjectileFromClass(UltimateAttackClass);
}
//////////////////////// End Ultimate Attack ////////////////////


// Dash Attack //////////////////////////////////////////////////
void ASCharacter::DashAttack(FInputActionValue const& Value)
{
	PlayAnimMontage(AttackAnim);
	GetWorldTimerManager().SetTimer(TimeHandle_Attack, this, &ASCharacter::DashAttack_Elapsed, 0.25f);
}

void ASCharacter::DashAttack_Elapsed()
{
	SpawnProjectileFromClass(DashAttackClass);
}
/////////////////////// End Dash Attack //////////////////////////

void ASCharacter::SpawnProjectileFromClass(TSubclassOf<AActor> Clazz)
{
	FVector HandLocation{ GetMesh()->GetSocketLocation("Muzzle_01") };
	UGameplayStatics::SpawnEmitterAttached(CastingEffect, GetMesh(), "Muzzle_01");

	// Spawn Actor params
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = this;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	// Collision Query
	FCollisionQueryParams QueryParams; // Ignore spawner
	QueryParams.AddIgnoredActor(this);

	// Shape
	FCollisionShape Shape;
	Shape.SetSphere(20.f);

	// ObjectParams for querying types
	FCollisionObjectQueryParams ObjectParams; // Add types to query
	ObjectParams.AddObjectTypesToQuery(ECC_WorldDynamic);
	ObjectParams.AddObjectTypesToQuery(ECC_WorldDynamic);
	ObjectParams.AddObjectTypesToQuery(ECC_Pawn);

	// Sweep by camera
	APlayerController* PC = Cast<APlayerController>(GetController());
	FVector CameraLocation = PC->PlayerCameraManager->GetCameraLocation();
	FVector CameraForward = PC->PlayerCameraManager->GetActorForwardVector();

	FVector TraceStart{ CameraLocation };
	FVector TraceEnd{ TraceStart + (CameraForward * 5000.f) };

	FHitResult Hit; // Make sure we have a hit and update the end
	if (GetWorld()->SweepSingleByObjectType(Hit, TraceStart, TraceEnd, FQuat::Identity, ObjectParams, Shape, QueryParams))
	{
		TraceEnd = Hit.ImpactPoint; // We have hit based on camera trace
	}

	// Get the rotation needed
	FRotator ProjRotation = FRotationMatrix::MakeFromX(TraceEnd - HandLocation).Rotator();

	// Spawn
	FTransform const SpawnTM = FTransform(ProjRotation, HandLocation);
	GetWorld()->SpawnActor<AActor>(Clazz, SpawnTM, SpawnParams);
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

	// Cast to enhanced input component and attach proper actions
	// Actions should be checked for input type and then do something with it
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ASCharacter::Move);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(PrimaryAttackAction, ETriggerEvent::Triggered, this, &ASCharacter::PrimaryAttack);
		EnhancedInputComponent->BindAction(InteractionAction, ETriggerEvent::Triggered, this, &ASCharacter::PrimaryInteract);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ASCharacter::Look);
		EnhancedInputComponent->BindAction(UltimateAttackAction, ETriggerEvent::Triggered, this, &ASCharacter::UltimateAttack);
		EnhancedInputComponent->BindAction(DashAttackAction, ETriggerEvent::Triggered, this, &ASCharacter::DashAttack);
	}

}

