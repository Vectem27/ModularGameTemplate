// Copyright Epic Games, Inc. All Rights Reserved.

#include "TP_MGFInputConfig.h"

#include "TP_MGFLogChannels.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(TP_MGFInputConfig)

UTP_MGFInputConfig::UTP_MGFInputConfig(const FObjectInitializer& ObjectInitializer)
{
}

const UInputAction* UTP_MGFInputConfig::FindNativeInputActionForTag(const FGameplayTag& InputTag, bool bLogNotFound) const
{
	for (const FTP_MGFInputAction& Action : NativeInputActions)
	{
		if (Action.InputAction && (Action.InputTag == InputTag))
		{
			return Action.InputAction;
		}
	}

	if (bLogNotFound)
	{
		UE_LOG(LogTP_MGFInput, Error, TEXT("Can't find NativeInputAction for InputTag [%s] on InputConfig [%s]."), *InputTag.ToString(), *GetNameSafe(this));
	}

	return nullptr;
}

const UInputAction* UTP_MGFInputConfig::FindAbilityInputActionForTag(const FGameplayTag& InputTag, bool bLogNotFound) const
{
	for (const FTP_MGFInputAction& Action : AbilityInputActions)
	{
		if (Action.InputAction && (Action.InputTag == InputTag))
		{
			return Action.InputAction;
		}
	}

	if (bLogNotFound)
	{
		UE_LOG(LogTP_MGF, Error, TEXT("Can't find AbilityInputAction for InputTag [%s] on InputConfig [%s]."), *InputTag.ToString(), *GetNameSafe(this));
	}

	return nullptr;
}
