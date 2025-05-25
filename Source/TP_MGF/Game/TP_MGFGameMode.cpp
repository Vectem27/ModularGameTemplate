#include "Game/TP_MGFGameMode.h"

// Engine includes
#include "Kismet/GameplayStatics.h"

// Project includes
#include "Pawn/TP_MGFCharacter.h"
#include "Pawn/TP_MGFPawnExtensionComponent.h"
#include "Player/TP_MGFPlayerState.h"
#include "Player/TP_MGFPlayerController.h"
#include "UI/TP_MGFHUD.h"
#include "Game/TP_MGFGameState.h"

#include "Game/GameDefinition/GameDefinition.h"
#include "Game/GameDefinition/GameDefinitionManagerComponent.h"
#include "Game/GameDefinition/PawnData.h"

#include "System/TP_MGFAssetManager.h"
#include "TP_MGFLogChannels.h"




ATP_MGFGameMode::ATP_MGFGameMode(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	GameStateClass = ATP_MGFGameState::StaticClass();
	PlayerControllerClass = ATP_MGFPlayerController::StaticClass();
	PlayerStateClass = ATP_MGFPlayerState::StaticClass();
	DefaultPawnClass = ATP_MGFCharacter::StaticClass();
	HUDClass = ATP_MGFHUD::StaticClass();
}

const UPawnData* ATP_MGFGameMode::GetPawnDataForController(const AController* InController) const
{
	// See if pawn data is already set on the player state
	if (InController != nullptr)
	{
		if (const ATP_MGFPlayerState* PS = InController->GetPlayerState<ATP_MGFPlayerState>())
		{
			if (const UPawnData* PawnData = PS->GetPawnData<UPawnData>())
			{
				return PawnData;
			}
		}
	}

	// If not, fall back to the the default for the current game definition
	check(GameState);
	UGameDefinitionManagerComponent* GameDefComponent = GameState->FindComponentByClass<UGameDefinitionManagerComponent>();
	check(GameDefComponent);

	if (GameDefComponent->IsGameDefinitionLoaded())
	{
		const UGameDefinition* GameDefinition = GameDefComponent->GetCurrentGameDefinitionChecked();
		if (GameDefinition->DefaultPawnData != nullptr)
		{
			return GameDefinition->DefaultPawnData;
		}

		// Game Definition is loaded and there's still no pawn data, fall back to the default for now
		return UTP_MGFAssetManager::Get().GetDefaultPawnData();
	}

	// Game definition not loaded yet, so there is no pawn data to be had
	return nullptr;
}

FString ATP_MGFGameMode::GetGameId() const
{
	return GameId;
}

void ATP_MGFGameMode::SetGameId(FString NewGameId)
{
	GameId = NewGameId;

	K2_OnGameIdSet(GameId);
}

void ATP_MGFGameMode::RequestPlayerRestartNextFrame(AController* Controller, bool bForceReset)
{
	if (bForceReset && (Controller != nullptr))
	{
		Controller->Reset();
	}

	if (APlayerController* PC = Cast<APlayerController>(Controller))
	{
		GetWorldTimerManager().SetTimerForNextTick(PC, &APlayerController::ServerRestartPlayer_Implementation);
	}
}

void ATP_MGFGameMode::OnGameDefinitionLoaded(const UGameDefinition* CurrentGameDefinition)
{
	// Spawn any players that are already attached
	// TODO: Here we're handling only *player* controllers, but in GetDefaultPawnClassForController_Implementation we skipped all controllers
	// GetDefaultPawnClassForController_Implementation might only be getting called for players anyways
	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		APlayerController* PC = Cast<APlayerController>(*Iterator);
		if ((PC != nullptr) && (PC->GetPawn() == nullptr))
		{
			if (PlayerCanRestart(PC))
			{
				RestartPlayer(PC);
			}
		}
	}
}

bool ATP_MGFGameMode::IsGameDefinitionLoaded() const
{
	check(GameState);
	UGameDefinitionManagerComponent* GameDefinitionComponent = GameState->FindComponentByClass<UGameDefinitionManagerComponent>();
	check(GameDefinitionComponent);

	return GameDefinitionComponent->IsGameDefinitionLoaded();
}

///////////////////////////////////////////////////////////////////////
// AGameModeBase methods

