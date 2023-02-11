// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"

#include "BattleArenaCharacter.h"

// Sets default values
AWeapon::AWeapon()
{

	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Weapon"));
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	bReplicates = true;
}

// Called when the game starts or when spawned
void AWeapon::BeginPlay()
{
	Super::BeginPlay();
	SetupWeapon(WeaponData);
}
void AWeapon::Attack(){}

void AWeapon::StopAttack(){}

void AWeapon::SetupWeapon(UPDA_WeaponBase* Weapon)
{
	if(Weapon)
	{
		DamageAmount = Weapon->Damage;
		WeaponMesh->SetSkeletalMesh(Weapon->Mesh);
	}
}

void AWeapon::Interact_Implementation(ABattleArenaCharacter* Player)
{
	IInteractable::Interact_Implementation(Player);
	UE_LOG(LogTemp, Warning, TEXT("Interacted"));
	Player->PickupWeapon(this->WeaponData,this);
	this->Destroy();
}

bool AWeapon::CanInteract_Implementation()
{
	return true;
}
