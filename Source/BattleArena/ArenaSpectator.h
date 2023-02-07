// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SpectatorPawn.h"
#include "ArenaSpectator.generated.h"

/**
 * 
 */
UCLASS()
class BATTLEARENA_API AArenaSpectator : public ASpectatorPawn
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputMappingContext* MappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* NextPlayerAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* PrevPlayerAction;

	UFUNCTION()
	void NextPlayer();

	UFUNCTION()
	void PrevPlayer();

	virtual void SetupPlayerInputComponent(class UInputComponent* SpectatorInputComponent) override;

	virtual void BeginPlay() override;
	
};
