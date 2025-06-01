// Copyright 2025 Martin Bournat. Licensed under the Apache License 2.0.

#include "GameFeatureActions/GameFeatureAction_AddInputContextMapping.h"

#include "Engine/World.h"
#include "EnhancedInputSubsystems.h"
#include "UserSettings/EnhancedInputUserSettings.h"
#include "InputMappingContext.h"

#if WITH_EDITOR
#include "Misc/DataValidation.h"
#endif

#define LOCTEXT_NAMESPACE "GameFeatures"

//////////////////////////////////////////////////////////////////////
// Registering

void UGameFeatureAction_AddInputContextMapping::OnGameFeatureRegistering()
{
	Super::OnGameFeatureRegistering();

	RegisterInputContextMappingsForGameInstanceHandle = FWorldDelegates::OnStartGameInstance.AddUObject(this,
		&UGameFeatureAction_AddInputContextMapping::HandleGameInstanceStartRegister);

	for (const FWorldContext& WorldContext : GEngine->GetWorldContexts())
	{
		UWorld* World = WorldContext.World();
		UGameInstance* GameInstance = World->GetGameInstance();
		if (GameInstance)
		{
			GameInstance->OnLocalPlayerAddedEvent.AddUObject(this, &UGameFeatureAction_AddInputContextMapping::RegisterMappingToLocalPlayer);

			for (auto localPlayer : GameInstance->GetLocalPlayers())
			{
				RegisterMappingToLocalPlayer(localPlayer);
			}
		}
	}
}

void UGameFeatureAction_AddInputContextMapping::HandleGameInstanceStartRegister(UGameInstance* GameInstance)
{
	GameInstance->OnLocalPlayerAddedEvent.AddUObject(this, &UGameFeatureAction_AddInputContextMapping::RegisterMappingToLocalPlayer);

	for (auto localPlayer : GameInstance->GetLocalPlayers())
		RegisterMappingToLocalPlayer(localPlayer);
}

void UGameFeatureAction_AddInputContextMapping::RegisterMappingToLocalPlayer(ULocalPlayer* localPlayer)
{
	if (ensure(localPlayer))
	{
		if (UEnhancedInputLocalPlayerSubsystem* EISubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(localPlayer))
		{
			if (UEnhancedInputUserSettings* Settings = EISubsystem->GetUserSettings())
			{
				for (const FInputMappingContextAndPriority& Entry : InputMappings)
				{
					// Skip entries that don't want to be registered
					if (!Entry.bRegisterWithSettings)
					{
						continue;
					}

					// Register this IMC with the settings!
					if (UInputMappingContext* IMC = Entry.InputMapping.LoadSynchronous())
					{
						Settings->RegisterInputMappingContext(IMC);
					}
					else
					{
						UE_LOG(LogGameFeatures, Warning, TEXT("Attempted to register a null InputMappingContext for local player %s."), *localPlayer->GetName());
					}
				}
			}
			else
			{
				UE_LOG(LogGameFeatures, Error, TEXT("Failed to find `UEnhancedInputUserSettings` for local player. Input mappings will not be registered. Make sure you're set to use the EnhancedInput system via config file."));
			}
		}
		else
		{
			UE_LOG(LogGameFeatures, Error, TEXT("Failed to find `UEnhancedInputLocalPlayerSubsystem` for local player. Input mappings will not be added. Make sure you're set to use the EnhancedInput system via config file."));
		}
	}
}


/////////////////////////////////////////////////////////////////////////////////
// Unregistering

void UGameFeatureAction_AddInputContextMapping::OnGameFeatureUnregistering()
{
	Super::OnGameFeatureUnregistering();

	FWorldDelegates::OnStartGameInstance.Remove(RegisterInputContextMappingsForGameInstanceHandle);

	for (const FWorldContext& WorldContext : GEngine->GetWorldContexts())
	{
		UWorld* World = WorldContext.World();
		UGameInstance* GameInstance = World->GetGameInstance();
		if (GameInstance)
		{
			for (auto localPlayer : GameInstance->GetLocalPlayers())
				UnregisterMappingToLocalPlayer(localPlayer);
		}
	}
}

