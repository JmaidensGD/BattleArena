// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"

// Sets default values
AWeapon::AWeapon()
{

	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Weapon"));
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

}

// Called when the game starts or when spawned
void AWeapon::BeginPlay()
{
	Super::BeginPlay();
	SetupWeapon();
}

void AWeapon::Attack(){}

void AWeapon::StopAttack(){}

void AWeapon::SetupWeapon()
{
	if(WeaponData)
	{
		DamageAmount = WeaponData->Damage;
		WeaponMesh->SetSkeletalMesh(WeaponData->Mesh);
	}
}

void AWeapon::Interact_Implementation()
{
	IInteractable::Interact_Implementation();
	UE_LOG(LogTemp, Warning, TEXT("Interacted"));
}

bool AWeapon::CanInteract_Implementation()
{
	return true;
}
