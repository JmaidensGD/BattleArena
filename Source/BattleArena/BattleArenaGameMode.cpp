// Copyright Epic Games, Inc. All Rights Reserved.

#include "BattleArenaGameMode.h"
#include "BattleArenaCharacter.h"
#include "BattleArenaGameInstance.h"
#include "BattleArenaPlayerController.h"
#include "BattleArenaGameState.h"
#include "BattleArenaPlayerState.h"
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
	CountdownLength = 60.0f;
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
	if (ABattleArenaPlayerController* PlayerController = Cast<ABattleArenaPlayerController>(NewPlayer))
	{
		PlayerController->PlayerID = NextID;
		UE_LOG(LogTemp, Warning, TEXT("PostLogin: %i"), PlayerController->PlayerID);
	}	
	NextID++;
	ABattleArenaCharacter* PC = Cast<ABattleArenaCharacter>(NewPlayer->GetPawn());
	PC->MaxHealth = 100.0f;
	PC->PlayerHealth = PC->MaxHealth;
}

void ABattleArenaGameMode::CompleteMiniGame(AActor* Player)
{
	if(!MinigameComplete)
	{
		MinigameComplete = true;
		if(Player)
		{
			Player->TeleportTo(FVector(0,0,0),FRotator(0,0,0));
			SetLootTimer();
		}
	}
}

void ABattleArenaGameMode::SetLootTimer()
{
	ABattleArenaGameState* GS = GetGameState<ABattleArenaGameState>();
	if(GS)
	{
		GetWorldTimerManager().SetTimer(GS->LootTimer, this,&ABattleArenaGameMode::EndLooting, 30.0f,false,30.0f);
	}
}

void ABattleArenaGameMode::EndLooting()
{
	GetWorld()->ServerTravel("/Game/ThirdPerson/Maps/Level2", ETravelType::TRAVEL_Absolute);
}

void ABattleArenaGameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	if(ABattleArenaGameState* GS = GetGameState<ABattleArenaGameState>())
	{
		GS->UpdateTimer(CountdownLength);
	}
}
