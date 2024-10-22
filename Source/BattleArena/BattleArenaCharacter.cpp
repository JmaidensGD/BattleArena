// Copyright Epic Games, Inc. All Rights Reserved.
// Copyright Epic Games, Inc. All Rights Reserved.

#include "BattleArenaCharacter.h"

#include "BattleArenaGameInstance.h"
#include "BattleArenaGameMode.h"
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
#include "MeleeWeapon.h"
#include "Algo/Rotate.h"
#include "Engine/DamageEvents.h"
#include "Engine/SkeletalMeshSocket.h"
#include "GameFramework/PlayerState.h"
#include "HAL/Platform.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

ABattleArenaCharacter::ABattleArenaCharacter()
{
	MaxWeapons = 2;
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
	
	//MeleeWeapon = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeleeWeapon"));
	//MeleeWeapon->SetupAttachment(GetMesh());
	// Create a follow camera

	
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	DeathCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("DeathCamera"));
	DeathCamera->SetupAttachment(RootComponent);
	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)
}

void ABattleArenaCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();
	
	if (IsLocallyControlled())
	{
		ServerSpawnWeapon();
	}

	//Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
		PlayerController->SetViewTarget(FollowCamera->GetOwner());
		Cast<ABattleArenaPlayerController>(PlayerController)->ClientHUDStateChanged(EHUDState::Playing);
	}
}

void ABattleArenaCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	if(Attacking && !Cooldown)
	{
		Attack();
	}
}

void ABattleArenaCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ABattleArenaCharacter,MaxHealth);
	DOREPLIFETIME(ABattleArenaCharacter,PlayerHealth);
	DOREPLIFETIME(ABattleArenaCharacter,EquippedIndex);
	DOREPLIFETIME(ABattleArenaCharacter,SpawnLocation);
	DOREPLIFETIME(ABattleArenaCharacter,EquippedWeapon);
	DOREPLIFETIME(ABattleArenaCharacter,Attacking);
	DOREPLIFETIME(ABattleArenaCharacter,Cooldown);
	DOREPLIFETIME(ABattleArenaCharacter,CanDoDamage);




}

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

		EnhancedInputComponent->BindAction(NextWeaponAction, ETriggerEvent::Completed, this, &ABattleArenaCharacter::NextWeapon);
		EnhancedInputComponent->BindAction(PreviousWeaponAction, ETriggerEvent::Completed, this, &ABattleArenaCharacter::PrevWeapon);

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
	const float actualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	PlayerHealth -= DamageAmount;
	UpdateUI();
	if(PlayerHealth<=0.0f)
	{
		UE_LOG(LogTemp, Warning, TEXT("DEAD"));
		//DEATH
		Die();
	}
	
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
	if (InventoryComponent->Weapons.Num()<MaxWeapons)
	{
		InventoryComponent->Weapons.Add(Weapon);
		UpdateInventory();
		WeaponActor->Destroy();
		if(EquippedWeapon==nullptr)
		{
			ServerSpawnWeapon();
		}
	}
}

void ABattleArenaCharacter::UpdateInventory_Implementation()
{
	PlayerUI->UpdateInventory();
}

