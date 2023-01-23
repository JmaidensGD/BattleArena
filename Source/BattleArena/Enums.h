#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class EWeaponState : uint8
{
	WEAPONSTATE_Equipping UMETA(DisplayName = "Equipping"),
	WEAPONSTATE_Attacking UMETA(DisplayName = "Attacking"),
	WEAPONSTATE_ReadyAttack UMETA(DisplayName = "ReadyAttack"),
	WEAPONSTATE_Idle UMETA(DisplayName = "Idle"),
};

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	NONE UMETA(DisplayName = "None"),
	SWORD UMETA(DisplayName = "Sword"),
	DAGGER UMETA(DisplayName = "Dagger"),
	AXE UMETA(DisplayName = "Axe"),
};