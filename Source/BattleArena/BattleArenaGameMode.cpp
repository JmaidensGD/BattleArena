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
#include "GameLiftServerSDK.h"

DEFINE_LOG_CATEGORY(GameServerLog);

ABattleArenaGameMode::ABattleArenaGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	//NextID = 0;
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
#if WITH_GAMELIFT
	UE_LOG(GameServerLog, Log, TEXT("Initializing the GameLift Server"));

	//Getting the module first.
	FGameLiftServerSDKModule* gameLiftSdkModule = &FModuleManager::LoadModuleChecked<FGameLiftServerSDKModule>(FName("GameLiftServerSDK"));

	//Define the server parameters
	FServerParameters serverParameters;

	//AuthToken returned from the "aws gamelift get-compute-auth-token" API. Note this will expire and require a new call to the API after 15 minutes.
	if (FParse::Value(FCommandLine::Get(), TEXT("-authtoken="), serverParameters.m_authToken))
	{
		UE_LOG(GameServerLog, Log, TEXT("AUTH_TOKEN: %s"), *serverParameters.m_authToken)
	}

	//The Host/Compute ID of the GameLift Anywhere instance.
	if (FParse::Value(FCommandLine::Get(), TEXT("-hostid="), serverParameters.m_hostId))
	{
		UE_LOG(GameServerLog, Log, TEXT("HOST_ID: %s"), *serverParameters.m_hostId)
	}

	//The EC2 or Anywhere Fleet ID.
	if (FParse::Value(FCommandLine::Get(), TEXT("-fleetid="), serverParameters.m_fleetId))
	{
		UE_LOG(GameServerLog, Log, TEXT("FLEET_ID: %s"), *serverParameters.m_fleetId)
	}

	//The WebSocket URL (GameLiftServiceSdkEndpoint).
	if (FParse::Value(FCommandLine::Get(), TEXT("-websocketurl="), serverParameters.m_webSocketUrl))
	{
		UE_LOG(GameServerLog, Log, TEXT("WEBSOCKET_URL: %s"), *serverParameters.m_webSocketUrl)
	}

	//The PID of the running process
	serverParameters.m_processId = FString::Printf(TEXT("%d"), GetCurrentProcessId());
	UE_LOG(GameServerLog, Log, TEXT("PID: %s"), *serverParameters.m_processId);

	//InitSDK will establish a local connection with GameLift's agent to enable further communication.
	gameLiftSdkModule->InitSDK(serverParameters);

	//When a game session is created, GameLift sends an activation request to the game server and passes along the game session object containing game properties and other settings.
	//Here is where a game server should take action based on the game session object.
	//Once the game server is ready to receive incoming player connections, it should invoke GameLiftServerAPI.ActivateGameSession()
	auto onGameSession = [=](Aws::GameLift::Server::Model::GameSession gameSession)
	{
		FString gameSessionId = FString(gameSession.GetGameSessionId());
		UE_LOG(GameServerLog, Log, TEXT("GameSession Initializing: %s"), *gameSessionId);
		gameLiftSdkModule->ActivateGameSession();
	};

	FProcessParameters params;
	params.OnStartGameSession.BindLambda(onGameSession);

	//OnProcessTerminate callback. GameLift will invoke this callback before shutting down an instance hosting this game server.
	//It gives this game server a chance to save its state, communicate with services, etc., before being shut down.
	//In this case, we simply tell GameLift we are indeed going to shutdown.
	params.OnTerminate.BindLambda([=]() {
		UE_LOG(GameServerLog, Log, TEXT("Game Server Process is terminating"));
		gameLiftSdkModule->ProcessEnding();
		});

	//This is the HealthCheck callback.
	//GameLift will invoke this callback every 60 seconds or so.
	//Here, a game server might want to check the health of dependencies and such.
	//Simply return true if healthy, false otherwise.
	//The game server has 60 seconds to respond with its health status. GameLift will default to 'false' if the game server doesn't respond in time.
	//In this case, we're always healthy!
	params.OnHealthCheck.BindLambda([]() {
		UE_LOG(GameServerLog, Log, TEXT("Performing Health Check"));
		return true;
		});

	//This game server tells GameLift that it will listen on port 7777 for incoming player connections.
	params.port = 7777;

	//Here, the game server tells GameLift what set of files to upload when the game session ends.
	//GameLift will upload everything specified here for the developers to fetch later.
	TArray<FString> logfiles;
	logfiles.Add(TEXT("GameLift426Test/Saved/Logs/GameLift426Test.log"));
	params.logParameters = logfiles;

	//Calling ProcessReady tells GameLift this game server is ready to receive incoming game sessions!
	UE_LOG(GameServerLog, Log, TEXT("Calling Process Ready"));
	gameLiftSdkModule->ProcessReady(params);
#endif
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