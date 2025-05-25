// Copyright Epic Games, Inc. All Rights Reserved.

#include "TP_MGFInputModifiers.h"

#include "EnhancedPlayerInput.h"
#include "GameFramework/PlayerController.h"
#include "Player/TP_MGFLocalPlayer.h"
#include "Settings/TP_MGFSettingsShared.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(TP_MGFInputModifiers)

DEFINE_LOG_CATEGORY_STATIC(LogTP_MGFInputModifiers, Log, All);

//////////////////////////////////////////////////////////////////////
// TP_MGFInputModifiersHelpers

namespace TP_MGFInputModifiersHelpers
{
	/** Returns the owning TP_MGFLocalPlayer of an Enhanced Player Input pointer */
	static UTP_MGFLocalPlayer* GetLocalPlayer(const UEnhancedPlayerInput* PlayerInput)
	{
		if (PlayerInput)
		{
			if (APlayerController* PC = Cast<APlayerController>(PlayerInput->GetOuter()))
			{
				return Cast<UTP_MGFLocalPlayer>(PC->GetLocalPlayer());
			}
		}
		return nullptr;
	}
	
}

//////////////////////////////////////////////////////////////////////
// UTP_MGFSettingBasedScalar

FInputActionValue UTP_MGFSettingBasedScalar::ModifyRaw_Implementation(const UEnhancedPlayerInput* PlayerInput, FInputActionValue CurrentValue, float DeltaTime)
{
	if (ensureMsgf(CurrentValue.GetValueType() != EInputActionValueType::Boolean, TEXT("Setting Based Scalar modifier doesn't support boolean values.")))
	{
		if (UTP_MGFLocalPlayer* LocalPlayer = TP_MGFInputModifiersHelpers::GetLocalPlayer(PlayerInput))
		{
			const UClass* SettingsClass = UTP_MGFSettingsShared::StaticClass();
			UTP_MGFSettingsShared* SharedSettings = LocalPlayer->GetSharedSettings();
			
			const bool bHasCachedProperty = PropertyCache.Num() == 3;
			
			const FProperty* XAxisValue = bHasCachedProperty ? PropertyCache[0] : SettingsClass->FindPropertyByName(XAxisScalarSettingName);
			const FProperty* YAxisValue = bHasCachedProperty ? PropertyCache[1] : SettingsClass->FindPropertyByName(YAxisScalarSettingName);
			const FProperty* ZAxisValue = bHasCachedProperty ? PropertyCache[2] : SettingsClass->FindPropertyByName(ZAxisScalarSettingName);

			if (PropertyCache.IsEmpty())
			{
				PropertyCache.Emplace(XAxisValue);
				PropertyCache.Emplace(YAxisValue);
				PropertyCache.Emplace(ZAxisValue);
			}

			FVector ScalarToUse = FVector(1.0, 1.0, 1.0);

			switch (CurrentValue.GetValueType())
			{
			case EInputActionValueType::Axis3D:
				ScalarToUse.Z = ZAxisValue ? *ZAxisValue->ContainerPtrToValuePtr<double>(SharedSettings) : 1.0;
				//[[fallthrough]];
			case EInputActionValueType::Axis2D:
				ScalarToUse.Y = YAxisValue ? *YAxisValue->ContainerPtrToValuePtr<double>(SharedSettings) : 1.0;
				//[[fallthrough]];
			case EInputActionValueType::Axis1D:
				ScalarToUse.X = XAxisValue ? *XAxisValue->ContainerPtrToValuePtr<double>(SharedSettings) : 1.0;
				break;
			}

			ScalarToUse.X = FMath::Clamp(ScalarToUse.X, MinValueClamp.X, MaxValueClamp.X);
			ScalarToUse.Y = FMath::Clamp(ScalarToUse.Y, MinValueClamp.Y, MaxValueClamp.Y);
			ScalarToUse.Z = FMath::Clamp(ScalarToUse.Z, MinValueClamp.Z, MaxValueClamp.Z);
			
			return CurrentValue.Get<FVector>() * ScalarToUse;
		}
	}
	
	return CurrentValue;	
}

//////////////////////////////////////////////////////////////////////
// UTP_MGFInputModifierDeadZone

