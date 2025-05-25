#include "Game/GameDefinition/GameDefinitionManagerComponent.h"

// Engine includes
#include "Engine/World.h"
#include "GameFeaturesSubsystem.h"
#include "GameFeatureAction.h"
#include "GameFeaturesSubsystemSettings.h"
#include "TimerManager.h"

// Project includes
#include "Game/GameDefinition/GameActionSet.h"
#include "Game/GameDefinition/GameDefinition.h"
#include "Game/GameDefinition/GameDefinitionManager.h"
#include "System/TP_MGFAssetManager.h"
#include "Settings/TP_MGFSettingsLocal.h"
#include "TP_MGFLogChannels.h"

UGameDefinitionManagerComponent::UGameDefinitionManagerComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UGameDefinitionManagerComponent::SetCurrentGameDefinition(FPrimaryAssetId GameDefinitionId)
{
	UTP_MGFAssetManager& AssetManager = UTP_MGFAssetManager::Get();
	FSoftObjectPath AssetPath = AssetManager.GetPrimaryAssetPath(GameDefinitionId);

	UObject* LoadedAsset = AssetPath.TryLoad();
	check(LoadedAsset);

	const UGameDefinition* GameDefinition = CastChecked<UGameDefinition>(LoadedAsset);

	check(CurrentGameDefinition == nullptr);
	CurrentGameDefinition = GameDefinition;
	StartGameDefinitionLoad();
}

void UGameDefinitionManagerComponent::CallOrRegister_OnGameDefinitionLoaded_HighPriority(FOnGameDefinitionLoaded::FDelegate&& Delegate)
{
	if (IsGameDefinitionLoaded())
	{
		Delegate.Execute(CurrentGameDefinition);
	}
	else
	{
		OnGameDefinitionLoaded_HighPriority.Add(MoveTemp(Delegate));
	}
}

void UGameDefinitionManagerComponent::CallOrRegister_OnGameDefinitionLoaded(FOnGameDefinitionLoaded::FDelegate&& Delegate)
{
	if (IsGameDefinitionLoaded())
		Delegate.Execute(CurrentGameDefinition);
	else
		OnGameDefinitionLoaded.Add(MoveTemp(Delegate));
}

void UGameDefinitionManagerComponent::CallOrRegister_OnGameDefinitionLoaded_LowPriority(FOnGameDefinitionLoaded::FDelegate&& Delegate)
{
	if (IsGameDefinitionLoaded())
		Delegate.Execute(CurrentGameDefinition);
	else
		OnGameDefinitionLoaded_LowPriority.Add(MoveTemp(Delegate));
}

const UGameDefinition* UGameDefinitionManagerComponent::GetCurrentGameDefinitionChecked() const
{
	check(LoadState == EGameDefinitionLoadState::Loaded);
	check(CurrentGameDefinition != nullptr);
	return CurrentGameDefinition;
}

bool UGameDefinitionManagerComponent::IsGameDefinitionLoaded() const
{
	return (LoadState == EGameDefinitionLoadState::Loaded) && (CurrentGameDefinition != nullptr);
}


void UGameDefinitionManagerComponent::StartGameDefinitionLoad()
{
	check(CurrentGameDefinition != nullptr);
	check(LoadState == EGameDefinitionLoadState::Unloaded);

	UE_LOG(LogTP_MGFGameDefinition, Log, TEXT("GAME DEFINITION: StartGameDefinitionLoad(CurrentGameDefinition = %s)"),
		*CurrentGameDefinition->GetPrimaryAssetId().ToString());

	LoadState = EGameDefinitionLoadState::Loading;

	UTP_MGFAssetManager& AssetManager = UTP_MGFAssetManager::Get();

	TSet<FPrimaryAssetId> BundleAssetList;
	TSet<FSoftObjectPath> RawAssetList;

	// Load assets associated with the game definition

	TArray<FName> BundlesToLoad;
	BundlesToLoad.Add(FTP_MGFBundles::Equipped);

	//@TODO: Centralize this client/server stuff into the TP_MGFAssetManager
	const ENetMode OwnerNetMode = GetOwner()->GetNetMode();
	const bool bLoadClient = GIsEditor || (OwnerNetMode != NM_DedicatedServer);
	const bool bLoadServer = GIsEditor || (OwnerNetMode != NM_Client);
	if (bLoadClient)
	{
		BundlesToLoad.Add(UGameFeaturesSubsystemSettings::LoadStateClient);
	}
	if (bLoadServer)
	{
		BundlesToLoad.Add(UGameFeaturesSubsystemSettings::LoadStateServer);
	}

	TSharedPtr<FStreamableHandle> BundleLoadHandle = nullptr;
	if (BundleAssetList.Num() > 0)
	{
		BundleLoadHandle = AssetManager.ChangeBundleStateForPrimaryAssets(BundleAssetList.Array(), BundlesToLoad, {}, false, FStreamableDelegate(), FStreamableManager::AsyncLoadHighPriority);
	}

	TSharedPtr<FStreamableHandle> RawLoadHandle = nullptr;
	if (RawAssetList.Num() > 0)
	{
		RawLoadHandle = AssetManager.LoadAssetList(RawAssetList.Array(), FStreamableDelegate(), FStreamableManager::AsyncLoadHighPriority, TEXT("StartGameDefinitionLoad()"));
	}

	// If both async loads are running, combine them
	TSharedPtr<FStreamableHandle> Handle = nullptr;
	if (BundleLoadHandle.IsValid() && RawLoadHandle.IsValid())
	{
		Handle = AssetManager.GetStreamableManager().CreateCombinedHandle({ BundleLoadHandle, RawLoadHandle });
	}
	else
	{
		Handle = BundleLoadHandle.IsValid() ? BundleLoadHandle : RawLoadHandle;
	}

	FStreamableDelegate OnAssetsLoadedDelegate = FStreamableDelegate::CreateUObject(this, &ThisClass::OnGameDefinitionLoadComplete);
	if (!Handle.IsValid() || Handle->HasLoadCompleted())
	{
		// Assets were already loaded, call the delegate now
		FStreamableHandle::ExecuteDelegate(OnAssetsLoadedDelegate);
	}
	else
	{
		Handle->BindCompleteDelegate(OnAssetsLoadedDelegate);

		Handle->BindCancelDelegate(FStreamableDelegate::CreateLambda([OnAssetsLoadedDelegate]()
			{
				OnAssetsLoadedDelegate.ExecuteIfBound();
			}));
	}

	// This set of assets gets preloaded, but we don't block the start of the game definition based on it
	TSet<FPrimaryAssetId> PreloadAssetList;
	//@TODO: Determine assets to preload (but not blocking-ly)
	if (PreloadAssetList.Num() > 0)
	{
		AssetManager.ChangeBundleStateForPrimaryAssets(PreloadAssetList.Array(), BundlesToLoad, {});
	}
}

