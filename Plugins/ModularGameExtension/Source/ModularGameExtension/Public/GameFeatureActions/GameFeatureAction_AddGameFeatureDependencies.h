// Copyright 2025 Martin Bournat. Licensed under the Apache License 2.0.

#pragma once

#include "GameFeatureAction.h"
#include "GameFeaturesSubsystem.h"

#include "GameFeatureAction_AddGameFeatureDependencies.generated.h"

UCLASS(meta = (DisplayName = "Add Game Feature Dependencies"))
class UGameFeatureAction_AddGameFeatureDependencies final : public UGameFeatureAction
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = "Dependencies", meta = (DisplayName = "Game Feature Dependencies", ToolTip = "List of game feature dependencies to add."))
	TArray<FString> GameFeatureDependenciesNames;
};
