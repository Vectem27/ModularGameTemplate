#pragma once

#include "Engine/DataAsset.h"

#include "GameActionSet.generated.h"

class UGameFeatureAction;

/**
 * Definition of a set of actions to perform as part of entering an game definition
 */
UCLASS(BlueprintType, NotBlueprintable)
class UGameActionSet : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	//~UObject interface
#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(class FDataValidationContext& Context) const override;
#endif
	//~End of UObject interface

	//~UPrimaryDataAsset interface
#if WITH_EDITORONLY_DATA
	virtual void UpdateAssetBundleData() override;
#endif
	//~End of UPrimaryDataAsset interface

public:
	// List of actions to perform as this experience is loaded/activated/deactivated/unloaded
	UPROPERTY(EditAnywhere, Instanced, Category = "Actions to Perform")
	TArray<TObjectPtr<UGameFeatureAction>> Actions;

	// List of Game Feature Plugins this experience wants to have active
	UPROPERTY(EditAnywhere, Category = "Feature Dependencies")
	TArray<FString> GameFeaturesToEnable;
};