void UGameFeatureAction_AddInputContextMapping::UnregisterMappingToLocalPlayer(ULocalPlayer* localPlayer)
{
	if (ensure(localPlayer))
	{
		if (UEnhancedInputLocalPlayerSubsystem* EISubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(localPlayer))
		{
			if (UEnhancedInputUserSettings* Settings = EISubsystem->GetUserSettings())
			{
				for (const FInputMappingContextAndPriority& Entry : InputMappings)
				{
					// Skip entries that don't want to be registered
					if (!Entry.bRegisterWithSettings)
					{
						continue;
					}

					// Register this IMC with the settings!
					if (UInputMappingContext* IMC = Entry.InputMapping.Get())
					{
						Settings->UnregisterInputMappingContext(IMC);
					}
				}
			}
			else
			{
				UE_LOG(LogGameFeatures, Error, TEXT("Failed to find `UEnhancedInputUserSettings` for local player. Input mappings will not be removed."));
			}
		}
		else
		{
			UE_LOG(LogGameFeatures, Error, TEXT("Failed to find `UEnhancedInputLocalPlayerSubsystem` for local player. Input mappings will not be added. Make sure you're set to use the EnhancedInput system via config file."));
		}
	}
}


/////////////////////////////////////////////////////////////////////////////////
// Activating

void UGameFeatureAction_AddInputContextMapping::OnGameFeatureActivating(FGameFeatureActivatingContext& Context)
{
	Super::OnGameFeatureActivating(Context);

	ActivateInputContextMappingsForGameInstanceHandle = FWorldDelegates::OnStartGameInstance.AddUObject(this,
		&UGameFeatureAction_AddInputContextMapping::HandleGameInstanceStartActivate, FGameFeatureStateChangeContext(Context));

	for (const FWorldContext& WorldContext : GEngine->GetWorldContexts())
	{
		UWorld* World = WorldContext.World();
		if (Context.ShouldApplyToWorldContext(WorldContext) && World)
		{
			UGameInstance* GameInstance = World->GetGameInstance();
			if (GameInstance)
			{
				GameInstance->OnLocalPlayerAddedEvent.AddUObject(this, &UGameFeatureAction_AddInputContextMapping::AddMappingToLocalPlayer);

				for (auto localPlayer : GameInstance->GetLocalPlayers())
				{
					AddMappingToLocalPlayer(localPlayer);
				}
			}
		}
	}
}

void UGameFeatureAction_AddInputContextMapping::HandleGameInstanceStartActivate(UGameInstance* GameInstance, FGameFeatureStateChangeContext ChangeContext)
{
	GameInstance->OnLocalPlayerAddedEvent.AddUObject(this, &UGameFeatureAction_AddInputContextMapping::AddMappingToLocalPlayer);

	for (auto localPlayer : GameInstance->GetLocalPlayers())
		AddMappingToLocalPlayer(localPlayer);
}

