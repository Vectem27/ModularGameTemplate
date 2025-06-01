#include "Player/TP_MGFPlayerController.h"

// Engine includes
#include "CommonInputSubsystem.h"

// Project includes
#include "AbilitySystem/TP_MGFAbilitySystemComponent.h"
#include "AbilitySystem/TP_MGFAbilitySystemGlobals.h"

#include "Player/TP_MGFPlayerState.h"
#include "Player/TP_MGFLocalPlayer.h"

#include "Settings/TP_MGFSettingsShared.h"

#include "UI/TP_MGFHUD.h"



namespace TP_MGF
{
	namespace Input
	{
		static int32 ShouldAlwaysPlayForceFeedback = 0;
		static FAutoConsoleVariableRef CVarShouldAlwaysPlayForceFeedback(TEXT("TP_MGFPC.ShouldAlwaysPlayForceFeedback"),
			ShouldAlwaysPlayForceFeedback,
			TEXT("Should force feedback effects be played, even if the last input device was not a gamepad?"));
	}
}

ATP_MGFPlayerController::ATP_MGFPlayerController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void ATP_MGFPlayerController::BeginPlay()
{
	Super::BeginPlay();

	SetActorHiddenInGame(false);
}

ATP_MGFPlayerState* ATP_MGFPlayerController::GetTP_MGFPlayerState() const
{
	return CastChecked<ATP_MGFPlayerState>(PlayerState, ECastCheckedType::NullAllowed);
}

UTP_MGFAbilitySystemComponent* ATP_MGFPlayerController::GetTP_MGFAbilitySystemComponent() const
{
	const ATP_MGFPlayerState* TP_MGFPS = GetTP_MGFPlayerState();
	return (TP_MGFPS ? TP_MGFPS->GetTP_MGFAbilitySystemComponent() : nullptr);
}

ATP_MGFHUD* ATP_MGFPlayerController::GetTP_MGFHUD() const
{
	return CastChecked<ATP_MGFHUD>(GetHUD(), ECastCheckedType::NullAllowed);
}

void ATP_MGFPlayerController::SetPlayer(UPlayer* InPlayer)
{
	Super::SetPlayer(InPlayer);

	if (const UTP_MGFLocalPlayer* TP_MGFLocalPlayer = Cast<UTP_MGFLocalPlayer>(InPlayer))
	{
		UTP_MGFSettingsShared* UserSettings = TP_MGFLocalPlayer->GetSharedSettings();
		UserSettings->OnSettingChanged.AddUObject(this, &ThisClass::OnSettingsChanged);

		OnSettingsChanged(UserSettings);
	}
}

void ATP_MGFPlayerController::OnSettingsChanged(UTP_MGFSettingsShared* InSettings)
{
	bForceFeedbackEnabled = InSettings->GetForceFeedbackEnabled();
}

void ATP_MGFPlayerController::PreProcessInput(const float DeltaTime, const bool bGamePaused)
{
	Super::PreProcessInput(DeltaTime, bGamePaused);
}

void ATP_MGFPlayerController::PostProcessInput(const float DeltaTime, const bool bGamePaused)
{
	if (UTP_MGFAbilitySystemComponent* TP_MGFASC = GetTP_MGFAbilitySystemComponent())
	{
		// TODO: Uncomment     TP_MGFASC->ProcessAbilityInput(DeltaTime, bGamePaused);
	}

	Super::PostProcessInput(DeltaTime, bGamePaused);
}

void ATP_MGFPlayerController::UpdateForceFeedback(IInputInterface* InputInterface, const int32 ControllerId)
{
	if (bForceFeedbackEnabled)
	{
		if (const UCommonInputSubsystem* CommonInputSubsystem = UCommonInputSubsystem::Get(GetLocalPlayer()))
		{
			const ECommonInputType CurrentInputType = CommonInputSubsystem->GetCurrentInputType();
			if (TP_MGF::Input::ShouldAlwaysPlayForceFeedback || CurrentInputType == ECommonInputType::Gamepad || CurrentInputType == ECommonInputType::Touch)
			{
				InputInterface->SetForceFeedbackChannelValues(ControllerId, ForceFeedbackValues);
				return;
			}
		}
	}

	InputInterface->SetForceFeedbackChannelValues(ControllerId, FForceFeedbackValues());
}

void ATP_MGFPlayerController::OnUnPossess()
{
	// Make sure the pawn that is being unpossessed doesn't remain our ASC's avatar actor
	if (APawn* PawnBeingUnpossessed = GetPawn())
	{
		if (UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(PlayerState))
		{
			if (ASC->GetAvatarActor() == PawnBeingUnpossessed)
			{
				ASC->SetAvatarActor(nullptr);
			}
		}
	}

	Super::OnUnPossess();
}