void UGameDefinitionManagerComponent::OnGameDefinitionLoadComplete()
{
	check(LoadState == EGameDefinitionLoadState::Loading);
	check(CurrentGameDefinition != nullptr);

	UE_LOG(LogTP_MGFGameDefinition, Log, TEXT("GAME DEFINITION: OnGameDefinitionLoadComplete(CurrentGameDefinition = %s)"),
		*CurrentGameDefinition->GetPrimaryAssetId().ToString());

	// find the URLs for our GameFeaturePlugins - filtering out dupes and ones that don't have a valid mapping
	GameFeaturePluginURLs.Reset();

	auto CollectGameFeaturePluginURLs = [This = this](const UPrimaryDataAsset* Context, const TArray<FString>& FeaturePluginList)
		{
			for (const FString& PluginName : FeaturePluginList)
			{
				FString PluginURL;
				if (UGameFeaturesSubsystem::Get().GetPluginURLByName(PluginName, /*out*/ PluginURL))
				{
					This->GameFeaturePluginURLs.AddUnique(PluginURL);
				}
				else
				{
					ensureMsgf(false, TEXT("OnGameDefinitionLoadComplete failed to find plugin URL from PluginName %s for game definition %s - fix data, ignoring for this run"), *PluginName, *Context->GetPrimaryAssetId().ToString());
				}
			}

			// 		// Add in our extra plugin
			// 		if (!CurrentPlaylistData->GameFeaturePluginToActivateUntilDownloadedContentIsPresent.IsEmpty())
			// 		{
			// 			FString PluginURL;
			// 			if (UGameFeaturesSubsystem::Get().GetPluginURLByName(CurrentPlaylistData->GameFeaturePluginToActivateUntilDownloadedContentIsPresent, PluginURL))
			// 			{
			// 				GameFeaturePluginURLs.AddUnique(PluginURL);
			// 			}
			// 		}
		};

	CollectGameFeaturePluginURLs(CurrentGameDefinition, CurrentGameDefinition->GameFeaturesToEnable);

	// Load and activate the features	
	NumGameFeaturePluginsLoading = GameFeaturePluginURLs.Num();
	if (NumGameFeaturePluginsLoading > 0)
	{
		LoadState = EGameDefinitionLoadState::LoadingGameFeatures;
		for (const FString& PluginURL : GameFeaturePluginURLs)
		{
			UGameDefinitionManager::NotifyOfPluginActivation(PluginURL);
			UGameFeaturesSubsystem::Get().LoadAndActivateGameFeaturePlugin(PluginURL, FGameFeaturePluginLoadComplete::CreateUObject(this, &ThisClass::OnGameFeaturePluginLoadComplete));
		}
	}
	else
	{
		OnGameDefinitionFullLoadCompleted();
	}
}

void UGameDefinitionManagerComponent::OnGameFeaturePluginLoadComplete(const UE::GameFeatures::FResult& Result)
{
	// decrement the number of plugins that are loading
	NumGameFeaturePluginsLoading--;

	if (NumGameFeaturePluginsLoading == 0)
	{
		OnGameDefinitionFullLoadCompleted();
	}
}

