// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "InputMappingContext.h"
#include "PlayerUI.h"
#include "Weapon.h"
#include "BattleArenaCharacter.generated.h"

UCLASS(config=Game)
class ABattleArenaCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Weapon, meta = (AllowPrivateAccess = "true"))
	class UInventoryComponent* InventoryComponent;
	
	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* JumpAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* AttackAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* InteractAction;
	
	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* SpectateAction;

	

public:
	ABattleArenaCharacter();
	
	UPROPERTY(VisibleAnywhere,BlueprintReadWrite,Replicated)
    float PlayerHealth;
    UPROPERTY(VisibleAnywhere,BlueprintReadWrite,Replicated)
    float MaxHealth;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
	UPlayerUI* PlayerUI;

	UFUNCTION(Client,Reliable)
	void UpdateUI();

	UFUNCTION(Client,Reliable)
	void UpdateInventory();
	
	UFUNCTION(Server,Reliable)
	void PickupWeapon(UPDA_WeaponBase* Weapon, AWeapon* WeaponActor);

	UFUNCTION(BlueprintImplementableEvent,BlueprintCallable)
	void RoundEnd(int Winner);
	
protected:

	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

	UFUNCTION(Server,Reliable)
	void Spectate();

	void Interact();

	void Attack();

	UFUNCTION(Server,Reliable,WithValidation)
	void ServerAttack();
	bool ServerAttack_Validate();
	void ServerAttack_Implementation();

	UFUNCTION(NetMulticast,Reliable)
	void MultiDebug(FVector StartLocation,FVector EndLocation,FHitResult HitResult);

	void Die();

	UFUNCTION(NetMulticast,Reliable,WithValidation)
	void MultiDie();
	bool MultiDie_Validate();
	void MultiDie_Implementation();
	
protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
	// To add mapping context
	virtual void BeginPlay();

	virtual void Tick(float DeltaSeconds) override;

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
};

