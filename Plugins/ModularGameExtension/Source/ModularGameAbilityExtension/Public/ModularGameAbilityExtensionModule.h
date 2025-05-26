// Copyright 2025 Martin Bournat. Licensed under the Apache License 2.0.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FModularGameAbilityExtensionModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
