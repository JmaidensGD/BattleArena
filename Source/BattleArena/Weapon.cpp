// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"

// Sets default values
AWeapon::AWeapon()
{

	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Weapon"));

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
	TArray<FName> Rows = WeaponStatsTable->GetRowNames();

	for (FName Row : Rows)
	{
		WeaponRow = WeaponStatsTable->FindRow<FWeaponStats>(Row,"");
		if(WeaponRow->WeaponType == WeaponType)
		{
			break;
		}
	}
	DamageAmount = WeaponRow->Damage;
}

