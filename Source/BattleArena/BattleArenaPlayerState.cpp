// Fill out your copyright notice in the Description page of Project Settings.


#include "BattleArenaPlayerState.h"

#include "Net/UnrealNetwork.h"

ABattleArenaPlayerState::ABattleArenaPlayerState()
{
	UpdateHud.AddDynamic(this,&ABattleArenaPlayerState::UpdateHudEvent);
}

void ABattleArenaPlayerState::ApplyDamage(float DamageAmount)
{
	PlayerHealth -= DamageAmount;
	UpdateHud.Broadcast(); 
	UE_LOG(LogTemp, Warning, TEXT("Health: %f"), PlayerHealth);
}

void ABattleArenaPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ABattleArenaPlayerState,MaxHealth);
	DOREPLIFETIME(ABattleArenaPlayerState,PlayerHealth);
}
