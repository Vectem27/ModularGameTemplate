// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "UserSettings/EnhancedInputUserSettings.h"

#include "TP_MGFPlayerMappableKeyProfile.generated.h"

UCLASS()
class TP_MGF_API UTP_MGFPlayerMappableKeyProfile : public UEnhancedPlayerMappableKeyProfile
{
	GENERATED_BODY()

protected:

	//~ Begin UEnhancedPlayerMappableKeyProfile interface
	virtual void EquipProfile() override;
	virtual void UnEquipProfile() override;
	//~ End UEnhancedPlayerMappableKeyProfile interface
};