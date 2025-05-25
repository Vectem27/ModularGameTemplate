#pragma once

#include "GameFramework/SaveGame.h"
#include "SubtitleDisplayOptions.h"

#include "UObject/ObjectPtr.h"
#include "TP_MGFSettingsShared.generated.h"

UENUM(BlueprintType)
enum class EColorBlindMode : uint8
{
	Off,
	// Deuteranope (green weak/blind)
	Deuteranope,
	// Protanope (red weak/blind)
	Protanope,
	// Tritanope(blue weak / bind)
	Tritanope
};

UENUM(BlueprintType)
enum class ETP_MGFAllowBackgroundAudioSetting : uint8
{
	Off,
	AllSounds,

	Num UMETA(Hidden),
};

class UTP_MGFLocalPlayer;


/**
 * UTP_MGF_SettingsShared - The "Shared" settings are stored as part of the USaveGame system, these settings are not machine
 * specific like the local settings, and are safe to store in the cloud - and 'share' them.  Using the save game system
 * we can also store settings per player, so things like controller keybind preferences should go here, because if those
 * are stored in the local settings all users would get them.
 *
 */
UCLASS()
class UTP_MGFSettingsShared : public ULocalPlayerSaveGame
{
	GENERATED_BODY()

public:
	DECLARE_EVENT_OneParam(UTP_MGFSettingsShared, FOnSettingChangedEvent, UTP_MGFSettingsShared* Settings);
	FOnSettingChangedEvent OnSettingChanged;

public:
	UTP_MGFSettingsShared();

	//~ULocalPlayerSaveGame interface
	int32 GetLatestDataVersion() const override;
	//~End of ULocalPlayerSaveGame interface

	/** Synchronously loads a settings object, this is not valid to call before login */
	static UTP_MGFSettingsShared* LoadOrCreateSettings(const UTP_MGFLocalPlayer* LocalPlayer);

	DECLARE_DELEGATE_OneParam(FOnSettingsLoadedEvent, UTP_MGFSettingsShared* Settings);
	/** Starts an async load of the settings object, calls Delegate on completion */
	static bool AsyncLoadOrCreateSettings(const UTP_MGFLocalPlayer* LocalPlayer, FOnSettingsLoadedEvent Delegate);

	/** Saves the settings to disk */
	void SaveSettings();

	/** Applies the current settings to the player */
	void ApplySettings();

	////////////////////////////////////////////////////////
	// Color Blind Options
public:
	UFUNCTION()
	EColorBlindMode GetColorBlindMode() const;
	UFUNCTION()
	void SetColorBlindMode(EColorBlindMode InMode);

	UFUNCTION()
	int32 GetColorBlindStrength() const;
	UFUNCTION()
	void SetColorBlindStrength(int32 InColorBlindStrength);

private:
	UPROPERTY()
	EColorBlindMode ColorBlindMode = EColorBlindMode::Off;

	UPROPERTY()
	int32 ColorBlindStrength = 10;

	////////////////////////////////////////////////////////
	// Subtitles
public:
	UFUNCTION()
	bool GetSubtitlesEnabled() const { return bEnableSubtitles; }
	UFUNCTION()
	void SetSubtitlesEnabled(bool Value) { ChangeValueAndDirty(bEnableSubtitles, Value); }

	UFUNCTION()
	ESubtitleDisplayTextSize GetSubtitlesTextSize() const { return SubtitleTextSize; }
	UFUNCTION()
	void SetSubtitlesTextSize(ESubtitleDisplayTextSize Value) { ChangeValueAndDirty(SubtitleTextSize, Value); ApplySubtitleOptions(); }

	UFUNCTION()
	ESubtitleDisplayTextColor GetSubtitlesTextColor() const { return SubtitleTextColor; }
	UFUNCTION()
	void SetSubtitlesTextColor(ESubtitleDisplayTextColor Value) { ChangeValueAndDirty(SubtitleTextColor, Value); ApplySubtitleOptions(); }

	UFUNCTION()
	ESubtitleDisplayTextBorder GetSubtitlesTextBorder() const { return SubtitleTextBorder; }
	UFUNCTION()
	void SetSubtitlesTextBorder(ESubtitleDisplayTextBorder Value) { ChangeValueAndDirty(SubtitleTextBorder, Value); ApplySubtitleOptions(); }

