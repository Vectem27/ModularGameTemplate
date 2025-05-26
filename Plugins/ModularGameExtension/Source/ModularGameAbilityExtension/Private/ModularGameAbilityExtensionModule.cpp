// Copyright 2025 Martin Bournat. Licensed under the Apache License 2.0.

#include "ModularGameAbilityExtensionModule.h"

#define LOCTEXT_NAMESPACE "FModularGameAbilityExtensionModule"

void FModularGameAbilityExtensionModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
}

void FModularGameAbilityExtensionModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FModularGameAbilityExtensionModule, ModularGameAbilityExtension)