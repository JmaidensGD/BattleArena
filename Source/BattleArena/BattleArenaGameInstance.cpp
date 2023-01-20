// Fill out your copyright notice in the Description page of Project Settings.


#include "BattleArenaGameInstance.h"
#include  "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"

UBattleArenaGameInstance::UBattleArenaGameInstance()
{
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
		}
	}
}

void UBattleArenaGameInstance::OnCreateSessionComplete(FName ServerName, bool Success)
{
	UE_LOG(LogTemp, Warning, TEXT("OnCreateSessionComplete, Succeeded: %d"), Success)
	if(Success)
	{
		GetWorld()->ServerTravel("/Game/ThirdPerson/Maps/Level1?listen");
	}
}

void UBattleArenaGameInstance::OnFindSessionComplete(bool Success)
{
	UE_LOG(LogTemp, Warning, TEXT("OnFindSessionComplete, Succeeded: %d"), Success)

	if(Success)
	{
		TArray<FOnlineSessionSearchResult> SearchResults = SessionSearch->SearchResults;
		UE_LOG(LogTemp, Warning, TEXT("SearchResults, Server Count: %d"), SearchResults.Num());
	}
}

void UBattleArenaGameInstance::CreateServer()
{
	UE_LOG(LogTemp, Warning, TEXT("Creating Server"))
	
	FOnlineSessionSettings SessionSettings;
	SessionSettings.bAllowJoinInProgress = true;
	SessionSettings.bIsDedicated = false;
	SessionSettings.bIsLANMatch = true;
	SessionSettings.bShouldAdvertise = true;
	SessionSettings.bUsesPresence = true;
	SessionSettings.NumPublicConnections = 5;
	
	SessionInterface->CreateSession(0, FName("Base Session Name"), SessionSettings);
}

void UBattleArenaGameInstance::JoinServer()
{
	UE_LOG(LogTemp, Warning, TEXT("Joining Server"))
	
	SessionSearch=MakeShareable(new FOnlineSessionSearch());
	SessionSearch->bIsLanQuery = true;
	SessionSearch->MaxSearchResults = 10000;g
	SessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);
	
	SessionInterface->FindSessions(0, SessionSearch.ToSharedRef());
}
