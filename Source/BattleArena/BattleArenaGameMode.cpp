// Copyright Epic Games, Inc. All Rights Reserved.

#include "BattleArenaGameMode.h"
#include "BattleArenaCharacter.h"
#include "BattleArenaGameInstance.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"
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

AActor* ABattleArenaGameMode::ChoosePlayerStart_Implementation(AController* Player)
{
	/*TSubclassOf<APlayerStart> ToFind;
	TArray<AActor*> PlayerStarts;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ToFind, PlayerStarts);

	for (AActor* PlayerStart : PlayerStarts)
	{
		APlayerStart* Start = Cast<APlayerStart>(PlayerStart);
		if(!Start->ActorHasTag("Taken"))
		{
			Start->Tags.Add("Taken");
			return Start;
		}
	}*/
	
	return Super::ChoosePlayerStart_Implementation(Player);
}

void ABattleArenaGameMode::CompleteMiniGame(AActor* Player)
{
	if(!MinigameComplete)
	{
		MinigameComplete = true;
		if(Player)
		{
			Player->TeleportTo(FVector(0,0,0),FRotator(0,0,0));
			SetLootTimer();
		}
	}
}

void ABattleArenaGameMode::SetLootTimer()
{
	GetWorldTimerManager().SetTimer(LootTimer, this,&ABattleArenaGameMode::EndLooting, 5.0f,false,5.0f);
}

void ABattleArenaGameMode::EndLooting()
{
	GetWorld()->ServerTravel("/Game/ThirdPerson/Maps/Level2", ETravelType::TRAVEL_Absolute);
}
