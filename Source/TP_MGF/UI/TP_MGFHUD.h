#pragma once

#include "GameFramework/HUD.h"

#include "TP_MGFHUD.generated.h"

/**
 * ATP_MGFHUD
 *
 * Note that you typically do not need to extend or modify this class, instead you would
 * use an "Add Widget" action in your experience to add a HUD layout and widgets to it
 *
 * This class exists primarily for debug rendering
 */
UCLASS()
class ATP_MGFHUD : public AHUD
{
	GENERATED_BODY()
	
public:
	ATP_MGFHUD(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:

	//~UObject interface
	virtual void PreInitializeComponents() override;
	//~End of UObject interface

	//~AActor interface
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	//~End of AActor interface

	//~AHUD interface
	virtual void GetDebugActorList(TArray<AActor*>& InOutList) override;
	//~End of AHUD interface
};
