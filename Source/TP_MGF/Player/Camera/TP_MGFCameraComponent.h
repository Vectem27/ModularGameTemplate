#pragma once

#include "Camera/CameraComponent.h"

#include "TP_MGFCameraComponent.generated.h"

class UTP_MGFCameraMode;
class UTP_MGFCameraModeStack;
struct FGameplayTag;

DECLARE_DELEGATE_RetVal(TSubclassOf<UTP_MGFCameraMode>, FTP_MGFCameraModeDelegate);

/**
 * UTP_MGFCameraComponent
 *
 *	The base camera component class used by this project.
 */
UCLASS()
class UTP_MGFCameraComponent : public UCameraComponent
{
	GENERATED_BODY()

public:
	UTP_MGFCameraComponent(const FObjectInitializer& ObjectInitializer);

	// Returns the camera component if one exists on the specified actor.
	UFUNCTION(BlueprintPure, Category = "TP_MGF|Camera")
	static UTP_MGFCameraComponent* FindCameraComponent(const AActor* Actor) { return (Actor ? Actor->FindComponentByClass<UTP_MGFCameraComponent>() : nullptr); }

	// Returns the target actor that the camera is looking at.
	virtual AActor* GetTargetActor() const { return GetOwner(); }

	// Delegate used to query for the best camera mode.
	FTP_MGFCameraModeDelegate DetermineCameraModeDelegate;

	// Add an offset to the field of view.  The offset is only for one frame, it gets cleared once it is applied.
	void AddFieldOfViewOffset(float FovOffset) { FieldOfViewOffset += FovOffset; }

	virtual void DrawDebug(UCanvas* Canvas) const;

	// Gets the tag associated with the top layer and the blend weight of it
	void GetBlendInfo(float& OutWeightOfTopLayer, FGameplayTag& OutTagOfTopLayer) const;

protected:

	virtual void OnRegister() override;
	virtual void GetCameraView(float DeltaTime, FMinimalViewInfo& DesiredView) override;

	virtual void UpdateCameraModes();

protected:

	// Stack used to blend the camera modes.
	UPROPERTY()
	TObjectPtr<UTP_MGFCameraModeStack> CameraModeStack;

	// Offset applied to the field of view.  The offset is only for one frame, it gets cleared once it is applied.
	float FieldOfViewOffset;
};
