// Copyright 2025 Martin Bournat. Licensed under the Apache License 2.0.

#include "GameFeatureDependenciesManager.h"

// Engine includes
#include "GameFeaturePluginOperationResult.h"
#include "GameFeaturesSubsystem.h"
#include "GameFeatureData.h"

// Plugin includes
#include "GameFeatureActions/GameFeatureAction_AddGameFeatureDependencies.h"

///////////////////////////////////////////////////////////////////////////////////
// UEngineSubsystem methods

void UGameFeatureDependenciesManager::Initialize(FSubsystemCollectionBase& Collection)
{
	UGameFeaturesSubsystem* GameFeatureSubsystem = GEngine->GetEngineSubsystem<UGameFeaturesSubsystem>();
	GameFeatureSubsystem->AddObserver(this);
}

void UGameFeatureDependenciesManager::Deinitialize()
{
	UGameFeaturesSubsystem* GameFeatureSubsystem = GEngine->GetEngineSubsystem<UGameFeaturesSubsystem>();
	GameFeatureSubsystem->RemoveObserver(this);
}

///////////////////////////////////////////////////////////////////////////////////
// IGameFeaturesSubsystemObserver methods

void UGameFeatureDependenciesManager::OnGameFeatureRegistering(const UGameFeatureData* GameFeatureData, const FString& PluginName, const FString& PluginURL)
{
	UGameFeaturesSubsystem* GameFeatureSubsystem = GEngine->GetEngineSubsystem<UGameFeaturesSubsystem>();

	for (auto action : GameFeatureData->GetActions())
	{
		if (IsValid(action) && action->GetClass() == UGameFeatureAction_AddGameFeatureDependencies::StaticClass())
		{
			UGameFeatureAction_AddGameFeatureDependencies* AddDependenciesAction = Cast<UGameFeatureAction_AddGameFeatureDependencies>(action);

			for (const FString& Dependency : AddDependenciesAction->GameFeatureDependencieNames)
			{
				// Add to plugins depends on this map
				FString DepURL;
				GameFeatureSubsystem->GetPluginURLByName(Dependency, DepURL);
				TArray<FString>& pluginsDependsOnThis = pluginsDependsOn.FindOrAdd(DepURL);
				pluginsDependsOnThis.AddUnique(PluginURL);
			}
		}
	}

	TArray<FString> dependencies = GetGameFeaturePluginDependencies(GameFeatureData);
	RegisterPluginDependencies(PluginURL, dependencies);
}

void UGameFeatureDependenciesManager::OnGameFeatureLoading(const UGameFeatureData* GameFeatureData, const FString& PluginURL)
{
	TArray<FString> dependencies = GetGameFeaturePluginDependencies(GameFeatureData);
	LoadPluginDependencies(PluginURL, dependencies);
}

void UGameFeatureDependenciesManager::OnGameFeatureUnloading(const UGameFeatureData* GameFeatureData, const FString& PluginURL)
{
	TArray<FString> dependencies = GetGameFeaturePluginDependencies(GameFeatureData);
	UnloadPluginDependencies(PluginURL, dependencies);
}

void UGameFeatureDependenciesManager::OnGameFeatureActivating(const UGameFeatureData* GameFeatureData, const FString& PluginURL)
{
	TArray<FString> dependencies = GetGameFeaturePluginDependencies(GameFeatureData);
	ActivatePluginDependencies(PluginURL, dependencies);
}

void UGameFeatureDependenciesManager::OnGameFeatureDeactivating(const UGameFeatureData* GameFeatureData, FGameFeatureDeactivatingContext& Context, const FString& PluginURL)
{
	TArray<FString> dependencies = GetGameFeaturePluginDependencies(GameFeatureData);
	DeactivatePluginDependencies(PluginURL, dependencies);
}

///////////////////////////////////////////////////////////////////////////////////

TArray<FString> UGameFeatureDependenciesManager::GetGameFeaturePluginDependencies(const UGameFeatureData* GameFeatureData) const
{
	TArray<FString> dependencieURLs;
	UGameFeaturesSubsystem* GameFeatureSubsystem = GEngine->GetEngineSubsystem<UGameFeaturesSubsystem>();

	for (auto action : GameFeatureData->GetActions())
	{
		if (IsValid(action) && action->GetClass() == UGameFeatureAction_AddGameFeatureDependencies::StaticClass())
		{
			UGameFeatureAction_AddGameFeatureDependencies* AddDependenciesAction = Cast<UGameFeatureAction_AddGameFeatureDependencies>(action);

			for (const FString& DependencyName : AddDependenciesAction->GameFeatureDependencieNames)
			{
				FString PluginURL;
				GameFeatureSubsystem->GetPluginURLByName(DependencyName, PluginURL);
				dependencieURLs.AddUnique(PluginURL);
			}
		}
	}

	return dependencieURLs;
}

