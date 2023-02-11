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

	UPROPERTY(BlueprintReadWrite,EditAnywhere)
	USkeletalMeshComponent* WeaponMesh;

	UPROPERTY()
	float DamageAmount;

	UPROPERTY(EditAnywhere)
	UPDA_WeaponBase* WeaponData;

public:	

	virtual void Attack();

	virtual void StopAttack();

	virtual void SetupWeapon(UPDA_WeaponBase* WeaponData);

	virtual void Interact_Implementation(ABattleArenaCharacter* Player) override;

	virtual bool CanInteract_Implementation() override;

};
