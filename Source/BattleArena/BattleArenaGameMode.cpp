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
	TSubclassOf<APlayerStart> ToFind;
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
	}
	
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
		UE_LOG(LogTemp, Warning, TEXT("ADDED PLAYER : %d"), NewPlayerState->GetPlayerId());
	}
	GI->InitScores();
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
	ABattleArenaGameState* GS = Cast<ABattleArenaGameState>(GameState);
	UE_LOG(LogTemp, Warning, TEXT("Round : %d"), GS->rounds);
	GetWorld()->ServerTravel("/Game/HG_Levels/HG_Level2", true);
	//PlayersAlive = GetNumPlayers();
}
 
void ABattleArenaGameMode::PlayerDeath(int32 ID)
{
	UE_LOG(LogTemp, Warning, TEXT("player death : %d"), ID);

	UBattleArenaGameInstance* GI = GetGameInstance<UBattleArenaGameInstance>();

	UE_LOG(LogTemp, Warning, TEXT("array size : %d"), GI->PlayersAlive.Num());
	
	GI->PlayersAlive.Remove(ID);

	UE_LOG(LogTemp, Warning, TEXT("array size : %d"), GI->PlayersAlive.Num());
	
	if(GI->PlayersAlive.Num()==1)
	{
		EndRound(GI->PlayersAlive[0]);
	}
}

void ABattleArenaGameMode::EndRound(int32 Winner)
{
	UE_LOG(LogTemp, Warning, TEXT("WINNER %d"), Winner);
	UBattleArenaGameInstance* GI = GetGameInstance<UBattleArenaGameInstance>();
	GI->PlayersAlive.Empty();
	GI->PlayerInventories.Empty();
	for (APlayerState* PlayerState : GetGameState<ABattleArenaGameState>()->PlayerArray)
	{
		GI->PlayersAlive.Add(PlayerState->GetPlayerId());
	}
	UE_LOG(LogTemp, Warning, TEXT("End Round"));
	ABattleArenaGameState* GS = Cast<ABattleArenaGameState>(GameState);
	GS->AddScore(Winner);
	GI->Results = GS->Results.Results;
	GetWorld()->ServerTravel("/Game/HG_Levels/HG_Level1", true);
}

void ABattleArenaGameMode::EndGame()
{
	UBattleArenaGameInstance* GI = GetGameInstance<UBattleArenaGameInstance>();
	for( TMap<int, int>::TIterator it = GI->Results.CreateIterator(); it; ++it )
	{
		PlayerResults.Add({it.Key(),it.Value()});
	}

	PlayerResults.Sort([](const FPlayerResult& Lhs, const FPlayerResult& Rhs) -> bool{
	// sort by a
	if (Lhs.Score > Rhs.Score)
	{
		return true;
	}
	else
	{
		return false;
	}
	});

	GI->SortedScores = PlayerResults;

	UE_LOG(LogTemp, Warning, TEXT("SIZE : %d"),PlayerResults.Num());

	for (FPlayerResult PlayerResult : PlayerResults)
	{
		UE_LOG(LogTemp, Warning, TEXT("ID : %d , SCORE : %d"),PlayerResult.PlayerID,PlayerResult.Score);
	}
	GetWorld()->ServerTravel("/Game/HG_Levels/PostRound", true);
}

void ABattleArenaGameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	if(ABattleArenaGameState* GS = GetGameState<ABattleArenaGameState>())
	{
		GS->UpdateTimer(GS->LootTimerLength);
	}
}