void UGameFeatureDependenciesManager::RegisterPluginDependencies(const FString& PluginURL, const TArray<FString>& Dependencies)
{
	UGameFeaturesSubsystem* GameFeatureSubsystem = GEngine->GetEngineSubsystem<UGameFeaturesSubsystem>();
	for (auto dep : Dependencies)
	{
		GameFeatureSubsystem->RegisterGameFeaturePlugin(
			dep,
			FGameFeaturePluginChangeStateComplete::CreateLambda([](const UE::GameFeatures::FResult& result)
				{
					ensure(!result.HasError());
				})
		);
	}
}


void UGameFeatureDependenciesManager::LoadPluginDependencies(const FString& PluginURL, const TArray<FString>& Dependencies)
{
	UGameFeaturesSubsystem* GameFeatureSubsystem = GEngine->GetEngineSubsystem<UGameFeaturesSubsystem>();
	for (auto dep : Dependencies)
	{
		GameFeatureSubsystem->LoadGameFeaturePlugin(
			dep,
			FGameFeaturePluginChangeStateComplete::CreateLambda([](const UE::GameFeatures::FResult& result)
				{
					ensure(!result.HasError());
				})
		);
	}
}

void UGameFeatureDependenciesManager::UnloadPluginDependencies(const FString& PluginURL, const TArray<FString>& Dependencies)
{
	UGameFeaturesSubsystem* GameFeatureSubsystem = GEngine->GetEngineSubsystem<UGameFeaturesSubsystem>();
	for (auto dep : Dependencies)
	{
		// Check if another loaded plugin depends on this
		bool canProcess = true;
		if (TArray<FString>* pluginsDepOnThis = pluginsDependsOn.Find(dep))
		{
			for (auto plugin : *pluginsDepOnThis)
			{
				if (GameFeatureSubsystem->IsGameFeaturePluginLoaded(plugin) || GameFeatureSubsystem->IsGameFeaturePluginActive(plugin))
				{
					canProcess = false;
					break;
				}
			}
		}

		// unload if it can
		if (canProcess)
		{
			GameFeatureSubsystem->ChangeGameFeatureTargetState(
				dep,
				EGameFeatureTargetState::Registered,
				FGameFeaturePluginChangeStateComplete::CreateLambda([](const UE::GameFeatures::FResult& Result)
					{
						ensure(!Result.HasError());
					})
			);
		}
	}
}

void UGameFeatureDependenciesManager::ActivatePluginDependencies(const FString& PluginURL, const TArray<FString>& Dependencies)
{
	UGameFeaturesSubsystem* GameFeatureSubsystem = GEngine->GetEngineSubsystem<UGameFeaturesSubsystem>();
	for (auto dep : Dependencies)
	{
		GameFeatureSubsystem->LoadAndActivateGameFeaturePlugin(
			dep,
			FGameFeaturePluginChangeStateComplete::CreateLambda([](const UE::GameFeatures::FResult& result)
				{
					ensure(!result.HasError());
				})
		);
	}
}

void UGameFeatureDependenciesManager::DeactivatePluginDependencies(const FString& PluginURL, const TArray<FString>& Dependencies)
{
	UGameFeaturesSubsystem* GameFeatureSubsystem = GEngine->GetEngineSubsystem<UGameFeaturesSubsystem>();
	for (auto dep : Dependencies)
	{
		// Check if another activated plugin depends on this
		bool canProcess = true;
		if (TArray<FString>* pluginsDepOnThis = pluginsDependsOn.Find(dep))
		{
			for (auto plugin : *pluginsDepOnThis)
			{
				if (GameFeatureSubsystem->IsGameFeaturePluginActive(plugin))
				{
					canProcess = false;
					break;
				}
			}
		}

		// deactivate if it can
		if (canProcess)
		{
			GameFeatureSubsystem->DeactivateGameFeaturePlugin(
				dep,
				FGameFeaturePluginChangeStateComplete::CreateLambda([](const UE::GameFeatures::FResult& result)
				{
					ensure(!result.HasError());
				})
			);
		}	
	}
}