void ATP_MGFGameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);

	FString NewGameId = UGameplayStatics::ParseOption(Options, TEXT("GameId"));

	if (!NewGameId.IsEmpty())
	{
		UE_LOG(LogTP_MGF, Display, TEXT("Game id set : %s"), *NewGameId);
		SetGameId(NewGameId);
	}
	else
	{
		UE_LOG(LogTP_MGF, Warning, TEXT("No gameId found after the game initialization"), *GameId);
	}
}

void ATP_MGFGameMode::InitGameState()
{
	Super::InitGameState();

	// TODO: Move all of the game definition loading logic into a separate function

	// Listen for the game definition load to complete	
	UGameDefinitionManagerComponent* GameDefinitionComponent = GameState->FindComponentByClass<UGameDefinitionManagerComponent>();
	check(GameDefinitionComponent);
	GameDefinitionComponent->CallOrRegister_OnGameDefinitionLoaded(FOnGameDefinitionLoaded::FDelegate::CreateUObject(this, &ATP_MGFGameMode::OnGameDefinitionLoaded));

	FPrimaryAssetId GameDefinitionId;
	FString GameDefinitionIdSource;

	UWorld* World = GetWorld();

	if (!GameDefinitionId.IsValid() && UGameplayStatics::HasOption(OptionsString, TEXT("GameDefinition")))
	{
		const FString GameDefinitionFromOptions = UGameplayStatics::ParseOption(OptionsString, TEXT("GameDefinition"));
		GameDefinitionId = FPrimaryAssetId(FPrimaryAssetType(UGameDefinition::StaticClass()->GetFName()), FName(*GameDefinitionFromOptions));
		GameDefinitionIdSource = TEXT("OptionsString");
	}

	// see if the command line wants to set the game definition
	if (!GameDefinitionId.IsValid())
	{
		FString GameDefinitionFromCommandLine;
		if (FParse::Value(FCommandLine::Get(), TEXT("GameDefinition="), GameDefinitionFromCommandLine))
		{
			GameDefinitionId = FPrimaryAssetId::ParseTypeAndName(GameDefinitionFromCommandLine);
			if (!GameDefinitionId.PrimaryAssetType.IsValid())
			{
				GameDefinitionId = FPrimaryAssetId(FPrimaryAssetType(UGameDefinition::StaticClass()->GetFName()), FName(*GameDefinitionFromCommandLine));
			}
			GameDefinitionIdSource = TEXT("CommandLine");
		}
	}

	// TODO: Add world settings support for game definition (Primarily to make development test easier)
	// see if the world settings has a default game definition
	//if (!GameDefinitionId.IsValid())
	//{
	//	if (ATP_MGFWorldSettings* TypedWorldSettings = Cast<ATP_MGFWorldSettings>(GetWorldSettings()))
	//	{
	//		GameDefinitionId = TypedWorldSettings->GetDefaultGameplayGameDefinition();
	//		GameDefinitionIdSource = TEXT("WorldSettings");
	//	}
	//}

	UTP_MGFAssetManager& AssetManager = UTP_MGFAssetManager::Get();
	FAssetData Dummy;
	if (GameDefinitionId.IsValid() && !AssetManager.GetPrimaryAssetData(GameDefinitionId, /*out*/ Dummy))
	{
		UE_LOG(LogTP_MGFGameDefinition, Error, TEXT("GAME DEFINITION: Wanted to use %s but couldn't find it, falling back to the default)"), *GameDefinitionId.ToString());
		GameDefinitionId = FPrimaryAssetId();
	}

	// Final fallback to the default game definition
	if (!GameDefinitionId.IsValid())
	{
		//@TODO: Pull this from a config setting or something
		GameDefinitionId = FPrimaryAssetId(FPrimaryAssetType("GameDefinition"), FName("DefaultGameDefinition"));
		GameDefinitionIdSource = TEXT("Default");
	}

	// Check and set game definition
	{
		if (GameDefinitionId.IsValid())
		{
			UE_LOG(LogTP_MGFGameDefinition, Log, TEXT("Identified game definition %s (Source: %s)"), *GameDefinitionId.ToString(), *GameDefinitionIdSource);

			UGameDefinitionManagerComponent* GameDefComponent = GameState->FindComponentByClass<UGameDefinitionManagerComponent>();
			check(GameDefComponent);
			GameDefComponent->SetCurrentGameDefinition(GameDefinitionId);
		}
		else
		{
			UE_LOG(LogTP_MGFGameDefinition, Error, TEXT("Failed to identify game definition, loading screen will stay up forever"));
		}
	}

	// TODO: add Save/Load logic for the game
}

