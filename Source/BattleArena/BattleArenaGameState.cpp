// Fill out your copyright notice in the Description page of Project Settings.


#include "BattleArenaGameState.h"

#include "BattleArenaCharacter.h"
#include "GameFramework/PlayerState.h"
#include "Net/UnrealNetwork.h"

void ABattleArenaGameState::AddScore_Implementation(int winner)
{
	if(Round<5)
	{
		Results[Round]=(winner);
		Round++;
		for (APlayerState* PlayerState : PlayerArray)
		{
			UE_LOG(LogTemp, Warning, TEXT("PLAYER"));
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

void ABattleArenaGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ABattleArenaGameState, Results);
	DOREPLIFETIME(ABattleArenaGameState, Round);

}
