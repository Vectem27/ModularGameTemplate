// Copyright 2025 Martin Bournat. Licensed under the Apache License 2.0.

#pragma once

#include "Modules/ModuleManager.h"

class FModularGameExtensionModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
