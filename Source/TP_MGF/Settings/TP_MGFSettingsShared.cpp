#include "Settings/TP_MGFSettingsShared.h"

#include "Kismet/GameplayStatics.h"

// Gameplay
#include "Player/TP_MGFLocalPlayer.h"

// Cultures
#include "Internationalization/Culture.h"

// Input 
#include "EnhancedInputSubsystems.h"
#include "UserSettings/EnhancedInputUserSettings.h"

// UI
#include "SubtitleDisplaySubsystem.h"
#include "Framework/Application/SlateApplication.h"


#include UE_INLINE_GENERATED_CPP_BY_NAME(TP_MGFSettingsShared)

static FString SHARED_SETTINGS_SLOT_NAME = TEXT("SharedGameSettings");

namespace TP_MGFSettingsSharedCVars
{
	static float DefaultGamepadLeftStickInnerDeadZone = 0.25f;
	static FAutoConsoleVariableRef CVarGamepadLeftStickInnerDeadZone(
		TEXT("gpad.DefaultLeftStickInnerDeadZone"),
		DefaultGamepadLeftStickInnerDeadZone,
		TEXT("Gamepad left stick inner deadzone")
	);

	static float DefaultGamepadRightStickInnerDeadZone = 0.25f;
	static FAutoConsoleVariableRef CVarGamepadRightStickInnerDeadZone(
		TEXT("gpad.DefaultRightStickInnerDeadZone"),
		DefaultGamepadRightStickInnerDeadZone,
		TEXT("Gamepad right stick inner deadzone")
	);
}

UTP_MGFSettingsShared::UTP_MGFSettingsShared()
{
	FInternationalization::Get().OnCultureChanged().AddUObject(this, &ThisClass::OnCultureChanged);

	GamepadMoveStickDeadZone = TP_MGFSettingsSharedCVars::DefaultGamepadLeftStickInnerDeadZone;
	GamepadLookStickDeadZone = TP_MGFSettingsSharedCVars::DefaultGamepadRightStickInnerDeadZone;
}

int32 UTP_MGFSettingsShared::GetLatestDataVersion() const
{
	// 0 = before subclassing ULocalPlayerSaveGame
	// 1 = first proper version
	return 1;
}

UTP_MGFSettingsShared* UTP_MGFSettingsShared::LoadOrCreateSettings(const UTP_MGFLocalPlayer* LocalPlayer)
{
	// This will stall the main thread while it loads
	UTP_MGFSettingsShared* SharedSettings = Cast<UTP_MGFSettingsShared>(LoadOrCreateSaveGameForLocalPlayer(UTP_MGFSettingsShared::StaticClass(), LocalPlayer, SHARED_SETTINGS_SLOT_NAME));

	SharedSettings->ApplySettings();

	return SharedSettings;
}

bool UTP_MGFSettingsShared::AsyncLoadOrCreateSettings(const UTP_MGFLocalPlayer* LocalPlayer, FOnSettingsLoadedEvent Delegate)
{
	FOnLocalPlayerSaveGameLoadedNative Lambda = FOnLocalPlayerSaveGameLoadedNative::CreateLambda([Delegate]
	(ULocalPlayerSaveGame* LoadedSave)
		{
			UTP_MGFSettingsShared* LoadedSettings = CastChecked<UTP_MGFSettingsShared>(LoadedSave);

			LoadedSettings->ApplySettings();

			Delegate.ExecuteIfBound(LoadedSettings);
		});

	return ULocalPlayerSaveGame::AsyncLoadOrCreateSaveGameForLocalPlayer(UTP_MGFSettingsShared::StaticClass(), LocalPlayer, SHARED_SETTINGS_SLOT_NAME, Lambda);
}

void UTP_MGFSettingsShared::SaveSettings()
{
	// Schedule an async save because it's okay if it fails
	AsyncSaveGameToSlotForLocalPlayer();

	// TODO_BH: Move this to the serialize function instead with a bumped version number
	if (UEnhancedInputLocalPlayerSubsystem* System = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(OwningPlayer))
	{
		if (UEnhancedInputUserSettings* InputSettings = System->GetUserSettings())
		{
			InputSettings->AsyncSaveSettings();
		}
	}
}

void UTP_MGFSettingsShared::ApplySettings()
{
	ApplySubtitleOptions();
	ApplyBackgroundAudioSettings();
	ApplyCultureSettings();

	if (UEnhancedInputLocalPlayerSubsystem* System = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(OwningPlayer))
	{
		if (UEnhancedInputUserSettings* InputSettings = System->GetUserSettings())
		{
			InputSettings->ApplySettings();
		}
	}
}

//////////////////////////////////////////////////////////////////////

EColorBlindMode UTP_MGFSettingsShared::GetColorBlindMode() const
{
	return ColorBlindMode;

}

void UTP_MGFSettingsShared::SetColorBlindMode(EColorBlindMode InMode)
{
	if (ColorBlindMode != InMode)
	{
		ColorBlindMode = InMode;
		FSlateApplication::Get().GetRenderer()->SetColorVisionDeficiencyType(
			(EColorVisionDeficiency)(int32)ColorBlindMode, (int32)ColorBlindStrength, true, false);
	}
}

