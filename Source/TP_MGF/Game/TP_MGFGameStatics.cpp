#include "Game/TP_MGFGameStatics.h"

// Engine includes
#include "Kismet/GameplayStatics.h" 

#include "Engine/AssetManager.h"
#include "Engine/World.h"
#include "Engine/StreamableManager.h"
#include "UObject/SoftObjectPath.h"

// Project includes
#include "Game/GameDefinition/GameDefinition.h"
#include "Game/GameDefinition/UserFacingGameDefinition.h"

FString BuildMapOptions(const FString& MapName, const TMap<FString, FString>& OptionMap)
{
    FString Options = "";

    bool isFirst = true;
    for (const auto& Pair : OptionMap)
    {
        if (!isFirst)
            Options +=  TEXT("?");
        isFirst = false;

        Options += Pair.Key + TEXT("=") + Pair.Value;
    }

    return Options;
}

bool UTP_MGFGameStatics::LaunchNewGame(const UObject* WorldContextObject, FString GameId, UUserFacingGameDefinition* UserFacingGameDefinition)
{
    if (!IsValid(UserFacingGameDefinition))
    {
        return false;
    }

    LaunchGameGeneric(
        WorldContextObject,
        UserFacingGameDefinition->MapId,
        GameId,
        UserFacingGameDefinition->GameDefinitionId,
        UAssetManager::Get().GetPrimaryAssetIdForObject(UserFacingGameDefinition)
    );

    return true;
}

void UTP_MGFGameStatics::LaunchGameGeneric(const UObject* WorldContextObject, FPrimaryAssetId MapId, FString GameId, FPrimaryAssetId GameDefinitionId, FPrimaryAssetId UserFacingGameDefinitionId)
{
	const FString GameDefinitionName = GameDefinitionId.PrimaryAssetName.ToString();
	const FString UserFacingGameDefinitionName = UserFacingGameDefinitionId.PrimaryAssetName.ToString();
    const FString MapName = MapId.PrimaryAssetName.ToString();

    TMap<FString, FString> OptionMap;
    OptionMap.Add("GameId", GameId);
    OptionMap.Add("GameDefinition", GameDefinitionName);
    OptionMap.Add("UserFacingGameDefinition", UserFacingGameDefinitionName);

    FString Options = BuildMapOptions(MapName, OptionMap);

	UGameplayStatics::OpenLevel(WorldContextObject, FName(*MapName), false, Options);
}
