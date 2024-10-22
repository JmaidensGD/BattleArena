// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "PDA_WeaponBase.h"
#include "GameFramework/GameModeBase.h"
#include "BattleArenaGameMode.generated.h"

class BattleArenaGameInstance;
class ABattleArenaCharacter;

USTRUCT(BlueprintType)
struct FPlayerResult
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere,BlueprintReadWrite)
	int PlayerID;
	UPROPERTY(VisibleAnywhere,BlueprintReadWrite)
	int Score;
};


UCLASS(minimalapi)
class ABattleArenaGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ABattleArenaGameMode();

	UFUNCTION()
	void PlayerDeath(int32 ID);

	UFUNCTION()
	void EndRound(int32 Winner);

	UFUNCTION()
	void EndGame();
	
protected:

	virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;

	virtual void PostLogin(APlayerController* NewPlayer) override;
	
	UFUNCTION(BlueprintCallable)
	void CompleteMiniGame(AActor* Player);

	UFUNCTION(BlueprintCallable)
	void FailMinigame(AActor* Player);

	UFUNCTION(BlueprintCallable)
	void SetLootTimer();

	UPROPERTY()
	ABattleArenaCharacter* PlayerChar;
	
	void EndLooting();

	bool MinigameComplete;
	
	FTimerHandle LootTimer;
	
	UPROPERTY(VisibleAnywhere)
	TArray<int32> PlayersAlive;

	UPROPERTY(VisibleAnywhere,BlueprintReadWrite)
	TArray<FPlayerResult> PlayerResults;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	UPDA_WeaponBase* Weapondata;

	virtual void Tick(float DeltaSeconds) override;

};