// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enums.h"
#include "Engine/DataTable.h"
#include "WeaponStats.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct FWeaponStats : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadWrite)
	EWeaponType WeaponType;

	UPROPERTY(BlueprintReadWrite)
	USkeletalMeshComponent* WeaponMesh;

	UPROPERTY(BlueprintReadWrite)
	float Damage;

	UPROPERTY(BlueprintReadWrite)
	bool bIsMelee;
};