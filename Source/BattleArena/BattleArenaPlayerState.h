// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BattleArenaCharacter.h"
#include "GameFramework/PlayerState.h"
#include "BattleArenaPlayerState.generated.h"

/**
 * 
 */

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FUpdateHud);

UCLASS()
class BATTLEARENA_API ABattleArenaPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintCallable, BlueprintAssignable)
	FUpdateHud UpdateHud;
	
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Replicated)
	float PlayerHealth;
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Replicated)
	float MaxHealth;

	UFUNCTION(Reliable,Server)
	void ApplyDamage(float DamageAmount);
	

protected:
	
	ABattleArenaPlayerState();
	
};
