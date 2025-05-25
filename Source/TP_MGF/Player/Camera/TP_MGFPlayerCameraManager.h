#pragma once

#include "Camera/PlayerCameraManager.h"

#include "TP_MGFPlayerCameraManager.generated.h"

class FObjectInitializer;

class FDebugDisplayInfo;
class UCanvas;

#define TP_MGF_CAMERA_DEFAULT_FOV		(80.0f)
#define TP_MGF_CAMERA_DEFAULT_PITCH_MIN	(-89.0f)
#define TP_MGF_CAMERA_DEFAULT_PITCH_MAX	(89.0f)

class UTP_MGFUICameraManagerComponent;

/**
 * UTP_MGFPlayerCameraManager
 *
 *	The base player camera manager class used by this project.
 */
UCLASS(notplaceable, MinimalAPI)
class ATP_MGFPlayerCameraManager : public APlayerCameraManager
{
	GENERATED_BODY()

public:
	ATP_MGFPlayerCameraManager(const FObjectInitializer& ObjectInitializer);

	UTP_MGFUICameraManagerComponent* GetUICameraComponent() const;

protected:

	virtual void UpdateViewTarget(FTViewTarget& OutVT, float DeltaTime) override;

	virtual void DisplayDebug(UCanvas* Canvas, const FDebugDisplayInfo& DebugDisplay, float& YL, float& YPos) override;

private:
	/** The UI Camera Component, controls the camera when UI is doing something important that gameplay doesn't get priority over. */
	UPROPERTY(Transient)
	TObjectPtr<UTP_MGFUICameraManagerComponent> UICamera;
};
