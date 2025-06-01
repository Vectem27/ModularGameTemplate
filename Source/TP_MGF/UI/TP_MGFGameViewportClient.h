#pragma once

#include "CommonGameViewportClient.h"

#include "TP_MGFGameViewportClient.generated.h"

UCLASS(BlueprintType)
class UTP_MGFGameViewportClient : public UCommonGameViewportClient
{
	GENERATED_BODY()

public:
	UTP_MGFGameViewportClient();

	virtual void Init(struct FWorldContext& WorldContext, UGameInstance* OwningGameInstance, bool bCreateNewAudioDevice = true) override;

};
