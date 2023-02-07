// Fill out your copyright notice in the Description page of Project Settings.


#include "BattleArenaHUD.h"

EHUDState ABattleArenaHUD::GetCurrentState() const
{
	return CurrentState;
}

void ABattleArenaHUD::OnStateChanged_Implementation(EHUDState NewState)
{
	CurrentState = NewState;
}