#pragma once

#include "GameFeatureAction.h"
#include "GameFeaturesSubsystem.h"

#include "GameFeatureAction_AddInputContextMapping.generated.h"

class AActor;
class UInputMappingContext;

USTRUCT()
struct TP_MGF_API FInputMappingContextAndPriority
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category="Input")
	TSoftObjectPtr<UInputMappingContext> InputMapping;

	// Higher priority input mappings will be prioritized over mappings with a lower priority.
	UPROPERTY(EditAnywhere, Category="Input")
	int32 Priority = 0;
	
	/** If true, then this mapping context will be registered with the settings when this game feature action is registered. */
	UPROPERTY(EditAnywhere, Category="Input")
	bool bRegisterWithSettings = true;
};

/**
 * Adds InputMappingContext to local players' EnhancedInput system. 
 * Expects that local players are set up to use the EnhancedInput system.
 */
UCLASS(meta = (DisplayName = "Add Input Mapping"))
class TP_MGF_API UGameFeatureAction_AddInputContextMapping final : public UGameFeatureAction
{
	GENERATED_BODY()

public:
	//~UGameFeatureAction interface
	virtual void OnGameFeatureRegistering() override;
	virtual void OnGameFeatureActivating(FGameFeatureActivatingContext& Context) override;
	virtual void OnGameFeatureDeactivating(FGameFeatureDeactivatingContext& Context) override;
	virtual void OnGameFeatureUnregistering() override;
	//~End of UGameFeatureAction interface

	//~UObject interface
#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(class FDataValidationContext& Context) const override;
#endif
	//~End of UObject interface

	UPROPERTY(EditAnywhere, Category="Input")
	TArray<FInputMappingContextAndPriority> InputMappings;

private:
	void HandleGameInstanceStartRegister(UGameInstance* GameInstance);
	void HandleGameInstanceStartActivate(UGameInstance* GameInstance, FGameFeatureStateChangeContext ChangeContext);

	void AddMappingToLocalPlayer(ULocalPlayer* localPlayer);
	void RemoveMappingToLocalPlayer(ULocalPlayer* localPlayer);

	void RegisterMappingToLocalPlayer(ULocalPlayer* localPlayer);
	void UnregisterMappingToLocalPlayer(ULocalPlayer* localPlayer);

	/** Delegate for when the game instance is changed to register IMC's */
	FDelegateHandle RegisterInputContextMappingsForGameInstanceHandle;

	/** Delegate for when the game instance is changed to register IMC's */
	FDelegateHandle ActivateInputContextMappingsForGameInstanceHandle;
};
