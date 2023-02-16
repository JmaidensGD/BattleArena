// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"

#include "BattleArenaCharacter.h"
#include "Net/UnrealNetwork.h"

// Sets default values
AWeapon::AWeapon()
{

	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Weapon"));
	RootComponent = WeaponMesh;
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	bReplicates = true;
}

void AWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AWeapon,WeaponMesh);
	DOREPLIFETIME(AWeapon,WeaponData);

}
// Called when the game starts or when spawned
void AWeapon::BeginPlay()
{
	Super::BeginPlay();
	SetupWeapon(WeaponData);
}
void AWeapon::Attack(){}

void AWeapon::StopAttack(){}

/*void AWeapon::SetupWeapon(UPDA_WeaponBase* Data)
{
	UE_LOG(LogTemp, Warning, TEXT("Setup Weapon"));
	if(Data)
	{
		DamageAmount = Data->Damage;
		WeaponData = Data;
		WeaponMesh->SetSkeletalMesh(Data->Mesh);
	}
}*/

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