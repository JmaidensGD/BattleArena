// Fill out your copyright notice in the Description page of Project Settings.


#include "BattleArenaPlayerState.h"

#include "Net/UnrealNetwork.h"

void ABattleArenaPlayerState::ApplyDamage_Implementation(float DamageAmount)
{
	PlayerHealth -= DamageAmount;
	UE_LOG(LogTemp, Warning, TEXT("Health: %f"), PlayerHealth);
}

ABattleArenaPlayerState::ABattleArenaPlayerState()
{
	bReplicates = true;
}

void ABattleArenaPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ABattleArenaPlayerState,MaxHealth);
	DOREPLIFETIME(ABattleArenaPlayerState,PlayerHealth);
}