// Copyright Epic Games, Inc. All Rights Reserved.

#include "BattleArenaCharacter.h"

#include "BattleArenaGameState.h"
#include "BattleArenaPlayerController.h"
#include "BattleArenaPlayerState.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Interactable.h"
#include "InventoryComponent.h"
#include "GameFramework/PlayerState.h"
#include "HAL/Platform.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"


//////////////////////////////////////////////////////////////////////////
// ABattleArenaCharacter

ABattleArenaCharacter::ABattleArenaCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetMesh());
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	InventoryComponent = CreateDefaultSubobject<UInventoryComponent>(TEXT("Inventory"));

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm
	
	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)
}

void ABattleArenaCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	//Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}

}

void ABattleArenaCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

void ABattleArenaCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ABattleArenaCharacter,MaxHealth);
	DOREPLIFETIME(ABattleArenaCharacter,PlayerHealth);
}

//////////////////////////////////////////////////////////////////////////
// Input

void ABattleArenaCharacter::Interact()
{
	UE_LOG(LogTemp, Warning, TEXT("PRESSED!"));
	APlayerController* MyController = Cast<APlayerController>(Controller);
	if (MyController)
	{
		APlayerCameraManager* MyCameraManager = MyController->PlayerCameraManager;
		auto StartLocation = MyCameraManager->GetCameraLocation();
		auto  EndLocation = MyCameraManager->GetCameraLocation() + (MyCameraManager->GetActorForwardVector() * 100);
		FHitResult HitResult;
		GetWorld()->SweepSingleByObjectType(HitResult, StartLocation, EndLocation, FQuat::Identity, 
		FCollisionObjectQueryParams(FCollisionObjectQueryParams::AllObjects),FCollisionShape::MakeSphere(25),
		FCollisionQueryParams(FName("Interaction"),true,this));
		if (HitResult.GetActor() != nullptr)
		{
			if (HitResult.GetActor()->GetClass()->ImplementsInterface(UInteractable::StaticClass()))
			{
				if (IInteractable::Execute_CanInteract(HitResult.GetActor()))
				{
					IInteractable::Execute_Interact(HitResult.GetActor(), this);
				}
			}
		}
	}
}


void ABattleArenaCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		//Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Completed, this, &ABattleArenaCharacter::Interact);

		EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Completed, this, &ABattleArenaCharacter::Attack);

		EnhancedInputComponent->BindAction(SpectateAction, ETriggerEvent::Triggered, this, &ABattleArenaCharacter::Spectate);
		//Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ABattleArenaCharacter::Move);

		//Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ABattleArenaCharacter::Look);

	}

}

float ABattleArenaCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent,
	AController* EventInstigator, AActor* DamageCauser)
{
	PlayerHealth -= DamageAmount;
	UE_LOG(LogTemp, Warning, TEXT("taken %s damage"), *FString::FromInt(DamageAmount));
	return DamageAmount;
}

void ABattleArenaCharacter::UpdateUI_Implementation()
{
	PlayerUI->UpdateUI();
}

void ABattleArenaCharacter::PickupWeapon_Implementation(UPDA_WeaponBase* Weapon, AWeapon* WeaponActor)
{
	UE_LOG(LogTemp, Warning, TEXT("PICKUP SERVER"));
    InventoryComponent->Weapons.Add(Weapon);
	UpdateInventory();
	WeaponActor->Destroy();
	UE_LOG(LogTemp, Warning, TEXT("Damage : %s"), *FString::SanitizeFloat(InventoryComponent->Weapons[0]->Damage));
	//InventoryComponent->Weapons.Add(Weapon);
}

void ABattleArenaCharacter::UpdateInventory_Implementation()
{
	PlayerUI->UpdateInventory();
}

void ABattleArenaCharacter::Attack_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("Client Stuff"));
	AttackRPC_Implementation();
}

void ABattleArenaCharacter::AttackRPC_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("Server Stuff"));
	APlayerController* MyController = Cast<APlayerController>(Controller);
	if (MyController)
	{
		APlayerCameraManager* MyCameraManager = MyController->PlayerCameraManager;
		auto StartLocation = MyCameraManager->GetCameraLocation();
		auto  EndLocation = MyCameraManager->GetCameraLocation() + (MyCameraManager->GetActorForwardVector() * 100);
		FHitResult HitResult;
		GetWorld()->SweepSingleByObjectType(HitResult, StartLocation, EndLocation, FQuat::Identity, 
		FCollisionObjectQueryParams(FCollisionObjectQueryParams::AllObjects),FCollisionShape::MakeSphere(25),
		FCollisionQueryParams(FName("Attack"),true,this));
		if (HitResult.GetActor() != nullptr)
		{
			if (HitResult.GetActor()->GetClass()->IsChildOf(ABattleArenaCharacter::StaticClass()))
			{
				UGameplayStatics::ApplyDamage(HitResult.GetActor(),25,GetController(),this,UDamageType::StaticClass());
				UE_LOG(LogTemp, Warning, TEXT("Damage Applied"));
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("NULL"));
		}
	}
}

void ABattleArenaCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	
		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void ABattleArenaCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void ABattleArenaCharacter::Spectate_Implementation()
{
	if(ABattleArenaPlayerController* PC = GetController<ABattleArenaPlayerController>())
	{
		PC->SetPlayerSpectate();
	}
}



