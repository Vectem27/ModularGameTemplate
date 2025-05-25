#pragma once

#include "Camera/PlayerCameraManager.h"

#include "TP_MGFUICameraManagerComponent.generated.h"

class ATP_MGFPlayerCameraManager;

UCLASS(Transient, Within = TP_MGFPlayerCameraManager)
class UTP_MGFUICameraManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	static UTP_MGFUICameraManagerComponent* GetComponent(APlayerController* PC);

public:
	UTP_MGFUICameraManagerComponent();
	virtual void InitializeComponent() override;

	bool IsSettingViewTarget() const { return bUpdatingViewTarget; }
	AActor* GetViewTarget() const { return ViewTarget; }
	void SetViewTarget(AActor* InViewTarget, FViewTargetTransitionParams TransitionParams = FViewTargetTransitionParams());

	bool NeedsToUpdateViewTarget() const;
	void UpdateViewTarget(struct FTViewTarget& OutVT, float DeltaTime);

	void OnShowDebugInfo(AHUD* HUD, UCanvas* Canvas, const FDebugDisplayInfo& DisplayInfo, float& YL, float& YPos);

private:
	UPROPERTY(Transient)
	TObjectPtr<AActor> ViewTarget;

	UPROPERTY(Transient)
	bool bUpdatingViewTarget;
};
