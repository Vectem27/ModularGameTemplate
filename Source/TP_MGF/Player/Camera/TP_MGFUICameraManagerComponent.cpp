#include "Player/Camera/TP_MGFUICameraManagerComponent.h"

#include "GameFramework/HUD.h"
#include "GameFramework/PlayerController.h"
#include "TP_MGFPlayerCameraManager.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(TP_MGFUICameraManagerComponent)

class AActor;
class FDebugDisplayInfo;

UTP_MGFUICameraManagerComponent* UTP_MGFUICameraManagerComponent::GetComponent(APlayerController* PC)
{
	if (PC != nullptr)
	{
		if (ATP_MGFPlayerCameraManager* PCCamera = Cast<ATP_MGFPlayerCameraManager>(PC->PlayerCameraManager))
		{
			return PCCamera->GetUICameraComponent();
		}
	}

	return nullptr;
}

UTP_MGFUICameraManagerComponent::UTP_MGFUICameraManagerComponent()
{
	bWantsInitializeComponent = true;

	if (!HasAnyFlags(RF_ClassDefaultObject))
	{
		// Register "showdebug" hook.
		if (!IsRunningDedicatedServer())
		{
			AHUD::OnShowDebugInfo.AddUObject(this, &ThisClass::OnShowDebugInfo);
		}
	}
}

void UTP_MGFUICameraManagerComponent::InitializeComponent()
{
	Super::InitializeComponent();
}

void UTP_MGFUICameraManagerComponent::SetViewTarget(AActor* InViewTarget, FViewTargetTransitionParams TransitionParams)
{
	TGuardValue<bool> UpdatingViewTargetGuard(bUpdatingViewTarget, true);

	ViewTarget = InViewTarget;
	CastChecked<ATP_MGFPlayerCameraManager>(GetOwner())->SetViewTarget(ViewTarget, TransitionParams);
}

bool UTP_MGFUICameraManagerComponent::NeedsToUpdateViewTarget() const
{
	return false;
}

void UTP_MGFUICameraManagerComponent::UpdateViewTarget(struct FTViewTarget& OutVT, float DeltaTime)
{
}

void UTP_MGFUICameraManagerComponent::OnShowDebugInfo(AHUD* HUD, UCanvas* Canvas, const FDebugDisplayInfo& DisplayInfo, float& YL, float& YPos)
{
}
