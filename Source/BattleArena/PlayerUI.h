// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BattleArenaPlayerState.h"
#include "Blueprint/UserWidget.h"
#include "Components/ProgressBar.h"
#include "PlayerUI.generated.h"

/**
 * 
 */
UCLASS()
class BATTLEARENA_API UPlayerUI : public UUserWidget
{
	GENERATED_BODY()
	
private:
	virtual bool Initialize() override;

public:

	UFUNCTION(BlueprintImplementableEvent,BlueprintCallable)
	void UpdateUI();
};
