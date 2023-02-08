// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapon.h"
#include "MeleeWeapon.generated.h"

/**
 * 
 */
UCLASS()
class BATTLEARENA_API AMeleeWeapon : public AWeapon
{
	GENERATED_BODY()

	virtual void Interact_Implementation() override;
	
};
