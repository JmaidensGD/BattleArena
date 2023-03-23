// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BattleArenaGameInstance.h"
#include "GameFramework/GameStateBase.h"
#include "BattleArenaGameState.generated.h"

/**
 * 
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnScoreChangedSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerJoinedSignature, FString, Name);

UCLASS()
class BATTLEARENA_API ABattleArenaGameState : public AGameStateBase
{
	GENERATED_BODY()
	
public:

	ABattleArenaGameState();
	
	UPROPERTY(BlueprintAssignable)
	FOnPlayerJoinedSignature OnPlayerJoin;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated)
	FServerInfo CurrentServerInfo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated)
	TArray<FString> LobbyPlayers;

	UPROPERTY(VisibleAnywhere,Replicated, BlueprintReadWrite)
	TArray<int> Results;

	UPROPERTY(Replicated,VisibleAnywhere,BlueprintReadWrite)
	int rounds;

	UFUNCTION(Server,Reliable,BlueprintCallable)
	void AddScore(int32 WinnerID);

	UFUNCTION(Server,Reliable,BlueprintCallable)
	void GetInventories();

	UFUNCTION(Client,Reliable,BlueprintCallable)
	void UpdateTimer(float Length);

	UPROPERTY(BlueprintAssignable)
	FOnScoreChangedSignature OnScoreChangedSignature;

	UPROPERTY(Replicated,VisibleAnywhere,BlueprintReadWrite)
	float LootTimerValue;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	float LootTimerLength;
	
	UPROPERTY(Replicated)
	FTimerHandle LootTimer;
};
