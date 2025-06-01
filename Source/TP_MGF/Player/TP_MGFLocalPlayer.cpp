#include "Player/TP_MGFLocalPlayer.h"

// Settings
#include "Settings/TP_MGFSettingsShared.h"
#include "Settings/TP_MGFSettingsLocal.h"

#include "AudioMixerBlueprintLibrary.h"

UTP_MGFLocalPlayer::UTP_MGFLocalPlayer()
{
}

void UTP_MGFLocalPlayer::PostInitProperties()
{
	Super::PostInitProperties();

	if (UTP_MGFSettingsLocal* LocalSettings = GetLocalSettings())
	{
		LocalSettings->OnAudioOutputDeviceChanged.AddUObject(this, &UTP_MGFLocalPlayer::OnAudioOutputDeviceChanged);
	}
}

UTP_MGFSettingsLocal* UTP_MGFLocalPlayer::GetLocalSettings() const
{
	return UTP_MGFSettingsLocal::Get();
}

UTP_MGFSettingsShared* UTP_MGFLocalPlayer::GetSharedSettings() const
{
	if (!SharedSettings)
		SharedSettings = UTP_MGFSettingsShared::LoadOrCreateSettings(this);

	return SharedSettings;
}

void UTP_MGFLocalPlayer::LoadSharedSettingsFromDisk(bool bForceLoad)
{
	if (!bForceLoad && SharedSettings)
		return; // Already loaded once, don't reload

	ensure(UTP_MGFSettingsShared::AsyncLoadOrCreateSettings(this, UTP_MGFSettingsShared::FOnSettingsLoadedEvent::CreateUObject(this, &UTP_MGFLocalPlayer::OnSharedSettingsLoaded)));
}

void UTP_MGFLocalPlayer::OnSharedSettingsLoaded(UTP_MGFSettingsShared* LoadedOrCreatedSettings)
{
	// The settings are applied before it gets here
	if (ensure(LoadedOrCreatedSettings))
	{
		// This will replace the temporary or previously loaded object which will GC out normally
		SharedSettings = LoadedOrCreatedSettings;
	}
}

void UTP_MGFLocalPlayer::OnAudioOutputDeviceChanged(const FString& InAudioOutputDeviceId)
{
	FOnCompletedDeviceSwap DevicesSwappedCallback;
	DevicesSwappedCallback.BindUFunction(this, FName("OnCompletedAudioDeviceSwap"));
	UAudioMixerBlueprintLibrary::SwapAudioOutputDevice(GetWorld(), InAudioOutputDeviceId, DevicesSwappedCallback);
}

void UTP_MGFLocalPlayer::OnCompletedAudioDeviceSwap(const FSwapAudioOutputResult& SwapResult)
{
	if (SwapResult.Result == ESwapAudioOutputDeviceResultState::Failure)
	{
	}
}
