// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "BattleArenaPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class BATTLEARENA_API ABattleArenaPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void SetPlayerPlay();

	/** Set Player to spectate. Should be called only on server */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void SetPlayerSpectate();

	/** Notify HUD of a state change so it shows suitable widgets accordingly */
	UFUNCTION(Reliable, Client)
	void ClientHUDStateChanged(EHUDState NewState);

	virtual void OnRep_Pawn() override;

	UPROPERTY(VisibleAnywhere,BlueprintReadWrite,Replicated)
	int32 PlayerID;

};
