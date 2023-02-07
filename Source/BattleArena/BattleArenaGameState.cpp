// Fill out your copyright notice in the Description page of Project Settings.


#include "BattleArenaGameState.h"

#include "BattleArenaCharacter.h"
#include "GameFramework/PlayerState.h"
#include "Net/UnrealNetwork.h"

void ABattleArenaGameState::AddScore_Implementation(int winner)
{
	if(rounds<5)
	{
		Results[rounds]=(winner);
		rounds++;
		for (APlayerState* PlayerState : PlayerArray)
		{
			ABattleArenaCharacter* PC = PlayerState->GetPawn<ABattleArenaCharacter>();
			if(PC)
			{
				PC->UpdateUI();
			}
		}
	}
}

ABattleArenaGameState::ABattleArenaGameState()
{
	bReplicates = true;
	Results.Init(0,5);
}

void ABattleArenaGameState::UpdateTimer_Implementation(float Length)
{
	LootTimerValue = Length - GetWorldTimerManager().GetTimerElapsed(LootTimer);
}

void ABattleArenaGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ABattleArenaGameState, Results);
	DOREPLIFETIME(ABattleArenaGameState, rounds);
	DOREPLIFETIME(ABattleArenaGameState, LootTimer);
	DOREPLIFETIME(ABattleArenaGameState, LootTimerValue);


}
