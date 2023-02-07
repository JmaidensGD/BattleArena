// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "BattleArenaGameInstance.generated.h"

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


protected:
	

	TSharedPtr<FOnlineSessionSearch> SessionSearch;
	
	virtual void Init() override;

	virtual void OnCreateSessionComplete(FName ServerName, bool Success);
	virtual void OnFindSessionComplete(bool Success);
	virtual void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);

	UFUNCTION(BlueprintCallable)
	void CreateServer();
	UFUNCTION(BlueprintCallable)
	void JoinServer();
};
