#pragma once

#include "Components/GameStateComponent.h"
#include "LoadingProcessInterface.h"

#include "GameDefinitionManagerComponent.generated.h"

namespace UE::GameFeatures { struct FResult; }

class UGameDefinition;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnGameDefinitionLoaded, const UGameDefinition* /*GameDefinition*/);

enum class EGameDefinitionLoadState
{
	Unloaded,
	Loading,
	LoadingGameFeatures,
	ExecutingActions,
	Loaded,
	Deactivating
};

UCLASS()
class TP_MGF_API UGameDefinitionManagerComponent final : public UGameStateComponent, public ILoadingProcessInterface
{
	GENERATED_BODY()

public:

	UGameDefinitionManagerComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	//~UActorComponent interface
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	//~End of UActorComponent interface

	//~ILoadingProcessInterface interface
	virtual bool ShouldShowLoadingScreen(FString& OutReason) const override;
	//~End of ILoadingProcessInterface

	// Tries to set the current experience, either a UI or gameplay one
	void SetCurrentGameDefinition(FPrimaryAssetId GameDefinitionId);

	// Ensures the delegate is called once the experience has been loaded,
	// before others are called.
	// However, if the experience has already loaded, calls the delegate immediately.
	void CallOrRegister_OnGameDefinitionLoaded_HighPriority(FOnGameDefinitionLoaded::FDelegate&& Delegate);

	// Ensures the delegate is called once the experience has been loaded
	// If the experience has already loaded, calls the delegate immediately
	void CallOrRegister_OnGameDefinitionLoaded(FOnGameDefinitionLoaded::FDelegate&& Delegate);

	// Ensures the delegate is called once the experience has been loaded
	// If the experience has already loaded, calls the delegate immediately
	void CallOrRegister_OnGameDefinitionLoaded_LowPriority(FOnGameDefinitionLoaded::FDelegate&& Delegate);

	// This returns the current experience if it is fully loaded, asserting otherwise
	// (i.e., if you called it too soon)
	const UGameDefinition* GetCurrentGameDefinitionChecked() const;

	// Returns true if the experience is fully loaded
	bool IsGameDefinitionLoaded() const;

private:
	void StartGameDefinitionLoad();
	void OnGameDefinitionLoadComplete();
	void OnGameFeaturePluginLoadComplete(const UE::GameFeatures::FResult& Result);
	void OnGameDefinitionFullLoadCompleted();

	void OnActionDeactivationCompleted();
	void OnAllActionsDeactivated();

private:
	UPROPERTY()
	TObjectPtr<const UGameDefinition> CurrentGameDefinition;

	EGameDefinitionLoadState LoadState = EGameDefinitionLoadState::Unloaded;

	int32 NumGameFeaturePluginsLoading = 0;
	TArray<FString> GameFeaturePluginURLs;

	int32 NumObservedPausers = 0;
	int32 NumExpectedPausers = 0;

	/**
	 * Delegate called when the game definition has finished loading just before others
	 * (e.g., subsystems that set up for regular gameplay)
	 */
	FOnGameDefinitionLoaded OnGameDefinitionLoaded_HighPriority;

	/** Delegate called when the game definition has finished loading */
	FOnGameDefinitionLoaded OnGameDefinitionLoaded;

	/** Delegate called when the game definition has finished loading */
	FOnGameDefinitionLoaded OnGameDefinitionLoaded_LowPriority;
};