FInputActionValue UTP_MGFInputModifierDeadZone::ModifyRaw_Implementation(const UEnhancedPlayerInput* PlayerInput, FInputActionValue CurrentValue, float DeltaTime)
{
	EInputActionValueType ValueType = CurrentValue.GetValueType();
	UTP_MGFLocalPlayer* LocalPlayer = TP_MGFInputModifiersHelpers::GetLocalPlayer(PlayerInput);
	if (ValueType == EInputActionValueType::Boolean || !LocalPlayer)
	{
		return CurrentValue;
	}
	
	UTP_MGFSettingsShared* Settings = LocalPlayer->GetSharedSettings();
	ensure(Settings);

	float LowerThreshold =
		(DeadzoneStick == EDeadzoneStick::MoveStick) ? 
		Settings->GetGamepadMoveStickDeadZone() :
		Settings->GetGamepadLookStickDeadZone();
	
	LowerThreshold = FMath::Clamp(LowerThreshold, 0.0f, 1.0f);
	
	auto DeadZoneLambda = [LowerThreshold, this](const float AxisVal)
	{
		// We need to translate and scale the input to the +/- 1 range after removing the dead zone.
		return FMath::Min(1.f, (FMath::Max(0.f, FMath::Abs(AxisVal) - LowerThreshold) / (UpperThreshold - LowerThreshold))) * FMath::Sign(AxisVal);
	};

	FVector NewValue = CurrentValue.Get<FVector>();
	switch (Type)
	{
	case EDeadZoneType::Axial:
		NewValue.X = DeadZoneLambda(NewValue.X);
		NewValue.Y = DeadZoneLambda(NewValue.Y);
		NewValue.Z = DeadZoneLambda(NewValue.Z);
		break;
	case EDeadZoneType::Radial:
		if (ValueType == EInputActionValueType::Axis3D)
		{
			NewValue = NewValue.GetSafeNormal() * DeadZoneLambda(NewValue.Size());
		}
		else if (ValueType == EInputActionValueType::Axis2D)
		{
			NewValue = NewValue.GetSafeNormal2D() * DeadZoneLambda(NewValue.Size2D());
		}
		else
		{
			NewValue.X = DeadZoneLambda(NewValue.X);
		}
		break;
	}

	return NewValue;
}

FLinearColor UTP_MGFInputModifierDeadZone::GetVisualizationColor_Implementation(FInputActionValue SampleValue, FInputActionValue FinalValue) const
{
	// Taken from UInputModifierDeadZone::GetVisualizationColor_Implementation
	if (FinalValue.GetValueType() == EInputActionValueType::Boolean || FinalValue.GetValueType() == EInputActionValueType::Axis1D)
	{
		return FLinearColor(FinalValue.Get<float>() == 0.f ? 1.f : 0.f, 0.f, 0.f);
	}
	return FLinearColor((FinalValue.Get<FVector2D>().X == 0.f ? 0.5f : 0.f) + (FinalValue.Get<FVector2D>().Y == 0.f ? 0.5f : 0.f), 0.f, 0.f);
}

//////////////////////////////////////////////////////////////////////
// UTP_MGFInputModifierMouseSensitivity

FInputActionValue UTP_MGFInputModifierMouseSensitivity::ModifyRaw_Implementation(const UEnhancedPlayerInput* PlayerInput, FInputActionValue CurrentValue, float DeltaTime)
{
	// You can't scale a boolean action type
	UTP_MGFLocalPlayer* LocalPlayer = TP_MGFInputModifiersHelpers::GetLocalPlayer(PlayerInput);
	if (CurrentValue.GetValueType() == EInputActionValueType::Boolean || !LocalPlayer)
		return CurrentValue;

	UTP_MGFSettingsShared* Settings = LocalPlayer->GetSharedSettings();
	ensure(Settings);

	FVector NewValue = CurrentValue.Get<FVector>();

	NewValue.Y *= Settings->GetMouseSensitivityY();
	NewValue.X *= Settings->GetMouseSensitivityX();

	return NewValue;
}

//////////////////////////////////////////////////////////////////////
// UTP_MGFInputModifierGamepadSensitivity

FInputActionValue UTP_MGFInputModifierGamepadSensitivity::ModifyRaw_Implementation(const UEnhancedPlayerInput* PlayerInput, FInputActionValue CurrentValue, float DeltaTime)
{
	// You can't scale a boolean action type
	UTP_MGFLocalPlayer* LocalPlayer = TP_MGFInputModifiersHelpers::GetLocalPlayer(PlayerInput);
	if (CurrentValue.GetValueType() == EInputActionValueType::Boolean || !LocalPlayer)
		return CurrentValue;
	
	UTP_MGFSettingsShared* Settings = LocalPlayer->GetSharedSettings();
	ensure(Settings);

	FVector NewValue = CurrentValue.Get<FVector>();

	NewValue.Y *= Settings->GetGamepadLookSensitivityY();
	NewValue.X *= Settings->GetGamepadLookSensitivityX();

	return NewValue;
}

//////////////////////////////////////////////////////////////////////
// UTP_MGFInputModifierAimInversion

FInputActionValue UTP_MGFInputModifierAimInversion::ModifyRaw_Implementation(const UEnhancedPlayerInput* PlayerInput, FInputActionValue CurrentValue, float DeltaTime)
{
	UTP_MGFLocalPlayer* LocalPlayer = TP_MGFInputModifiersHelpers::GetLocalPlayer(PlayerInput);
	if (!LocalPlayer || CurrentValue.GetValueType() == EInputActionValueType::Boolean)
		return CurrentValue;
	
	UTP_MGFSettingsShared* Settings = LocalPlayer->GetSharedSettings();
	ensure(Settings);

	FVector NewValue = CurrentValue.Get<FVector>();
	
	if (Settings->GetInvertVerticalAxis())
		NewValue.Y *= -1.0f;
	
	if (Settings->GetInvertHorizontalAxis())
		NewValue.X *= -1.0f;

	return NewValue;
}
