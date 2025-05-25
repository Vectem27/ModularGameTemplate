#include "GameFeatures/TP_MGFGameFeaturePolicy.h"

#include "AbilitySystem/TP_MGFGameplayCueManager.h"
#include "GameFeatureData.h"
#include "GameplayCueSet.h"

UTP_MGFGameFeaturePolicy::UTP_MGFGameFeaturePolicy(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

UTP_MGFGameFeaturePolicy& UTP_MGFGameFeaturePolicy::Get()
{
	return UGameFeaturesSubsystem::Get().GetPolicy<UTP_MGFGameFeaturePolicy>();
}

void UTP_MGFGameFeaturePolicy::InitGameFeatureManager()
{
	// TODO: Uncomment these lines
	//Observers.Add(NewObject<UTP_MGFGameFeature_HotfixManager>());
	//Observers.Add(NewObject<UTP_MGFGameFeature_AddGameplayCuePaths>());

	UGameFeaturesSubsystem& Subsystem = UGameFeaturesSubsystem::Get();
	for (UObject* Observer : Observers)
	{
		Subsystem.AddObserver(Observer);
	}

	Super::InitGameFeatureManager();
}

void UTP_MGFGameFeaturePolicy::ShutdownGameFeatureManager()
{
	Super::ShutdownGameFeatureManager();

	UGameFeaturesSubsystem& Subsystem = UGameFeaturesSubsystem::Get();
	for (UObject* Observer : Observers)
	{
		Subsystem.RemoveObserver(Observer);
	}
	Observers.Empty();
}

TArray<FPrimaryAssetId> UTP_MGFGameFeaturePolicy::GetPreloadAssetListForGameFeature(const UGameFeatureData* GameFeatureToLoad, bool bIncludeLoadedAssets) const
{
	return Super::GetPreloadAssetListForGameFeature(GameFeatureToLoad, bIncludeLoadedAssets);
}

const TArray<FName> UTP_MGFGameFeaturePolicy::GetPreloadBundleStateForGameFeature() const
{
	return Super::GetPreloadBundleStateForGameFeature();
}

void UTP_MGFGameFeaturePolicy::GetGameFeatureLoadingMode(bool& bLoadClientData, bool& bLoadServerData) const
{
	// Editor will load both, this can cause hitching as the bundles are set to not preload in editor
	bLoadClientData = !IsRunningDedicatedServer();
	bLoadServerData = !IsRunningClientOnly();
}

bool UTP_MGFGameFeaturePolicy::IsPluginAllowed(const FString& PluginURL) const
{
	return Super::IsPluginAllowed(PluginURL);
}

//////////////////////////////////////////////////////////////////////
//

//#include "Hotfix/TP_MGFHotfixManager.h"
//
//void UTP_MGFGameFeature_HotfixManager::OnGameFeatureLoading(const UGameFeatureData* GameFeatureData, const FString& PluginURL)
//{
//	if (UTP_MGFHotfixManager* HotfixManager = Cast<UTP_MGFHotfixManager>(UOnlineHotfixManager::Get(nullptr)))
//	{
//		HotfixManager->RequestPatchAssetsFromIniFiles();
//	}
//}

//////////////////////////////////////////////////////////////////////
//

//#include "AbilitySystemGlobals.h"
//#include "GameFeatureAction_AddGameplayCuePath.h"
//
//#include UE_INLINE_GENERATED_CPP_BY_NAME(TP_MGFGameFeaturePolicy)
//
//class FName;
//struct FPrimaryAssetId;
//
//void UTP_MGFGameFeature_AddGameplayCuePaths::OnGameFeatureRegistering(const UGameFeatureData* GameFeatureData, const FString& PluginName, const FString& PluginURL)
//{
//	TRACE_CPUPROFILER_EVENT_SCOPE(UTP_MGFGameFeature_AddGameplayCuePaths::OnGameFeatureRegistering);
//	
//	const FString PluginRootPath = TEXT("/") + PluginName;
//	for (const UGameFeatureAction* Action : GameFeatureData->GetActions())
//	{
//		if (const UGameFeatureAction_AddGameplayCuePath* AddGameplayCueGFA = Cast<UGameFeatureAction_AddGameplayCuePath>(Action))
//		{
//			const TArray<FDirectoryPath>& DirsToAdd = AddGameplayCueGFA->GetDirectoryPathsToAdd();
//			
//			if (UTP_MGFGameplayCueManager* GCM = UTP_MGFGameplayCueManager::Get())
//			{
//				UGameplayCueSet* RuntimeGameplayCueSet = GCM->GetRuntimeCueSet();
//				const int32 PreInitializeNumCues = RuntimeGameplayCueSet ? RuntimeGameplayCueSet->GameplayCueData.Num() : 0;
//
//				for (const FDirectoryPath& Directory : DirsToAdd)
//				{
//					FString MutablePath = Directory.Path;
//					UGameFeaturesSubsystem::FixPluginPackagePath(MutablePath, PluginRootPath, false);
//					GCM->AddGameplayCueNotifyPath(MutablePath, /** bShouldRescanCueAssets = */ false);	
//				}
//				
//				// Rebuild the runtime library with these new paths
//				if (!DirsToAdd.IsEmpty())
//				{
//					GCM->InitializeRuntimeObjectLibrary();	
//				}
//
//				const int32 PostInitializeNumCues = RuntimeGameplayCueSet ? RuntimeGameplayCueSet->GameplayCueData.Num() : 0;
//				if (PreInitializeNumCues != PostInitializeNumCues)
//				{
//					GCM->RefreshGameplayCuePrimaryAsset();
//				}
//			}
//		}
//	}
//}

//void UTP_MGFGameFeature_AddGameplayCuePaths::OnGameFeatureUnregistering(const UGameFeatureData* GameFeatureData, const FString& PluginName, const FString& PluginURL)
//{
//	const FString PluginRootPath = TEXT("/") + PluginName;
//	for (const UGameFeatureAction* Action : GameFeatureData->GetActions())
//	{
//		if (const UGameFeatureAction_AddGameplayCuePath* AddGameplayCueGFA = Cast<UGameFeatureAction_AddGameplayCuePath>(Action))
//		{
//			const TArray<FDirectoryPath>& DirsToAdd = AddGameplayCueGFA->GetDirectoryPathsToAdd();
//			
//			if (UGameplayCueManager* GCM = UAbilitySystemGlobals::Get().GetGameplayCueManager())
//			{
//				int32 NumRemoved = 0;
//				for (const FDirectoryPath& Directory : DirsToAdd)
//				{
//					FString MutablePath = Directory.Path;
//					UGameFeaturesSubsystem::FixPluginPackagePath(MutablePath, PluginRootPath, false);
//					NumRemoved += GCM->RemoveGameplayCueNotifyPath(MutablePath, /** bShouldRescanCueAssets = */ false);
//				}
//
//				ensure(NumRemoved == DirsToAdd.Num());
//				
//				// Rebuild the runtime library only if there is a need to
//				if (NumRemoved > 0)
//				{
//					GCM->InitializeRuntimeObjectLibrary();	
//				}			
//			}
//		}
//	}
//}
