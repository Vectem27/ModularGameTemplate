#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "TP_MGFGameStatics.generated.h"

class UUserFacingGameDefinition;

UCLASS()
class UTP_MGFGameStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	
	UFUNCTION(BlueprintCallable, meta=(WorldContext = "WorldContextObject"))
	static bool LaunchNewGame(
		const UObject* WorldContextObject, 
		FString GameId,
		UUserFacingGameDefinition* UserFacingGameDefinition
	);

	//UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
	//static void LoadGame(UObject* WorldContextObject, FString gameId);

	//UFUNCTION(BlueprintPure, meta = (WorldContext = "WorldContextObject"))
	//static UTP_MGFUserFacingGameDefinition* GetGameDefinitionFromGameId(UObject* WorldContextObject, FString gameId);

	//UFUNCTION(BlueprintPure, meta = (WorldContext = "WorldContextObject"))
	//static FString GetCurrentGameId(UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
	static void LaunchGameGeneric(
		const UObject* WorldContextObject,
		FPrimaryAssetId MapId,
		FString GameId, 
		FPrimaryAssetId GameDefinitionId, 
		FPrimaryAssetId UserFacingGameDefinitionId
	);
	
};
