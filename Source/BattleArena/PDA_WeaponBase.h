// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "PDA_WeaponBase.generated.h"

/**
 * 
 */
UCLASS()
class BATTLEARENA_API UPDA_WeaponBase : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	FString Name;

	UPROPERTY(EditAnywhere)
	USkeletalMesh* Mesh;

	UPROPERTY(EditAnywhere)
	float Damage;
	
};
