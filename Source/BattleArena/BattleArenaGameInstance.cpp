// Fill out your copyright notice in the Description page of Project Settings.


#include "BattleArenaGameInstance.h"
#include  "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "Kismet/GameplayStatics.h"

UBattleArenaGameInstance::UBattleArenaGameInstance()
{
	MySessionName = FName("Base Session Name");
}

void UBattleArenaGameInstance::Init()
{
	Super::Init();
	if(IOnlineSubsystem* SubSystem = IOnlineSubsystem::Get())
	{
		SessionInterface = SubSystem->GetSessionInterface();
		if(SessionInterface.IsValid())
		{
			//Bind Delegeates
			SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &UBattleArenaGameInstance::OnCreateSessionComplete);
			SessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this,&UBattleArenaGameInstance::OnFindSessionComplete);
			SessionInterface->OnJoinSessionCompleteDelegates.AddUObject(this,&UBattleArenaGameInstance::OnJoinSessionComplete);
		}
	}
}

void UBattleArenaGameInstance::OnCreateSessionComplete(FName SessionName, bool Success)
{
	UE_LOG(LogTemp, Warning, TEXT("OnCreateSessionComplete, Succeeded: %d"), Success)
	if(Success)
	{
		GetWorld()->ServerTravel("/Game/ThirdPerson/Maps/Level1?listen");
	}
}

void UBattleArenaGameInstance::OnFindSessionComplete(bool Success)
{
	SearchingForServer.Broadcast(false);
	
	UE_LOG(LogTemp, Warning, TEXT("OnFindSessionComplete, Succeeded: %d"), Success)

	if(Success)
	{
		int8 ArrayIndex = 0;

		for (FOnlineSessionSearchResult SearchResult : SessionSearch->SearchResults)
		{
			
			if(!SearchResult.IsValid())
				continue;
			FServerInfo ServerInfo;
			FString ServerName = "Placeholder Server Name";
			FString HostName = "Placeholder Server Host Name";

			SearchResult.Session.SessionSettings.Get(FName("ServerNameKey"), ServerName);
			SearchResult.Session.SessionSettings.Get(FName("ServerHostNameKey"), HostName);
			
			ServerInfo.ServerName = ServerName;
			ServerInfo.MaxPlayers = SearchResult.Session.SessionSettings.NumPublicConnections;
			ServerInfo.CurrentPlayers = ServerInfo.MaxPlayers - SearchResult.Session.NumOpenPublicConnections;
			ServerInfo.ServerIndex = ArrayIndex;
			ServerInfo.SetPlayerCount();
			
			ServerListDelegate.Broadcast(ServerInfo);
			ArrayIndex++;

		}

		UE_LOG(LogTemp, Warning, TEXT("SearchResults, Server Count: %d"), SessionSearch->SearchResults.Num());
		
		if(SessionSearch->SearchResults.Num())
		{
			//UE_LOG(LogTemp, Warning, TEXT("Joining Server"));
			//SessionInterface->JoinSession(0,"Base Session Name", SearchResults[0]);
		}
		
	}
}

void UBattleArenaGameInstance::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	UE_LOG(LogTemp, Warning, TEXT("OnJoinSessionComplete, SessionName : %s"),*SessionName.ToString());
	if(APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(),0))
	{
		FString JoinAddress;
		SessionInterface->GetResolvedConnectString(SessionName, JoinAddress);
		if(JoinAddress!="")
		{
			PlayerController->ClientTravel(JoinAddress, TRAVEL_Absolute);
		}
	}
}

void UBattleArenaGameInstance::CreateServer(FString ServerName, FString HostName)
{
	UE_LOG(LogTemp, Warning, TEXT("Creating Server"))
	
	FOnlineSessionSettings SessionSettings;
	SessionSettings.bAllowJoinInProgress = true;
	SessionSettings.bIsDedicated = false;
	if (IOnlineSubsystem::Get()->GetSubsystemName() != "NULL")
		SessionSettings.bIsLANMatch = false;
	else
		SessionSettings.bIsLANMatch = true;
	
	SessionSettings.bShouldAdvertise = true;
	SessionSettings.bUsesPresence = true;
	SessionSettings.NumPublicConnections = 5;
	SessionSettings.Set(FName("ServerNameKey"), ServerName, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
	SessionSettings.Set(FName("ServerHostNameKey"), HostName, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
	
	SessionInterface->CreateSession(0, MySessionName, SessionSettings);
}

void UBattleArenaGameInstance::FindServers()
{
	SearchingForServer.Broadcast(true);
	
	UE_LOG(LogTemp, Warning, TEXT("Finding Server"))
	
	SessionSearch=MakeShareable(new FOnlineSessionSearch());
	if (IOnlineSubsystem::Get()->GetSubsystemName() != "NULL")
		SessionSearch->bIsLanQuery = false;
	else
		SessionSearch->bIsLanQuery = true;
	
	SessionSearch->MaxSearchResults = 100;
	SessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);
	
	SessionInterface->FindSessions(0, SessionSearch.ToSharedRef());
}

void UBattleArenaGameInstance::JoinServer(int32 ArrayIndex)
{
	FOnlineSessionSearchResult Result = SessionSearch->SearchResults[ArrayIndex];
	if(Result.IsValid())
	{
		SessionInterface->JoinSession(0,MySessionName,Result);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("FAILED TO JOIN SERVER, %d"), ArrayIndex);
	}
}
