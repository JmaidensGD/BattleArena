// Copyright Epic Games, Inc. All Rights Reserved.

#include "BattleArenaGameMode.h"
#include "BattleArenaCharacter.h"
#include "UObject/ConstructorHelpers.h"

ABattleArenaGameMode::ABattleArenaGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
