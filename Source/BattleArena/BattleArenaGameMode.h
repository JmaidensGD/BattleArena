// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "PDA_WeaponBase.h"
#include "GameFramework/GameModeBase.h"
#include "BattleArenaGameMode.generated.h"

UCLASS(minimalapi)
class ABattleArenaGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ABattleArenaGameMode();

protected:

	virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;

	virtual void PostLogin(APlayerController* NewPlayer) override;
	
	UFUNCTION(BlueprintCallable)
	void CompleteMiniGame(AActor* Player);

	UFUNCTION(BlueprintCallable)
	void SetLootTimer();

	void EndLooting();

	bool MinigameComplete;
	
	FTimerHandle LootTimer;

	UPROPERTY()
	int32 NextID;
	float CountdownLength;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	UPDA_WeaponBase* Weapondata;

	virtual void Tick(float DeltaSeconds) override;

};