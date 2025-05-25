// Copyright Epic Games, Inc. All Rights Reserved.

#include "TP_MGFInputComponent.h"

#include "EnhancedInputSubsystems.h"
#include "Player/TP_MGFLocalPlayer.h"
#include "Settings/TP_MGFSettingsLocal.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(TP_MGFInputComponent)

class UTP_MGFInputConfig;

UTP_MGFInputComponent::UTP_MGFInputComponent(const FObjectInitializer& ObjectInitializer)
{
}

void UTP_MGFInputComponent::AddInputMappings(const UTP_MGFInputConfig* InputConfig, UEnhancedInputLocalPlayerSubsystem* InputSubsystem) const
{
	check(InputConfig);
	check(InputSubsystem);

	// Here you can handle any custom logic to add something from your input config if required
}

void UTP_MGFInputComponent::RemoveInputMappings(const UTP_MGFInputConfig* InputConfig, UEnhancedInputLocalPlayerSubsystem* InputSubsystem) const
{
	check(InputConfig);
	check(InputSubsystem);

	// Here you can handle any custom logic to remove input mappings that you may have added above
}

void UTP_MGFInputComponent::RemoveBinds(TArray<uint32>& BindHandles)
{
	for (uint32 Handle : BindHandles)
	{
		RemoveBindingByHandle(Handle);
	}
	BindHandles.Reset();
}