void UGameFeatureAction_AddInputContextMapping::AddMappingToLocalPlayer(ULocalPlayer* localPlayer)
{
	if (!ensure(localPlayer)) return;

	// Copie en pointeur fort pour éviter capture de référence potentiellement détruite
	TWeakObjectPtr<ULocalPlayer> WeakLocalPlayer = localPlayer;

	if (UEnhancedInputLocalPlayerSubsystem* InputSystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(localPlayer))
	{
		TWeakObjectPtr<UEnhancedInputLocalPlayerSubsystem> WeakInputSystem = InputSystem;

		for (const FInputMappingContextAndPriority& Entry : InputMappings)
		{
			const UInputMappingContext* IMC = Entry.InputMapping.LoadSynchronous();
			if (!IMC) continue;

			// Copie du pointer vers l'InputMappingContext car TSoftObjectPtr pourrait être GC'd
			TSoftObjectPtr<UInputMappingContext> MappingCopy = Entry.InputMapping;
			const int32 Priority = Entry.Priority;

			localPlayer->GetWorld()->GetTimerManager().SetTimerForNextTick([WeakLocalPlayer, WeakInputSystem, MappingCopy, Priority]()
				{
					if (WeakLocalPlayer.IsValid() && WeakInputSystem.IsValid())
					{
						if (const UInputMappingContext* Mapping = MappingCopy.Get())
						{
							WeakInputSystem->AddMappingContext(Mapping, Priority);

							if (!WeakInputSystem->HasMappingContext(Mapping))
								UE_LOG(LogGameFeatures, Warning, TEXT("Failed to add InputMappingContext '%s' to local player %s."), *Mapping->GetName(), *WeakLocalPlayer->GetName());
						}
						else
						{
							UE_LOG(LogGameFeatures, Warning, TEXT("Attempted to add a null InputMappingContext to local player %s."), *WeakLocalPlayer->GetName());
						}
					}
					else
					{
						UE_LOG(LogGameFeatures, Warning, TEXT("Failed to add InputMappingContext to local player. LocalPlayer or InputSystem is invalid."));
					}
				});
		}
	}
	else
	{
		UE_LOG(LogGameFeatures, Error, TEXT("Failed to find `UEnhancedInputLocalPlayerSubsystem` for local player. Input mappings will not be added."));
	}
}


////////////////////////////////////////////////////////////////////////////////////
// Deactivating

void UGameFeatureAction_AddInputContextMapping::OnGameFeatureDeactivating(FGameFeatureDeactivatingContext& Context)
{
	Super::OnGameFeatureDeactivating(Context);

	FWorldDelegates::OnStartGameInstance.Remove(ActivateInputContextMappingsForGameInstanceHandle);

	for (const FWorldContext& WorldContext : GEngine->GetWorldContexts())
	{
		UWorld* World = WorldContext.World();
		if (Context.ShouldApplyToWorldContext(WorldContext) && World)
		{
			UGameInstance* GameInstance = World->GetGameInstance();
			if (GameInstance)
			{
				GameInstance->OnLocalPlayerAddedEvent.RemoveAll(this);

				for (auto localPlayer : GameInstance->GetLocalPlayers())
				{
					RemoveMappingToLocalPlayer(localPlayer);
				}
			}
		}
	}
}

void UGameFeatureAction_AddInputContextMapping::RemoveMappingToLocalPlayer(ULocalPlayer* localPlayer)
{
	// TODO: Check for mapping added cause, and remove only if its this cause (Can create a subsystem for that)


	if (ensure(localPlayer))
	{
		if (UEnhancedInputLocalPlayerSubsystem* InputSystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(localPlayer))
		{
			for (const FInputMappingContextAndPriority& Entry : InputMappings)
			{
				if (const UInputMappingContext* IMC = Entry.InputMapping.Get())
				{
					InputSystem->RemoveMappingContext(IMC);
				}
				else
				{
					UE_LOG(LogGameFeatures, Warning, TEXT("Attempted to remove a null InputMappingContext from local player %s."), *localPlayer->GetName());
				}
			}
		}
		else
		{
			UE_LOG(LogGameFeatures, Error, TEXT("Failed to find `UEnhancedInputLocalPlayerSubsystem` for local player. Input mappings will not be added. Make sure you're set to use the EnhancedInput system via config file."));
		}
	}

}



/////////////////////////////////////////////////////////////////////

#if WITH_EDITOR
EDataValidationResult UGameFeatureAction_AddInputContextMapping::IsDataValid(FDataValidationContext& Context) const
{
	EDataValidationResult Result = CombineDataValidationResults(Super::IsDataValid(Context), EDataValidationResult::Valid);

	int32 Index = 0;

	for (const FInputMappingContextAndPriority& Entry : InputMappings)
	{
		if (Entry.InputMapping.IsNull())
		{
			Result = EDataValidationResult::Invalid;
			Context.AddError(FText::Format(LOCTEXT("NullInputMapping", "Null InputMapping at index {0}."), Index));
		}
		++Index;
	}

	return Result;
}
#endif


#undef LOCTEXT_NAMESPACE