void ABattleArenaCharacter::Attack()
{
	BPAttack();

	if(EquippedWeapon!=nullptr)
	{
		


		APlayerController* MyController = Cast<APlayerController>(Controller);
		if (MyController)
		{
			auto StartLocation = EquippedWeapon->WeaponMesh->GetSocketLocation("WeaponSocketA");
			auto  EndLocation = EquippedWeapon->WeaponMesh->GetSocketLocation("WeaponSocketB");
			FHitResult HitResult;
			FCollisionQueryParams QueryParams;
			QueryParams.AddIgnoredActor(this);
			QueryParams.bTraceComplex = true;
        
			GetWorld()->LineTraceSingleByChannel(HitResult,StartLocation,EndLocation, ECC_Camera,QueryParams);
			//DrawDebugLine(GetWorld(), StartLocation, EndLocation, HitResult.bBlockingHit ? FColor::Blue : FColor::Red, false, 5.0f, 0, 2.0f);
        		
			if (HitResult.GetActor() != nullptr)
			{
				if (HitResult.GetActor()->GetClass()->IsChildOf(ABattleArenaCharacter::StaticClass()))
				{
					UGameplayStatics::PlaySound2D(GetWorld(),Audio[2]);
					ServerAttack();
					Cooldown = true;
				}
		
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("NULL"));
			}
		}
		
	}
	else
	{
		//hardcoded attack forward, no weapon detection
		APlayerController* MyController = Cast<APlayerController>(Controller);
		if (MyController)
		{
			auto StartLocation = GetMesh()->GetBoneLocation(FName("Main_Spine1"));
			auto  EndLocation = StartLocation + FollowCamera->GetForwardVector() * 50.0f;
			FHitResult HitResult;
			FCollisionQueryParams QueryParams;
			QueryParams.AddIgnoredActor(this);
			QueryParams.bTraceComplex = true;

			GetWorld()->LineTraceSingleByChannel(HitResult,StartLocation,EndLocation, ECC_Camera,QueryParams);
			//DrawDebugLine(GetWorld(), StartLocation, EndLocation, HitResult.bBlockingHit ? FColor::Blue : FColor::Red, false, 5.0f, 0, 2.0f);

			if (HitResult.GetActor() != nullptr)
			{
				if (HitResult.GetActor()->ActorHasTag(TEXT("Destructible")))
				{
					HitResult.GetActor()->TakeDamage(1, FDamageEvent(), GetController(), this);
				}
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("NULL"));
			}
		}
	}
}

void ABattleArenaCharacter::ServerAttack_Implementation()
{
	if(GetLocalRole() == ROLE_Authority)
	{
		UE_LOG(LogTemp, Warning, TEXT("Server Stuff"));
		APlayerController* MyController = Cast<APlayerController>(Controller);
		if (MyController)
		{
			auto StartLocation = EquippedWeapon->WeaponMesh->GetSocketLocation("WeaponSocketA");
			auto  EndLocation = EquippedWeapon->WeaponMesh->GetSocketLocation("WeaponSocketB");
			FHitResult HitResult;
			FCollisionQueryParams QueryParams;
			QueryParams.AddIgnoredActor(this);
			QueryParams.bTraceComplex = true;

			GetWorld()->LineTraceSingleByChannel(HitResult,StartLocation,EndLocation, ECC_Camera,QueryParams);
			//MultiDebug(StartLocation,EndLocation,HitResult);
			
			if (HitResult.GetActor() != nullptr)
			{
				if (HitResult.GetActor()->GetClass()->IsChildOf(ABattleArenaCharacter::StaticClass()))
				{
					Cast<ABattleArenaCharacter>(HitResult.GetActor())->TakeDamage(EquippedWeapon->WeaponData->Damage,FDamageEvent(),GetController(),this);
				}
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("NULL"));
			}
		}

		
	}
}

void ABattleArenaCharacter::ServerSpawnWeapon_Implementation()
{
	if(InventoryComponent->Weapons.Num()>0)
	{
		FActorSpawnParameters SpawnParameters;
		SpawnParameters.bNoFail = true;
		SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		FVector Loc = GetMesh()->GetSocketLocation("WeaponSocket");
		FRotator Rot = GetMesh()->GetSocketRotation("WeaponSocket");
		EquippedWeapon = GetWorld()->SpawnActor<AWeapon>(WeaponClass, Loc, Rot, SpawnParameters);
		EquippedWeapon->SetOwner(this);
		EquippedWeapon->Interactable = false;
		//EquippedWeapon->AttachToActor(this,FAttachmentTransformRules::KeepWorldTransform,"WeaponSocket");
		EquippedWeapon->AttachToComponent(GetMesh(),FAttachmentTransformRules::SnapToTargetIncludingScale,"WeaponSocket");
		UpdateWeapon();
	}
}

