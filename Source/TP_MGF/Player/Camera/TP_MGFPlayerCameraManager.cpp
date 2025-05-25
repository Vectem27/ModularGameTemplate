#include "Player/Camera/TP_MGFPlayerCameraManager.h"

#include "Async/TaskGraphInterfaces.h"
#include "Engine/Canvas.h"
#include "Engine/Engine.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"

#include "TP_MGFCameraComponent.h"
#include "TP_MGFUICameraManagerComponent.h"

class FDebugDisplayInfo;

static FName UICameraComponentName(TEXT("UICamera"));

ATP_MGFPlayerCameraManager::ATP_MGFPlayerCameraManager(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	DefaultFOV   = TP_MGF_CAMERA_DEFAULT_FOV;
	ViewPitchMin = TP_MGF_CAMERA_DEFAULT_PITCH_MIN;
	ViewPitchMax = TP_MGF_CAMERA_DEFAULT_PITCH_MAX;

	UICamera = CreateDefaultSubobject<UTP_MGFUICameraManagerComponent>(UICameraComponentName);
}

UTP_MGFUICameraManagerComponent* ATP_MGFPlayerCameraManager::GetUICameraComponent() const
{
	return UICamera;
}

void ATP_MGFPlayerCameraManager::UpdateViewTarget(FTViewTarget& OutVT, float DeltaTime)
{
	// If the UI Camera is looking at something, let it have priority.
	if (UICamera->NeedsToUpdateViewTarget())
	{
		Super::UpdateViewTarget(OutVT, DeltaTime);
		UICamera->UpdateViewTarget(OutVT, DeltaTime);
		return;
	}

	Super::UpdateViewTarget(OutVT, DeltaTime);
}

void ATP_MGFPlayerCameraManager::DisplayDebug(UCanvas* Canvas, const FDebugDisplayInfo& DebugDisplay, float& YL, float& YPos)
{
	check(Canvas);

	FDisplayDebugManager& DisplayDebugManager = Canvas->DisplayDebugManager;

	DisplayDebugManager.SetFont(GEngine->GetSmallFont());
	DisplayDebugManager.SetDrawColor(FColor::Yellow);
	DisplayDebugManager.DrawString(FString::Printf(TEXT("TP_MGFPlayerCameraManager: %s"), *GetNameSafe(this)));

	Super::DisplayDebug(Canvas, DebugDisplay, YL, YPos);

	const APawn* Pawn = (PCOwner ? PCOwner->GetPawn() : nullptr);

	if (const UTP_MGFCameraComponent* CameraComponent = UTP_MGFCameraComponent::FindCameraComponent(Pawn))
	{
		CameraComponent->DrawDebug(Canvas);
	}
}