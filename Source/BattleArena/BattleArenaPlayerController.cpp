// Fill out your copyright notice in the Description page of Project Settings.


#include "BattleArenaPlayerController.h"

#include "BattleArenaHUD.h"
#include "Enums.h"
#include "GameFramework/PlayerState.h"
#include "Net/UnrealNetwork.h"

void ABattleArenaPlayerController::SetPlayerPlay()
{
	// Only proceed if we're on the server
	if (!HasAuthority())
	{
		return;
	}

	// Update the state on server
	PlayerState->SetIsSpectator(false);
	ChangeState(NAME_Playing);

	bPlayerIsWaiting = false;

	// Push the state update to the client
	ClientGotoState(NAME_Playing);

	// Update the HUD to remove the spectator screen
	ClientHUDStateChanged(EHUDState::Playing);
}

void ABattleArenaPlayerController::OnRep_Pawn()
{
	Super::OnRep_Pawn();
	if(IsInState(NAME_Spectating))
	{
		ServerViewNextPlayer();
	}
}


void ABattleArenaPlayerController::SetPlayerSpectate()
{
	// Only proceed if we're on the server
	if (!HasAuthority())
	{
		return;
	}

	// Update the state on server
	PlayerState->SetIsSpectator(true);
	ChangeState(NAME_Spectating);

	bPlayerIsWaiting = true;

	// Push the state update to the client
	ClientGotoState(NAME_Spectating);

	ViewAPlayer(1);

	// Update the HUD to add the spectator screen
	ClientHUDStateChanged(EHUDState::Spectating);
}

void ABattleArenaPlayerController::ClientHUDStateChanged_Implementation(EHUDState NewState)
{
	if (ABattleArenaHUD* HUD = GetHUD<ABattleArenaHUD>())
	{
		HUD->OnStateChanged(NewState);
	}
}

void ABattleArenaPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ABattleArenaPlayerController, PlayerID);

}