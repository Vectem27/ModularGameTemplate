#pragma once

#include "Engine/DataAsset.h"

#include "UserFacingGameDefinition.generated.h"

class UWorld;
class UGameDefinition;

/** Description of settings used to display game definition in the UI and start a new game */
UCLASS(BlueprintType)
class UUserFacingGameDefinition : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	/** The specific map to load */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Game", meta=(AllowedTypes = "Map"))
	FPrimaryAssetId MapId;

	/** The game definition to load */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Game", meta=(AllowedTypes = "GameDefinition"))
	FPrimaryAssetId GameDefinitionId;

	/** Extra arguments passed as URL options to the game */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Experience)
	TMap<FString, FString> ExtraArgs;

	/** Primary title in the UI */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Experience)
	FText GameTitle;

	/** Secondary title */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Experience)
	FText GameSubTitle;

	/** Full description */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Experience)
	FText GameDescription;

	/** Icon used in the UI */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Experience)
	TObjectPtr<UTexture2D> GameIcon;

	/** The loading screen widget to show when loading into (or back out of) a given experience */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = LoadingScreen)
	TSoftClassPtr<UUserWidget> LoadingScreenWidget;

	/** If true, this will show up in the game list in the menu */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Experience)
	bool bShowInMenu = true;
};