void UGameDefinitionManagerComponent::OnGameDefinitionFullLoadCompleted()
{
	check(LoadState != EGameDefinitionLoadState::Loaded);

	LoadState = EGameDefinitionLoadState::ExecutingActions;

	// Execute the actions
	FGameFeatureActivatingContext Context;

	// Only apply to our specific world context if set
	const FWorldContext* ExistingWorldContext = GEngine->GetWorldContextFromWorld(GetWorld());
	if (ExistingWorldContext)
	{
		Context.SetRequiredWorldContextHandle(ExistingWorldContext->ContextHandle);
	}

	auto ActivateListOfActions = [&Context](const TArray<UGameFeatureAction*>& ActionList)
		{
			for (UGameFeatureAction* Action : ActionList)
			{
				if (Action != nullptr)
				{
					//@TODO: The fact that these don't take a world are potentially problematic in client-server PIE
					// The current behavior matches systems like gameplay tags where loading and registering apply to the entire process,
					// but actually applying the results to actors is restricted to a specific world
					Action->OnGameFeatureRegistering();
					Action->OnGameFeatureLoading();
					Action->OnGameFeatureActivating(Context);
				}
			}
		};

	ActivateListOfActions(CurrentGameDefinition->Actions);
	for (const TObjectPtr<UGameActionSet>& ActionSet : CurrentGameDefinition->ActionSets)
	{
		if (ActionSet != nullptr)
		{
			ActivateListOfActions(ActionSet->Actions);
		}
	}

	LoadState = EGameDefinitionLoadState::Loaded;

	OnGameDefinitionLoaded_HighPriority.Broadcast(CurrentGameDefinition);
	OnGameDefinitionLoaded_HighPriority.Clear();

	OnGameDefinitionLoaded.Broadcast(CurrentGameDefinition);
	OnGameDefinitionLoaded.Clear();

	OnGameDefinitionLoaded_LowPriority.Broadcast(CurrentGameDefinition);
	OnGameDefinitionLoaded_LowPriority.Clear();

	// Apply any necessary scalability settings
#if !UE_SERVER
	UTP_MGFSettingsLocal::Get()->OnGameDefinitionLoaded();
#endif
}

void UGameDefinitionManagerComponent::OnActionDeactivationCompleted()
{
	check(IsInGameThread());
	++NumObservedPausers;

	if (NumObservedPausers == NumExpectedPausers)
	{
		OnAllActionsDeactivated();
	}
}

void UGameDefinitionManagerComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	// deactivate any features this game definition loaded
	//@TODO: This should be handled FILO as well
	for (const FString& PluginURL : GameFeaturePluginURLs)
	{
		if (UGameDefinitionManager::RequestToDeactivatePlugin(PluginURL))
		{
			UGameFeaturesSubsystem::Get().DeactivateGameFeaturePlugin(PluginURL);
		}
	}

	//@TODO: Ensure proper handling of a partially-loaded state too
	if (LoadState == EGameDefinitionLoadState::Loaded)
	{
		LoadState = EGameDefinitionLoadState::Deactivating;

		// Make sure we won't complete the transition prematurely if someone registers as a pauser but fires immediately
		NumExpectedPausers = INDEX_NONE;
		NumObservedPausers = 0;

		// Deactivate and unload the actions
		FGameFeatureDeactivatingContext Context(TEXT(""), [this](FStringView) { this->OnActionDeactivationCompleted(); });

		const FWorldContext* ExistingWorldContext = GEngine->GetWorldContextFromWorld(GetWorld());
		if (ExistingWorldContext)
		{
			Context.SetRequiredWorldContextHandle(ExistingWorldContext->ContextHandle);
		}

		auto DeactivateListOfActions = [&Context](const TArray<UGameFeatureAction*>& ActionList)
			{
				for (UGameFeatureAction* Action : ActionList)
				{
					if (Action)
					{
						Action->OnGameFeatureDeactivating(Context);
						Action->OnGameFeatureUnregistering();
					}
				}
			};

		DeactivateListOfActions(CurrentGameDefinition->Actions);
		for (const TObjectPtr<UGameActionSet>& ActionSet : CurrentGameDefinition->ActionSets)
		{
			if (ActionSet != nullptr)
			{
				DeactivateListOfActions(ActionSet->Actions);
			}
		}

		NumExpectedPausers = Context.GetNumPausers();

		if (NumExpectedPausers > 0)
		{
			UE_LOG(LogTP_MGFGameDefinition, Error, TEXT("Actions that have asynchronous deactivation aren't fully supported yet in TP_MGF game definitions"));
		}

		if (NumExpectedPausers == NumObservedPausers)
		{
			OnAllActionsDeactivated();
		}
	}
}

bool UGameDefinitionManagerComponent::ShouldShowLoadingScreen(FString& OutReason) const
{
	if (LoadState != EGameDefinitionLoadState::Loaded)
	{
		OutReason = TEXT("GameDefinition still loading");
		return true;
	}
	else
	{
		return false;
	}
}

void UGameDefinitionManagerComponent::OnAllActionsDeactivated()
{
	//@TODO: We actually only deactivated and didn't fully unload...
	LoadState = EGameDefinitionLoadState::Unloaded;
	CurrentGameDefinition = nullptr;
	//@TODO:	GEngine->ForceGarbageCollection(true);
}