bool ABattleArenaCharacter::ServerSpawnWeapon_Validate()
{
	return true;
}

void ABattleArenaCharacter::MultiDebug_Implementation(FVector StartLocation,FVector EndLocation,FHitResult HitResult)
{
	DrawDebugLine(GetWorld(), StartLocation, EndLocation, HitResult.bBlockingHit ? FColor::Blue : FColor::Red, false, 5.0f, 0, 2.0f);
}

bool ABattleArenaCharacter::ServerAttack_Validate()
{
	return true;
}

void ABattleArenaCharacter::Die()
{
	if(GetLocalRole() == ROLE_Authority)
	{
		ServerNotifyDeath();
		MultiDie();
		Spectate();
	}
}

void ABattleArenaCharacter::ServerNotifyDeath_Implementation()
{
	ABattleArenaPlayerController* PC = Cast<ABattleArenaPlayerController>(GetController());
	UWorld* World = GetWorld();
	ABattleArenaGameMode* GM = Cast<ABattleArenaGameMode>(UGameplayStatics::GetGameMode(World));
	if(PC)
	{
		UE_LOG(LogTemp, Warning, TEXT("Controller Valid"));
		UE_LOG(LogTemp, Warning, TEXT(" %d"), GetPlayerState()->GetPlayerId());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Controller Invalid"));
	}
	GM->PlayerDeath(GetPlayerState()->GetPlayerId());
}

void ABattleArenaCharacter::MultiDie_Implementation()
{
	this->GetCharacterMovement()->DisableMovement();
	this->SetActorEnableCollision(false);
	this->GetMesh()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	this->GetMesh()->SetAllBodiesSimulatePhysics(true);
	UGameplayStatics::PlaySoundAtLocation(GetWorld(),Audio[0],this->GetActorLocation());
}

void ABattleArenaCharacter::EquipWeapon(int32 WeaponIndex)
{
	EquippedIndex = WeaponIndex;
	UpdateWeapon();
}

void ABattleArenaCharacter::NextWeapon_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("Next"));
	if(InventoryComponent->Weapons.Num()>1)
	{
		int32 NewIndex = (EquippedIndex == MaxWeapons-1) ? 0 : EquippedIndex + 1;
		EquipWeapon(NewIndex);
		UE_LOG(LogTemp, Warning, TEXT("%s"),*FString::FromInt(EquippedIndex));
	}
}

void ABattleArenaCharacter::PrevWeapon_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("Prev"));
	if(InventoryComponent->Weapons.Num()>1)
	{
		int32 NewIndex = (EquippedIndex == 0) ? MaxWeapons-1 : EquippedIndex - 1;
		EquipWeapon(NewIndex);
		UE_LOG(LogTemp, Warning, TEXT("%s"),*FString::FromInt(EquippedIndex));
	}
}

void ABattleArenaCharacter::UpdateWeapon_Implementation()
{
	EquippedWeapon->SetupWeapon(InventoryComponent->Weapons[EquippedIndex]);
}

bool ABattleArenaCharacter::MultiDie_Validate()
{
	return true;
}

void ABattleArenaCharacter::LoadWeapons_Implementation()
{
	ABattleArenaPlayerController* PCON = Cast<ABattleArenaPlayerController>(GetController());
	UE_LOG(LogTemp, Warning, TEXT("Player ID : %s"),*FString::FromInt(GetPlayerState()->GetPlayerId()));
	TArray<UPDA_WeaponBase*> SavedWeapons = GetGameInstance<UBattleArenaGameInstance>()->GetWeapon(GetPlayerState()->GetPlayerId());
	for (UPDA_WeaponBase* SavedWeapon : SavedWeapons)
	{
		InventoryComponent->Weapons.Add(SavedWeapon);
	}
}

void ABattleArenaCharacter::LoadScores_Implementation()
{
	Cast<ABattleArenaGameState>(UGameplayStatics::GetGameState(GetWorld()))->Scores = GetGameInstance<UBattleArenaGameInstance>()->SortedScores;
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



