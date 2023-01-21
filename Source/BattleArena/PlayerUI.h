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

	UPROPERTY(meta=(BindWidget))
	UProgressBar* PlayerHealthBar;
	
private:
	virtual bool Initialize() override;

public:
	UPROPERTY(BlueprintReadWrite)
	ABattleArenaPlayerState* PS;
	
};
