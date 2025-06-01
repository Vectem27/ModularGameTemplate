// Copyright 2025 Martin Bournat. Licensed under the Apache License 2.0.

#pragma once

#include "Subsystems/EngineSubsystem.h"
#include "GameFeatureStateChangeObserver.h"

#include "GameFeatureDependenciesManager.generated.h"

/**
 * 
 */
UCLASS()
class MODULARGAMEEXTENSION_API UGameFeatureDependenciesManager : public UEngineSubsystem, public IGameFeatureStateChangeObserver
{
	GENERATED_BODY()
	
public:
	//~UEngineSubsystem interface
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	//~End of UEngineSubsystem interface

	//~IGameFeatureStateChangeObserver interface
	virtual void OnGameFeatureRegistering(const UGameFeatureData* GameFeatureData, const FString& PluginName, const FString& PluginURL) override;
	virtual void OnGameFeatureLoading(const UGameFeatureData* GameFeatureData, const FString& PluginURL) override;
	virtual void OnGameFeatureUnloading(const UGameFeatureData* GameFeatureData, const FString& PluginURL) override;
	virtual void OnGameFeatureActivating(const UGameFeatureData* GameFeatureData, const FString& PluginURL) override;
	virtual void OnGameFeatureDeactivating(const UGameFeatureData* GameFeatureData, FGameFeatureDeactivatingContext& Context, const FString& PluginURL) override;
	//~End of IGameFeatureStateChangeObserver interface

private:
	TArray<FString> GetGameFeaturePluginDependencies(const UGameFeatureData* GameFeatureData) const;

	void RegisterPluginDependencies(const FString& PluginURL, const TArray<FString>& Dependencies);

	void LoadPluginDependencies(const FString& PluginURL, const TArray<FString>& Dependencies);
	void UnloadPluginDependencies(const FString& PluginURL, const TArray<FString>& Dependencies);

	void ActivatePluginDependencies(const FString& PluginURL, const TArray<FString>& Dependencies);
	void DeactivatePluginDependencies(const FString& PluginURL, const TArray<FString>& Dependencies);
private:
	TMap<FString /*PluginURL*/, TArray<FString /*PluginURL*/>> pluginsDependsOn; // Map of plugin names to the list of plugins they depend on
};
