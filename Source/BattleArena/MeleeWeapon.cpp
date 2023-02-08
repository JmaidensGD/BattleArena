// Fill out your copyright notice in the Description page of Project Settings.


#include "MeleeWeapon.h"

void AMeleeWeapon::Interact_Implementation()
{
	IInteractable::Interact_Implementation();
	UE_LOG(LogTemp, Warning, TEXT("Interacted melee"));
}
