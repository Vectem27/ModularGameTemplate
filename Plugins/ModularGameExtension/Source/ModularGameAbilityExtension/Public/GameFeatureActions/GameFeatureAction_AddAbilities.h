#pragma once

#include "GameFeatureActions/GameFeatureAction_WorldActionBase.h"

#include "GameFeatureAction_AddAbilities.generated.h"

class UGameplayAbility;
class UInputAction;
class UAttributeSet;
class UDataTable;
class UAbilitySet;

USTRUCT(BlueprintType)
struct FAbilityGrant
{
	GENERATED_BODY()

	// Type of ability to grant
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AssetBundles = "Client,Server"))
	TSoftClassPtr<UGameplayAbility> AbilityType;

	// Input action to bind the ability to, if any (can be left unset)
 	UPROPERTY(EditAnywhere, BlueprintReadOnly)
 	TSoftObjectPtr<UInputAction> InputAction;
};

USTRUCT(BlueprintType)
struct FAttributeSetGrant
{
	GENERATED_BODY()

	// Ability set to grant
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AssetBundles = "Client,Server"))
	TSoftClassPtr<UAttributeSet> AttributeSetType;

	// Data table referent to initialize the attributes with, if any (can be left unset)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AssetBundles = "Client,Server"))
	TSoftObjectPtr<UDataTable> InitializationData;
};

USTRUCT()
struct FGameFeatureAbilitiesEntry
{
	GENERATED_BODY()

	// The base actor class to add to
	UPROPERTY(EditAnywhere, Category = "Abilities")
	TSoftClassPtr<AActor> ActorClass;

	// List of abilities to grant to actors of the specified class
	UPROPERTY(EditAnywhere, Category = "Abilities")
	TArray<FAbilityGrant> GrantedAbilities;

	// List of attribute sets to grant to actors of the specified class 
	UPROPERTY(EditAnywhere, Category = "Attributes")
	TArray<FAttributeSetGrant> GrantedAttributes;
};

//////////////////////////////////////////////////////////////////////
// UGameFeatureAction_AddAbilities

/**
 * GameFeatureAction responsible for granting abilities (and attributes) to actors of a specified type.
 */
UCLASS(MinimalAPI, meta = (DisplayName = "Add Abilities"))
class UGameFeatureAction_AddAbilities final : public UGameFeatureAction_WorldActionBase
{
	GENERATED_BODY()

public:
	//~ Begin UGameFeatureAction interface
	virtual void OnGameFeatureActivating(FGameFeatureActivatingContext& Context) override;
	virtual void OnGameFeatureDeactivating(FGameFeatureDeactivatingContext& Context) override;
	//~ End UGameFeatureAction interface

	//~ Begin UObject interface
#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(class FDataValidationContext& Context) const override;
#endif
	//~ End UObject interface

	/**  */
	UPROPERTY(EditAnywhere, Category = "Abilities", meta = (TitleProperty = "ActorClass", ShowOnlyInnerProperties))
	TArray<FGameFeatureAbilitiesEntry> AbilitiesList;
	
protected:
	//~ Begin UGameFeatureAction_WorldActionBase interface
	virtual void AddToWorld(const FWorldContext& WorldContext, const FGameFeatureStateChangeContext& ChangeContext) override;
	//~ End UGameFeatureAction_WorldActionBase interface
};
