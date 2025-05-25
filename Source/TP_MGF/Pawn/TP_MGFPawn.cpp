#include "Pawn/TP_MGFPawn.h"

ATP_MGFPawn::ATP_MGFPawn(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
}