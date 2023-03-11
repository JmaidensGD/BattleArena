// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "BattleArenaLobbyGamemode.generated.h"

/**
 * 
 */
UCLASS()
class BATTLEARENA_API ABattleArenaLobbyGamemode : public AGameModeBase
{
	GENERATED_BODY()

	ABattleArenaLobbyGamemode();

protected:
	virtual void PostLogin(APlayerController* NewPlayer) override;

	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	void StartGame();

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Replicated)
	TArray<APlayerController*> AllPlayerControllers;

public:
};