UClass* ATP_MGFGameMode::GetDefaultPawnClassForController_Implementation(AController* InController)
{
	if (const UPawnData* PawnData = GetPawnDataForController(InController))
	{
		if (PawnData->PawnClass)
		{
			return PawnData->PawnClass;
		}
	}

	return Super::GetDefaultPawnClassForController_Implementation(InController);
}

APawn* ATP_MGFGameMode::SpawnDefaultPawnAtTransform_Implementation(AController* NewPlayer, const FTransform& SpawnTransform)
{
	FActorSpawnParameters SpawnInfo;
	SpawnInfo.Instigator = GetInstigator();
	SpawnInfo.ObjectFlags |= RF_Transient;	// Never save the default player pawns into a map.
	SpawnInfo.bDeferConstruction = true;

	if (UClass* PawnClass = GetDefaultPawnClassForController(NewPlayer))
	{
		if (APawn* SpawnedPawn = GetWorld()->SpawnActor<APawn>(PawnClass, SpawnTransform, SpawnInfo))
		{
			if (UTP_MGFPawnExtensionComponent* PawnExtComp = UTP_MGFPawnExtensionComponent::FindPawnExtensionComponent(SpawnedPawn))
			{
				if (const UPawnData* PawnData = GetPawnDataForController(NewPlayer))
				{
					PawnExtComp->SetPawnData(PawnData);
				}
				else
				{
					UE_LOG(LogTP_MGF, Error, TEXT("Game mode was unable to set PawnData on the spawned pawn [%s]."), *GetNameSafe(SpawnedPawn));
				}
			}

			SpawnedPawn->FinishSpawning(SpawnTransform);

			return SpawnedPawn;
		}
		else
		{
			UE_LOG(LogTP_MGF, Error, TEXT("Game mode was unable to spawn Pawn of class [%s] at [%s]."), *GetNameSafe(PawnClass), *SpawnTransform.ToHumanReadableString());
		}
	}
	else
	{
		UE_LOG(LogTP_MGF, Error, TEXT("Game mode was unable to spawn Pawn due to NULL pawn class."));
	}

	return nullptr;
}

void ATP_MGFGameMode::HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer)
{
	// Delay starting new players until the game definition has been loaded
	// (players who log in prior to that will be started by OnGameDefinitionLoaded)
	if (IsGameDefinitionLoaded())
	{
		Super::HandleStartingNewPlayer_Implementation(NewPlayer);
	}
}

void ATP_MGFGameMode::GenericPlayerInitialization(AController* NewPlayer)
{
	Super::GenericPlayerInitialization(NewPlayer);

	OnGameModePlayerInitialized.Broadcast(this, NewPlayer);
}

void ATP_MGFGameMode::FailedToRestartPlayer(AController* NewPlayer)
{
	Super::FailedToRestartPlayer(NewPlayer);

	// If we tried to spawn a pawn and it failed, lets try again *note* check if there's actually a pawn class
	// before we try this forever.
	if (UClass* PawnClass = GetDefaultPawnClassForController(NewPlayer))
	{
		if (APlayerController* NewPC = Cast<APlayerController>(NewPlayer))
		{
			// If it's a player don't loop forever, maybe something changed and they can no longer restart if so stop trying.
			if (PlayerCanRestart(NewPC))
			{
				RequestPlayerRestartNextFrame(NewPlayer, false);
			}
			else
			{
				UE_LOG(LogTP_MGF, Verbose, TEXT("FailedToRestartPlayer(%s) and PlayerCanRestart returned false, so we're not going to try again."), *GetPathNameSafe(NewPlayer));
			}
		}
		else
		{
			RequestPlayerRestartNextFrame(NewPlayer, false);
		}
	}
	else
	{
		UE_LOG(LogTP_MGF, Verbose, TEXT("FailedToRestartPlayer(%s) but there's no pawn class so giving up."), *GetPathNameSafe(NewPlayer));
	}
}
