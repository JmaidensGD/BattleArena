// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enums.h"
#include "Interactable.h"
#include "PDA_WeaponBase.h"
#include "WeaponStats.h"
#include "Engine/DataTable.h"
#include "GameFramework/Actor.h"
#include "Weapon.generated.h"

UCLASS()
class BATTLEARENA_API AWeapon : public AActor, public IInteractable
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWeapon();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


public:
	UPROPERTY(BlueprintReadWrite,EditAnywhere,Replicated)
	USkeletalMeshComponent* WeaponMesh;

	UPROPERTY()
	float DamageAmount;

	UPROPERTY(BlueprintReadWrite,EditAnywhere)
	bool Interactable;

	UPROPERTY(BlueprintReadWrite,EditAnywhere,Replicated)
	UPDA_WeaponBase* WeaponData;

	virtual void Attack();

	virtual void StopAttack();
	
	UFUNCTION(BlueprintImplementableEvent)
	void SetupWeapon(UPDA_WeaponBase* Data);

	
	virtual void Interact_Implementation(ABattleArenaCharacter* Player) override;

	virtual bool CanInteract_Implementation() override;

};
