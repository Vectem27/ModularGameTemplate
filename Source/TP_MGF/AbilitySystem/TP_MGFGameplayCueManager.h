#pragma once

#include "GameplayCueManager.h"

#include "TP_MGFGameplayCueManager.generated.h"

/**
 * UTP_MGFGameplayCueManager
 *
 * Game-specific manager for gameplay cues
 */
UCLASS()
class UTP_MGFGameplayCueManager : public UGameplayCueManager
{
	GENERATED_BODY()

public:
	// TODO: remove and define these real function

	UTP_MGFGameplayCueManager(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get()) {}

	static UTP_MGFGameplayCueManager* Get() { return nullptr; }

	void LoadAlwaysLoadedCues() {}
};
