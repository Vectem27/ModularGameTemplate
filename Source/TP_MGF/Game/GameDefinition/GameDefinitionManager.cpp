#include "Game/GameDefinition/GameDefinitionManager.h"

#include "Engine/Engine.h"
#include "Subsystems/SubsystemCollection.h"

#if WITH_EDITOR

void UGameDefinitionManager::OnPlayInEditorBegun()
{
	ensure(GameFeaturePluginRequestCountMap.IsEmpty());
	GameFeaturePluginRequestCountMap.Empty();
}

void UGameDefinitionManager::NotifyOfPluginActivation(const FString PluginURL)
{
	if (GIsEditor)
	{
		UGameDefinitionManager* GameDefinitionManagerSubsystem = GEngine->GetEngineSubsystem<UGameDefinitionManager>();
		check(GameDefinitionManagerSubsystem);

		// Track the number of requesters who activate this plugin. Multiple load/activation requests are always allowed because concurrent requests are handled.
		int32& Count = GameDefinitionManagerSubsystem->GameFeaturePluginRequestCountMap.FindOrAdd(PluginURL);
		++Count;
	}
}

bool UGameDefinitionManager::RequestToDeactivatePlugin(const FString PluginURL)
{
	if (GIsEditor)
	{
		UGameDefinitionManager* GameDefinitionManagerSubsystem = GEngine->GetEngineSubsystem<UGameDefinitionManager>();
		check(GameDefinitionManagerSubsystem);

		// Only let the last requester to get this far deactivate the plugin
		int32& Count = GameDefinitionManagerSubsystem->GameFeaturePluginRequestCountMap.FindChecked(PluginURL);
		--Count;

		if (Count == 0)
		{
			GameDefinitionManagerSubsystem->GameFeaturePluginRequestCountMap.Remove(PluginURL);
			return true;
		}

		return false;
	}

	return true;
}

#endif