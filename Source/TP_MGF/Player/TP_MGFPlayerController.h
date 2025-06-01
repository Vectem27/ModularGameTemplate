#pragma once
#include "CommonPlayerController.h"

#include "TP_MGFPlayerController.generated.h"

class ATP_MGFPlayerState;
class UTP_MGFAbilitySystemComponent;
class ATP_MGFHUD;
class UTP_MGFSettingsShared;

/**
 * ATP_MGFPlayerController
 *
 *	The base player controller class used by this project.
 */
UCLASS(meta = (ShortTooltip = "The base player controller class used by this project."))
class TP_MGF_API ATP_MGFPlayerController : public ACommonPlayerController
{
	GENERATED_BODY()

public:
	ATP_MGFPlayerController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintCallable, Category = "TP_MGF|PlayerController")
	ATP_MGFPlayerState* GetTP_MGFPlayerState() const;

	UFUNCTION(BlueprintCallable, Category = "TP_MGF|PlayerController")
	UTP_MGFAbilitySystemComponent* GetTP_MGFAbilitySystemComponent() const;

	UFUNCTION(BlueprintCallable, Category = "TP_MGF|PlayerController")
	ATP_MGFHUD* GetTP_MGFHUD() const;

	//~AActor interface
	virtual void BeginPlay() override;
	//~End of AActor interface

	//~AController interface
	virtual void OnUnPossess() override;
	//~End of AController interface

	//~APlayerController interface
	virtual void SetPlayer(UPlayer* InPlayer) override;
	virtual void UpdateForceFeedback(IInputInterface* InputInterface, const int32 ControllerId) override;
	virtual void PreProcessInput(const float DeltaTime, const bool bGamePaused) override;
	virtual void PostProcessInput(const float DeltaTime, const bool bGamePaused) override;
	//~End of APlayerController interface

protected:
	void OnSettingsChanged(UTP_MGFSettingsShared* Settings);
};