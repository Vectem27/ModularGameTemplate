#pragma once

#include "Engine/DataAsset.h"

#include "PawnData.generated.h"

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
	// Class to instantiate for this pawn (should usually derive from UTP_MGFPawn or UTP_MGFCharacter).
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UTP_MGF|Pawn")
	TSubclassOf<APawn> PawnClass;
};
