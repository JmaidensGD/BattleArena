// Fill out your copyright notice in the Description page of Project Settings.


#include "BattleArenaGameState.h"

#include "BattleArenaCharacter.h"
#include "BattleArenaGameInstance.h"
#include "BattleArenaPlayerController.h"
#include "InventoryComponent.h"
#include "GameFramework/PlayerState.h"
#include "Net/UnrealNetwork.h"

void ABattleArenaGameState::AddScore_Implementation(int winner)
{
	UBattleArenaGameInstance* GI = Cast<UBattleArenaGameInstance>(GetGameInstance());
	rounds = GI->RoundNumber;
	Results.Results = GI->Results;
	if (Results.Results.Contains(winner))
	{
		Results.Results[winner]++;
	}
	else
	{
		Results.Results.Add(winner,1);
	}
	rounds++;
	GI->RoundNumber++;
	for (APlayerState* PlayerState : PlayerArray)
	{
		if (ABattleArenaCharacter* PC = PlayerState->GetPawn<ABattleArenaCharacter>())
		{
			PC->RoundEnd(winner);
		}
		UE_LOG(LogTemp, Warning, TEXT("PLAYER"));
		ABattleArenaCharacter* PC = PlayerState->GetPawn<ABattleArenaCharacter>();
		if(PC)
		{
			PC->UpdateUI();
		}
	}
	for( TMap<int, int>::TIterator it = Results.Results.CreateIterator(); it; ++it )
	{
		UE_LOG(LogTemp, Warning, TEXT("PLAYER : %d Score : %d"), it.Key(), it.Value());
	}
}

ABattleArenaGameState::ABattleArenaGameState()
{
	bReplicates = true;
	LootTimerLength = 45.0f;
}

void ABattleArenaGameState::GetInventories_Implementation()
{
	UBattleArenaGameInstance* GI = Cast<UBattleArenaGameInstance>(GetGameInstance());
	for (APlayerState* PlayerState : PlayerArray)
	{
		UE_LOG(LogTemp, Warning, TEXT("Get INV"));
		ABattleArenaCharacter* PC = PlayerState->GetPawn<ABattleArenaCharacter>();
		if(PC)
		{
			FPlayerWeapons Weapons;
			Weapons.Weapons = PC->InventoryComponent->Weapons;
			GI->PlayerInventories.Add(PlayerState->GetPlayerId(),Weapons);
		}
	}
}

void ABattleArenaGameState::UpdateTimer_Implementation(float Length)
{
	if (GetWorldTimerManager().GetTimerElapsed(LootTimer) == -1)
	{
		LootTimerValue = Length;
	}
	else
	{
		LootTimerValue = Length - GetWorldTimerManager().GetTimerElapsed(LootTimer);
	}
}

void ABattleArenaGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ABattleArenaGameState, Results);
	DOREPLIFETIME(ABattleArenaGameState, rounds);
	DOREPLIFETIME(ABattleArenaGameState, LootTimer);
	DOREPLIFETIME(ABattleArenaGameState, LootTimerValue);
	DOREPLIFETIME(ABattleArenaGameState, LobbyPlayers);
	DOREPLIFETIME(ABattleArenaGameState, CurrentServerInfo);
}