int32 UTP_MGFSettingsShared::GetColorBlindStrength() const
{
	return ColorBlindStrength;
}

void UTP_MGFSettingsShared::SetColorBlindStrength(int32 InColorBlindStrength)
{
	InColorBlindStrength = FMath::Clamp(InColorBlindStrength, 0, 10);
	if (ColorBlindStrength != InColorBlindStrength)
	{
		ColorBlindStrength = InColorBlindStrength;
		FSlateApplication::Get().GetRenderer()->SetColorVisionDeficiencyType(
			(EColorVisionDeficiency)(int32)ColorBlindMode, (int32)ColorBlindStrength, true, false);
	}
}


//////////////////////////////////////////////////////////////////////

void UTP_MGFSettingsShared::ApplySubtitleOptions()
{
	if (USubtitleDisplaySubsystem* SubtitleSystem = USubtitleDisplaySubsystem::Get(OwningPlayer))
	{
		FSubtitleFormat SubtitleFormat;
		SubtitleFormat.SubtitleTextSize = SubtitleTextSize;
		SubtitleFormat.SubtitleTextColor = SubtitleTextColor;
		SubtitleFormat.SubtitleTextBorder = SubtitleTextBorder;
		SubtitleFormat.SubtitleBackgroundOpacity = SubtitleBackgroundOpacity;

		SubtitleSystem->SetSubtitleDisplayOptions(SubtitleFormat);
	}
}

//////////////////////////////////////////////////////////////////////

void UTP_MGFSettingsShared::SetAllowAudioInBackgroundSetting(ETP_MGFAllowBackgroundAudioSetting NewValue)
{
	if (ChangeValueAndDirty(AllowAudioInBackground, NewValue))
	{
		ApplyBackgroundAudioSettings();
	}
}

void UTP_MGFSettingsShared::ApplyBackgroundAudioSettings()
{
	if (OwningPlayer && OwningPlayer->IsPrimaryPlayer())
	{
		FApp::SetUnfocusedVolumeMultiplier((AllowAudioInBackground != ETP_MGFAllowBackgroundAudioSetting::Off) ? 1.0f : 0.0f);
	}
}

//////////////////////////////////////////////////////////////////////

void UTP_MGFSettingsShared::ApplyCultureSettings()
{
	if (bResetToDefaultCulture)
	{
		const FCulturePtr SystemDefaultCulture = FInternationalization::Get().GetDefaultCulture();
		check(SystemDefaultCulture.IsValid());

		const FString CultureToApply = SystemDefaultCulture->GetName();
		if (FInternationalization::Get().SetCurrentCulture(CultureToApply))
		{
			// Clear this string
			GConfig->RemoveKey(TEXT("Internationalization"), TEXT("Culture"), GGameUserSettingsIni);
			GConfig->Flush(false, GGameUserSettingsIni);
		}
		bResetToDefaultCulture = false;
	}
	else if (!PendingCulture.IsEmpty())
	{
		// SetCurrentCulture may trigger PendingCulture to be cleared (if a culture change is broadcast) so we take a copy of it to work with
		const FString CultureToApply = PendingCulture;
		if (FInternationalization::Get().SetCurrentCulture(CultureToApply))
		{
			// Note: This is intentionally saved to the users config
			// We need to localize text before the player logs in and very early in the loading screen
			GConfig->SetString(TEXT("Internationalization"), TEXT("Culture"), *CultureToApply, GGameUserSettingsIni);
			GConfig->Flush(false, GGameUserSettingsIni);
		}
		ClearPendingCulture();
	}
}

void UTP_MGFSettingsShared::ResetCultureToCurrentSettings()
{
	ClearPendingCulture();
	bResetToDefaultCulture = false;
}

const FString& UTP_MGFSettingsShared::GetPendingCulture() const
{
	return PendingCulture;
}

void UTP_MGFSettingsShared::SetPendingCulture(const FString& NewCulture)
{
	PendingCulture = NewCulture;
	bResetToDefaultCulture = false;
	bIsDirty = true;
}

void UTP_MGFSettingsShared::OnCultureChanged()
{
	ClearPendingCulture();
	bResetToDefaultCulture = false;
}

void UTP_MGFSettingsShared::ClearPendingCulture()
{
	PendingCulture.Reset();
}

bool UTP_MGFSettingsShared::IsUsingDefaultCulture() const
{
	FString Culture;
	GConfig->GetString(TEXT("Internationalization"), TEXT("Culture"), Culture, GGameUserSettingsIni);

	return Culture.IsEmpty();
}

void UTP_MGFSettingsShared::ResetToDefaultCulture()
{
	ClearPendingCulture();
	bResetToDefaultCulture = true;
	bIsDirty = true;
}

//////////////////////////////////////////////////////////////////////

void UTP_MGFSettingsShared::ApplyInputSensitivity()
{

}