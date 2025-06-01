#pragma once

#include "Engine/DataAsset.h"

#include "PawnData.generated.h"

class UTP_MGFAbilitySet;
class UTP_MGFAbilityTagRelationshipMapping;
class UTP_MGFInputConfig;
class UTP_MGFCameraMode;

/**
 * UPawnData
 *
 *	Non-mutable data asset that contains properties used to define a pawn.
 */
UCLASS(BlueprintType, Const, Meta = (DisplayName = "Pawn Data", ShortTooltip = "Data asset used to define a Pawn."))
class UPawnData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPawnData(const FObjectInitializer& ObjectInitializer);

	// Class to instantiate for this pawn (should usually derive from UTP_MGFPawn or UTP_MGFCharacter).
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UTP_MGF|Pawn")
	TSubclassOf<APawn> PawnClass;

	// Ability sets to grant to this pawn's ability system.
	//UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TP_MGF|Abilities")
	//TArray<TObjectPtr<UTP_MGFAbilitySet>> AbilitySets;

	// What mapping of ability tags to use for actions taking by this pawn
	//UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TP_MGF|Abilities")
	//TObjectPtr<UTP_MGFAbilityTagRelationshipMapping> TagRelationshipMapping;
};