	UFUNCTION()
	ESubtitleDisplayBackgroundOpacity GetSubtitlesBackgroundOpacity() const { return SubtitleBackgroundOpacity; }
	UFUNCTION()
	void SetSubtitlesBackgroundOpacity(ESubtitleDisplayBackgroundOpacity Value) { ChangeValueAndDirty(SubtitleBackgroundOpacity, Value); ApplySubtitleOptions(); }

	void ApplySubtitleOptions();

private:
	UPROPERTY()
	bool bEnableSubtitles = true;

	UPROPERTY()
	ESubtitleDisplayTextSize SubtitleTextSize = ESubtitleDisplayTextSize::Medium;

	UPROPERTY()
	ESubtitleDisplayTextColor SubtitleTextColor = ESubtitleDisplayTextColor::White;

	UPROPERTY()
	ESubtitleDisplayTextBorder SubtitleTextBorder = ESubtitleDisplayTextBorder::None;

	UPROPERTY()
	ESubtitleDisplayBackgroundOpacity SubtitleBackgroundOpacity = ESubtitleDisplayBackgroundOpacity::Medium;

	////////////////////////////////////////////////////////
	// Shared audio settings
public:
	UFUNCTION()
	ETP_MGFAllowBackgroundAudioSetting GetAllowAudioInBackgroundSetting() const { return AllowAudioInBackground; }
	UFUNCTION()
	void SetAllowAudioInBackgroundSetting(ETP_MGFAllowBackgroundAudioSetting NewValue);

	void ApplyBackgroundAudioSettings();

private:
	UPROPERTY()
	ETP_MGFAllowBackgroundAudioSetting AllowAudioInBackground = ETP_MGFAllowBackgroundAudioSetting::Off;

	////////////////////////////////////////////////////////
	// Culture / language
public:
	/** Gets the pending culture */
	const FString& GetPendingCulture() const;

	/** Sets the pending culture to apply */
	void SetPendingCulture(const FString& NewCulture);

	// Called when the culture changes.
	void OnCultureChanged();

	/** Clears the pending culture to apply */
	void ClearPendingCulture();

	bool IsUsingDefaultCulture() const;

	void ResetToDefaultCulture();
	bool ShouldResetToDefaultCulture() const { return bResetToDefaultCulture; }

	void ApplyCultureSettings();
	void ResetCultureToCurrentSettings();

private:
	/** The pending culture to apply */
	UPROPERTY(Transient)
	FString PendingCulture;

	/* If true, resets the culture to default. */
	bool bResetToDefaultCulture = false;

	////////////////////////////////////////////////////////
	// Gamepad Deadzone
public:
	/** Getter for gamepad move stick dead zone value. */
	UFUNCTION()
	float GetGamepadMoveStickDeadZone() const { return GamepadMoveStickDeadZone; }

	/** Setter for gamepad move stick dead zone value. */
	UFUNCTION()
	void SetGamepadMoveStickDeadZone(const float NewValue) { ChangeValueAndDirty(GamepadMoveStickDeadZone, NewValue); }

	/** Getter for gamepad look stick dead zone value. */
	UFUNCTION()
	float GetGamepadLookStickDeadZone() const { return GamepadLookStickDeadZone; }

	/** Setter for gamepad look stick dead zone value. */
	UFUNCTION()
	void SetGamepadLookStickDeadZone(const float NewValue) { ChangeValueAndDirty(GamepadLookStickDeadZone, NewValue); }

private:
	/** Holds the gamepad move stick dead zone value. */
	UPROPERTY()
	float GamepadMoveStickDeadZone;

	/** Holds the gamepad look stick dead zone value. */
	UPROPERTY()
	float GamepadLookStickDeadZone;


	////////////////////////////////////////////////////////
	// Gamepad Trigger Haptics
public:
	UFUNCTION()
	bool GetTriggerHapticsEnabled() const { return bTriggerHapticsEnabled; }
	UFUNCTION()
	void SetTriggerHapticsEnabled(const bool NewValue) { ChangeValueAndDirty(bTriggerHapticsEnabled, NewValue); }

	UFUNCTION()
	bool GetTriggerPullUsesHapticThreshold() const { return bTriggerPullUsesHapticThreshold; }
	UFUNCTION()
	void SetTriggerPullUsesHapticThreshold(const bool NewValue) { ChangeValueAndDirty(bTriggerPullUsesHapticThreshold, NewValue); }

	UFUNCTION()
	uint8 GetTriggerHapticStrength() const { return TriggerHapticStrength; }
	UFUNCTION()
	void SetTriggerHapticStrength(const uint8 NewValue) { ChangeValueAndDirty(TriggerHapticStrength, NewValue); }

