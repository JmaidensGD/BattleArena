// Fill out your copyright notice in the Description page of Project Settings.


#include "ArenaSpectator.h"

#include "BattleArenaPlayerController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

void AArenaSpectator::SetupPlayerInputComponent(UInputComponent* SpectatorInputComponent)
{
	Super::SetupPlayerInputComponent(SpectatorInputComponent);
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(SpectatorInputComponent)) {
		
		EnhancedInputComponent->BindAction(NextPlayerAction, ETriggerEvent::Completed, this, &AArenaSpectator::NextPlayer);
		EnhancedInputComponent->BindAction(PrevPlayerAction, ETriggerEvent::Completed, this, &AArenaSpectator::PrevPlayer);

	}
}

void AArenaSpectator::BeginPlay()
{
	Super::BeginPlay();
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(MappingContext, 0);
		}
	}
}

void AArenaSpectator::NextPlayer()
{
	if(ABattleArenaPlayerController* PC = GetController<ABattleArenaPlayerController>())
	{
		PC->ServerViewNextPlayer();
	}
}

void AArenaSpectator::PrevPlayer()
{
	if(ABattleArenaPlayerController* PC = GetController<ABattleArenaPlayerController>())
	{
		PC->ServerViewPrevPlayer();
	}
}
