// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Engine/DeveloperSettings.h"
#include "UObject/SoftObjectPtr.h"

#include "TP_MGFAudioSettings.generated.h"

class UObject;
class USoundEffectSubmixPreset;
class USoundSubmix;

USTRUCT()
struct TP_MGF_API FTP_MGFSubmixEffectChainMap
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, meta = (AllowedClasses = "/Script/Engine.SoundSubmix"))
	TSoftObjectPtr<USoundSubmix> Submix = nullptr;

	UPROPERTY(EditAnywhere, meta = (AllowedClasses = "/Script/Engine.SoundEffectSubmixPreset"))
	TArray<TSoftObjectPtr<USoundEffectSubmixPreset>> SubmixEffectChain;

};

/**
 * 
 */
UCLASS(config = Game, defaultconfig, meta = (DisplayName = "TP_MGFAudioSettings"))
class TP_MGF_API UTP_MGFAudioSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:

	/** The Default Base Control Bus Mix */
	UPROPERTY(config, EditAnywhere, Category = MixSettings, meta = (AllowedClasses = "/Script/AudioModulation.SoundControlBusMix"))
	FSoftObjectPath DefaultControlBusMix;

	/** The Loading Screen Control Bus Mix - Called during loading screens to cover background audio events */
	UPROPERTY(config, EditAnywhere, Category = MixSettings, meta = (AllowedClasses = "/Script/AudioModulation.SoundControlBusMix"))
	FSoftObjectPath LoadingScreenControlBusMix;

	/** The Default Base Control Bus Mix */
	UPROPERTY(config, EditAnywhere, Category = UserMixSettings, meta = (AllowedClasses = "/Script/AudioModulation.SoundControlBusMix"))
	FSoftObjectPath UserSettingsControlBusMix;

	/** Control Bus assigned to the Overall sound volume setting */
	UPROPERTY(config, EditAnywhere, Category = UserMixSettings, meta = (AllowedClasses = "/Script/AudioModulation.SoundControlBus"))
	FSoftObjectPath OverallVolumeControlBus;

	/** Control Bus assigned to the Music sound volume setting */
	UPROPERTY(config, EditAnywhere, Category = UserMixSettings, meta = (AllowedClasses = "/Script/AudioModulation.SoundControlBus"))
	FSoftObjectPath MusicVolumeControlBus;

	/** Control Bus assigned to the SoundFX sound volume setting */
	UPROPERTY(config, EditAnywhere, Category = UserMixSettings, meta = (AllowedClasses = "/Script/AudioModulation.SoundControlBus"))
	FSoftObjectPath SoundFXVolumeControlBus;

	/** Control Bus assigned to the Dialogue sound volume setting */
	UPROPERTY(config, EditAnywhere, Category = UserMixSettings, meta = (AllowedClasses = "/Script/AudioModulation.SoundControlBus"))
	FSoftObjectPath DialogueVolumeControlBus;

	/** Submix Processing Chains to achieve high dynamic range audio output */
	UPROPERTY(config, EditAnywhere, Category = EffectSettings)
	TArray<FTP_MGFSubmixEffectChainMap> HDRAudioSubmixEffectChain;
	
	/** Submix Processing Chains to achieve low dynamic range audio output */
	UPROPERTY(config, EditAnywhere, Category = EffectSettings)
	TArray<FTP_MGFSubmixEffectChainMap> LDRAudioSubmixEffectChain;

private:


};