	UFUNCTION()
	uint8 GetTriggerHapticStartPosition() const { return TriggerHapticStartPosition; }
	UFUNCTION()
	void SetTriggerHapticStartPosition(const uint8 NewValue) { ChangeValueAndDirty(TriggerHapticStartPosition, NewValue); }

private:
	/** Are trigger haptics enabled? */
	UPROPERTY()
	bool bTriggerHapticsEnabled = false;
	/** Does the game use the haptic feedback as its threshold for judging button presses? */
	UPROPERTY()
	bool bTriggerPullUsesHapticThreshold = true;
	/** The strength of the trigger haptic effects. */
	UPROPERTY()
	uint8 TriggerHapticStrength = 8;
	/** The start position of the trigger haptic effects */
	UPROPERTY()
	uint8 TriggerHapticStartPosition = 0;


	////////////////////////////////////////////////////////
	// Gamepad Vibration
public:
	UFUNCTION()
	bool GetForceFeedbackEnabled() const { return bForceFeedbackEnabled; }

	UFUNCTION()
	void SetForceFeedbackEnabled(const bool NewValue) { ChangeValueAndDirty(bForceFeedbackEnabled, NewValue); }

private:
	/** Is force feedback enabled when a controller is being used? */
	UPROPERTY()
	bool bForceFeedbackEnabled = true;


	////////////////////////////////////////////////////////
	// Look Sensitivity
public:
	UFUNCTION()
	double GetMouseSensitivityX() const { return MouseSensitivityX; }
	UFUNCTION()
	void SetMouseSensitivityX(double NewValue) { ChangeValueAndDirty(MouseSensitivityX, NewValue); ApplyInputSensitivity(); }

	UFUNCTION()
	double GetMouseSensitivityY() const { return MouseSensitivityY; }
	UFUNCTION()
	void SetMouseSensitivityY(double NewValue) { ChangeValueAndDirty(MouseSensitivityY, NewValue); ApplyInputSensitivity(); }

	UFUNCTION()
	double GetGamepadLookSensitivityX() const { return GamepadLookSensitivityX; }
	UFUNCTION()
	void SetGamepadLookSensitivityX(double NewValue) { ChangeValueAndDirty(GamepadLookSensitivityX, NewValue); ApplyInputSensitivity(); }

	UFUNCTION()
	double GetGamepadLookSensitivityY() const { return GamepadLookSensitivityY; }
	UFUNCTION()
	void SetGamepadLookSensitivityY(double NewValue) { ChangeValueAndDirty(GamepadLookSensitivityY, NewValue); ApplyInputSensitivity(); }

	UFUNCTION()
	bool GetInvertVerticalAxis() const { return bInvertVerticalAxis; }
	UFUNCTION()
	void SetInvertVerticalAxis(bool NewValue) { ChangeValueAndDirty(bInvertVerticalAxis, NewValue); ApplyInputSensitivity(); }

	UFUNCTION()
	bool GetInvertHorizontalAxis() const { return bInvertHorizontalAxis; }
	UFUNCTION()
	void SetInvertHorizontalAxis(bool NewValue) { ChangeValueAndDirty(bInvertHorizontalAxis, NewValue); ApplyInputSensitivity(); }

	void ApplyInputSensitivity();
private:
	/** Holds the mouse horizontal sensitivity */
	UPROPERTY()
	double MouseSensitivityX = 1.0;

	/** Holds the mouse vertical sensitivity */
	UPROPERTY()
	double MouseSensitivityY = 1.0;

	/** Holds the gamepad horizontal look sensitivity */
	UPROPERTY()
	double GamepadLookSensitivityX = 1.0;

	/** Holds the gamepad vertical look sensitivity */
	UPROPERTY()
	double GamepadLookSensitivityY = 1.0;

	/** If true then the vertical look axis should be inverted */
	UPROPERTY()
	bool bInvertVerticalAxis = false;

	/** If true then the horizontal look axis should be inverted */
	UPROPERTY()
	bool bInvertHorizontalAxis = false;


	////////////////////////////////////////////////////////
	/// Dirty and Change Reporting
private:
	template<typename T>
	bool ChangeValueAndDirty(T& CurrentValue, const T& NewValue)
	{
		if (CurrentValue != NewValue)
		{
			CurrentValue = NewValue;
			bIsDirty = true;
			OnSettingChanged.Broadcast(this);

			return true;
		}

		return false;
	}

	bool bIsDirty = false;
};
