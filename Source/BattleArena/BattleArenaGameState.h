// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "BattleArenaGameState.generated.h"

/**
 * 
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnScoreChangedSignature);
UCLASS()
class BATTLEARENA_API ABattleArenaGameState : public AGameStateBase
{
	GENERATED_BODY()
	
public:

	ABattleArenaGameState();

	UPROPERTY(VisibleAnywhere,Replicated, BlueprintReadWrite)
	TArray<int> Results;

	UPROPERTY(Replicated,VisibleAnywhere,BlueprintReadWrite)
	int rounds;

	UFUNCTION(Server,Reliable,BlueprintCallable)
	void AddScore(int winner);

	UFUNCTION(Client,Reliable,BlueprintCallable)
	void UpdateTimer(float Length);

	UPROPERTY(BlueprintAssignable)
	FOnScoreChangedSignature OnScoreChangedSignature;

	UPROPERTY(Replicated,VisibleAnywhere,BlueprintReadWrite)
	float LootTimerValue;
	
	UPROPERTY(Replicated)
	FTimerHandle LootTimer;
};
