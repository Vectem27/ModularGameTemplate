#pragma once

#include "ModularPawn.h"

#include "TP_MGFPawn.generated.h"

/* ATP_MGFPawn */
UCLASS()
class TP_MGF_API ATP_MGFPawn : public AModularPawn
{
	GENERATED_BODY()

public:
	ATP_MGFPawn(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
};
