// Fill out your copyright notice in the Description page of Project Settings.


#include "BattleArenaLobbyGamemode.h"

#include "BattleArenaGameInstance.h"
#include "BattleArenaGameState.h"
#include "Net/UnrealNetwork.h"

ABattleArenaLobbyGamemode::ABattleArenaLobbyGamemode()
{
}

void ABattleArenaLobbyGamemode::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ABattleArenaLobbyGamemode,AllPlayerControllers);
}

void ABattleArenaLobbyGamemode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	if (GetLocalRole() == ROLE_Authority)
	{
		AllPlayerControllers.Add(NewPlayer);
		UBattleArenaGameInstance* GI = GetGameInstance<UBattleArenaGameInstance>();
		UE_LOG(LogTemp, Warning, TEXT("PostLogin TESTER: %s"), *GI->CurrentServerInfo.ServerName);
		
	}
	else
	{
		
	}
}

void ABattleArenaLobbyGamemode::BeginPlay()
{
	Super::BeginPlay();
	ABattleArenaGameState* GS = GetGameState<ABattleArenaGameState>();
	UBattleArenaGameInstance* GI = GetGameInstance<UBattleArenaGameInstance>();
	GS->CurrentServerInfo.ServerName = GI->CurrentServerInfo.ServerName;
	//GetGameState<ABattleArenaGameState>()->CurrentServerInfo.ServerName = GetGameInstance<UBattleArenaGameInstance>()->CurrentServerInfo.ServerName;
	//UE_LOG(LogTemp, Warning, TEXT("PostLogin TESTER: %s"), *GetGameState<ABattleArenaGameState>()->CurrentServerInfo.ServerName);
	UE_LOG(LogTemp, Warning, TEXT("PostLoginyutr %s"), *GS->CurrentServerInfo.ServerName);


}
