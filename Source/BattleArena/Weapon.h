// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enums.h"
#include "WeaponStats.h"
#include "Engine/DataTable.h"
#include "GameFramework/Actor.h"
#include "Weapon.generated.h"

UCLASS()
class BATTLEARENA_API AWeapon : public AActor
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

	UPROPERTY(BlueprintReadWrite)
	EWeaponType WeaponType;

	UPROPERTY(EditAnywhere)
	UDataTable* WeaponStatsTable;

	FWeaponStats* WeaponRow;

public:	

	virtual void Attack();

	virtual void StopAttack();

	UFUNCTION()
	void SetupWeapon();

};
