#pragma once

#include "CommonLocalPlayer.h"

#include "TP_MGFLocalPlayer.generated.h"

class UTP_MGFSettingsLocal;
class UTP_MGFSettingsShared;
class UInputMappingContext;
struct FSwapAudioOutputResult;

UCLASS(config = Engine, transient)
class UTP_MGFLocalPlayer : public UCommonLocalPlayer
{
	GENERATED_BODY()

public:
	UTP_MGFLocalPlayer();

	//~UObject interface
	virtual void PostInitProperties() override;
	//~End of UObject interface

	/** Gets the local settings for this player, this is read from config files at process startup and is always valid */
	UFUNCTION()
	UTP_MGFSettingsLocal* GetLocalSettings() const;

	/** Gets the shared setting for this player, this is read using the save game system so may not be correct until after user login */
	UFUNCTION()
	UTP_MGFSettingsShared* GetSharedSettings() const;

	/** Starts an async request to load the shared settings, this will call OnSharedSettingsLoaded after loading or creating new ones */
	void LoadSharedSettingsFromDisk(bool bForceLoad = false);

protected:
	void OnSharedSettingsLoaded(UTP_MGFSettingsShared* LoadedOrCreatedSettings);

	void OnAudioOutputDeviceChanged(const FString& InAudioOutputDeviceId);

	UFUNCTION()
	void OnCompletedAudioDeviceSwap(const FSwapAudioOutputResult& SwapResult);

private:
	UPROPERTY(Transient)
	mutable TObjectPtr<UTP_MGFSettingsShared> SharedSettings;

	UPROPERTY(Transient)
	mutable TObjectPtr<const UInputMappingContext> InputMappingContext;
};
