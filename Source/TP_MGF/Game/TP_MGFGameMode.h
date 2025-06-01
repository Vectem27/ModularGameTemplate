#pragma once

#include "ModularGameMode.h"

#include "TP_MGFGameMode.generated.h"

class UPawnData;
class UGameDefinition;

/**
 * Post login event, triggered when a player or bot joins the game as well as after seamless and non seamless travel
 *
 * This is called after the player has finished initialization
 */
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnTP_MGFGameModePlayerInitialized, AGameModeBase* /*GameMode*/, AController* /*NewPlayer*/);


UCLASS(meta = (ShortTooltip = "The base game mode class used by this project."))
class TP_MGF_API ATP_MGFGameMode : public AModularGameModeBase
{
	GENERATED_BODY()
	
public:
	ATP_MGFGameMode(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintCallable, Category = "TP_MGF|Pawn")
	const UPawnData* GetPawnDataForController(const AController* InController) const;

	UFUNCTION(BlueprintCallable, Category = "TP_MGF|Game")
	FString GetGameId() const;

	UFUNCTION(BlueprintCallable, Category = "TP_MGF|Game")
	virtual void SetGameId(FString NewGameId);

	//~AGameModeBase interface
	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;
	virtual void InitGameState() override;
	virtual UClass* GetDefaultPawnClassForController_Implementation(AController* InController) override;
	virtual APawn* SpawnDefaultPawnAtTransform_Implementation(AController* NewPlayer, const FTransform& SpawnTransform) override;
	virtual void HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer) override;
	virtual void GenericPlayerInitialization(AController* NewPlayer) override;
	virtual void FailedToRestartPlayer(AController* NewPlayer) override;
	//~End of AGameModeBase interface

	// Restart (respawn) the specified player next frame
	// - If bForceReset is true, the controller will be reset this frame (abandoning the currently possessed pawn, if any)
	UFUNCTION(BlueprintCallable)
	void RequestPlayerRestartNextFrame(AController* Controller, bool bForceReset = false);

	// Delegate called on player initialization, described above 
	FOnTP_MGFGameModePlayerInitialized OnGameModePlayerInitialized;
protected:
	void OnGameDefinitionLoaded(const UGameDefinition* CurrentGameDefinition);
	bool IsGameDefinitionLoaded() const;

	UFUNCTION(BlueprintImplementableEvent, DisplayName="On Game ID Set", Category = "TP_MGF|Game")
	void K2_OnGameIdSet(const FString& NewGameId);
private:
	UPROPERTY()
	FString GameId; 
};
