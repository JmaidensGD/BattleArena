// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PDA_WeaponBase.h"
#include "Engine/GameInstance.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "BattleArenaGameInstance.generated.h"

USTRUCT()
struct FPlayerWeapons
{
	GENERATED_BODY()
public:
	UPROPERTY()
	TArray<UPDA_WeaponBase*> Weapons;
};

USTRUCT(BlueprintType)
struct FServerInfo
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadOnly)
	FString ServerName;
	UPROPERTY(BlueprintReadOnly)
	FString PlayerCountString;
	UPROPERTY(BlueprintReadOnly)
	int32 CurrentPlayers;
	UPROPERTY(BlueprintReadOnly)
	int32 MaxPlayers;
	UPROPERTY(BlueprintReadOnly)
	int32 ServerIndex;
	
	void SetPlayerCount()
	{
		PlayerCountString = FString::FromInt(CurrentPlayers) + "/" + FString::FromInt(MaxPlayers);
	}
};


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FServerDelegate, FServerInfo, ServerListDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FServerSearchingDelegate, bool, SearchingForServer);

/**
 * 
 */
UCLASS()
class BATTLEARENA_API UBattleArenaGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
	UBattleArenaGameInstance();
	IOnlineSessionPtr SessionInterface;
	
	UPROPERTY()
	TMap<int32,FPlayerWeapons> PlayerInventories;

protected:
	FName MySessionName;

	UPROPERTY(BlueprintAssignable)
	FServerDelegate ServerListDelegate;
	UPROPERTY(BlueprintAssignable)
	FServerSearchingDelegate SearchingForServer;

	TSharedPtr<FOnlineSessionSearch> SessionSearch;
	
	virtual void Init() override;

	virtual void OnCreateSessionComplete(FName SessionName, bool Success);
	virtual void OnFindSessionComplete(bool Success);
	virtual void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);

	UFUNCTION(BlueprintCallable)
	void CreateServer(FString ServerName, FString HostName);
	UFUNCTION(BlueprintCallable)
	void FindServers();
	UFUNCTION(BlueprintCallable)
	void JoinServer(int32 ArrayIndex);
};
