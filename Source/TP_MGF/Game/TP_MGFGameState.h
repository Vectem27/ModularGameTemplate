#pragma once

#include "AbilitySystemInterface.h"
#include "ModularGameState.h"

#include "TP_MGFGameState.generated.h"

class APlayerState;
class UAbilitySystemComponent;
class UGameDefinitionManagerComponent;
class UTP_MGFAbilitySystemComponent;

UCLASS()
class ATP_MGFGameState : public AModularGameStateBase, public IAbilitySystemInterface
{
	GENERATED_BODY()
	
public:
	ATP_MGFGameState(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	//~AActor interface
	virtual void PostInitializeComponents() override;
	//~End of AActor interface
	
	//~IAbilitySystemInterface
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	//~End of IAbilitySystemInterface

private:
	// Handles loading and managing the current gameplay experience
	UPROPERTY()
	TObjectPtr<UGameDefinitionManagerComponent> ExperienceManagerComponent;

	// The ability system component subobject for game-wide things (primarily gameplay cues)
	UPROPERTY(VisibleAnywhere, Category = "TP_MGF|GameState")
	TObjectPtr<UTP_MGFAbilitySystemComponent> AbilitySystemComponent;
};
