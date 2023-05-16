// Copyright Epic Games, Inc. All Rights Reserved.

#include "BattleArenaGameMode.h"
#include "BattleArenaCharacter.h"
#include "BattleArenaGameInstance.h"
#include "BattleArenaPlayerController.h"
#include "BattleArenaGameState.h"
#include "BattleArenaPlayerState.h"
#include "MeleeWeapon.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "UObject/ConstructorHelpers.h"

ABattleArenaGameMode::ABattleArenaGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	//NextID = 0;
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
	PrimaryActorTick.bStartWithTickEnabled = true;
	PrimaryActorTick.bCanEverTick = true;
	bUseSeamlessTravel = true;
}


AActor* ABattleArenaGameMode::ChoosePlayerStart_Implementation(AController* Player)
{
	/*TSubclassOf<APlayerStart> ToFind;
	TArray<AActor*> PlayerStarts;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ToFind, PlayerStarts);

	for (AActor* PlayerStart : PlayerStarts)
	{
		APlayerStart* Start = Cast<APlayerStart>(PlayerStart);
		if(!Start->ActorHasTag("Taken"))
		{
			Start->Tags.Add("Taken");
			return Start;
		}
	}*/
	
	return Super::ChoosePlayerStart_Implementation(Player);
}

void ABattleArenaGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	ABattleArenaCharacter* PC = Cast<ABattleArenaCharacter>(NewPlayer->GetPawn());
	PC->SpawnLocation = PC->GetActorLocation();
	UBattleArenaGameInstance* GI = GetGameInstance<UBattleArenaGameInstance>();
	APlayerState* NewPlayerState = NewPlayer->GetPlayerState<APlayerState>();
	if (NewPlayerState!= nullptr)
	{
		GI->PlayersAlive.Add(NewPlayerState->GetPlayerId());
	}
	PC->MaxHealth = 100.0f;
	PC->PlayerHealth = PC->MaxHealth;
	/*FVector Loc = PC->GetActorLocation();
	Loc.Z += 50;
	FRotator Rot(0,0,0);
	FActorSpawnParameters SpawnInfo;
	AMeleeWeapon* Weapon = GetWorld()->SpawnActor<AMeleeWeapon>(Loc,Rot,SpawnInfo);
	Weapon->SetupWeapon(Weapondata);
	PC->EquippedWeapon = Weapon;*/
}

void ABattleArenaGameMode::CompleteMiniGame(AActor* Player)
{
	if(!MinigameComplete)
	{
		MinigameComplete = true;
		SetLootTimer();
	}
	if(Player)
	{
		Player->TeleportTo(FVector(700,950,250),FRotator(0,0,0),false,true);
	}
}

void ABattleArenaGameMode::FailMinigame(AActor* Player)
{
	if(Player)
	{
		Player->TeleportTo(Cast<ABattleArenaCharacter>(Player)->SpawnLocation,FRotator(0,0,0),false,true);
	}
}

void ABattleArenaGameMode::SetLootTimer()
{
	ABattleArenaGameState* GS = GetGameState<ABattleArenaGameState>();
	if(GS)
	{
		GetWorldTimerManager().SetTimer(GS->LootTimer, this,&ABattleArenaGameMode::EndLooting, GS->LootTimerLength,false,GS->LootTimerLength);
	}
}

void ABattleArenaGameMode::EndLooting()
{
	GetGameState<ABattleArenaGameState>()->GetInventories();
	GetWorld()->ServerTravel("/Game/HG_Levels/HG_Level2", true);
	//PlayersAlive = GetNumPlayers();
}

void ABattleArenaGameMode::PlayerDeath(int32 ID)
{
	UE_LOG(LogTemp, Warning, TEXT("player death : %d"), ID);

	UBattleArenaGameInstance* GI = GetGameInstance<UBattleArenaGameInstance>();

	UE_LOG(LogTemp, Warning, TEXT("players alive : %d"), GI->PlayersAlive.Num());
	
	GI->PlayersAlive.Remove(ID);

	UE_LOG(LogTemp, Warning, TEXT("players alive : %d"), GI->PlayersAlive.Num());
	
	if(GI->PlayersAlive.Num()==1)
	{
		GI->PlayersAlive.Empty();
		for (APlayerState* PlayerState : GetGameState<ABattleArenaGameState>()->PlayerArray)
		{
			GI->PlayersAlive.Add(PlayerState->GetPlayerId());
		}
		EndRound(GI->PlayersAlive[0]);
	}
}

void ABattleArenaGameMode::EndRound(int32 Winner)
{
	UBattleArenaGameInstance* GI = GetGameInstance<UBattleArenaGameInstance>();
	GI->PlayerInventories.Empty();
	UE_LOG(LogTemp, Warning, TEXT("End Round"));
	ABattleArenaGameState* GS = Cast<ABattleArenaGameState>(GameState);
	GS->AddScore(GI->PlayersAlive[0]);
	GetWorld()->ServerTravel("/Game/HG_Levels/HG_Level1", true);
}

void ABattleArenaGameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	if(ABattleArenaGameState* GS = GetGameState<ABattleArenaGameState>())
	{
		GS->UpdateTimer(GS->LootTimerLength);
	}
}