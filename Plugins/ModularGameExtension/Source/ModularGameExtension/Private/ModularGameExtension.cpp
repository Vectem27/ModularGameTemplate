// Copyright 2025 Martin Bournat. Licensed under the Apache License 2.0.

#include "ModularGameExtension.h"

#define LOCTEXT_NAMESPACE "FModularGameExtensionModule"

void FModularGameExtensionModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
}

void FModularGameExtensionModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FModularGameExtensionModule, ModularGameExtension)