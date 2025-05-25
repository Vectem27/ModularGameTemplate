#pragma once

#include "AbilitySystemInterface.h"
#include "ModularPlayerState.h"
#include "TP_MGFPlayerState.generated.h"

class ATP_MGFPlayerController;
class UTP_MGFAbilitySystemComponent;
class UPawnData;
class UGameDefinition;

/**
 * ATP_MGFPlayerState
 *
 *	Base player state class used by this project.
 */
UCLASS()
class TP_MGF_API ATP_MGFPlayerState : public AModularPlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()
	
public:
	ATP_MGFPlayerState(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintCallable, Category = "TP_MGF|PlayerState")
	ATP_MGFPlayerController* GetTP_MGFPlayerController() const;

	UFUNCTION(BlueprintCallable, Category = "TP_MGF|PlayerState")
	UTP_MGFAbilitySystemComponent* GetTP_MGFAbilitySystemComponent() const { return AbilitySystemComponent; }
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	template <class T>
	const T* GetPawnData() const { return Cast<T>(PawnData); }

	void SetPawnData(const UPawnData* InPawnData);

	//~AActor interface
	virtual void PostInitializeComponents() override;
	//~End of AActor interface

	static const FName NAME_TP_MGFAbilityReady;

	void OnGameDefinitionLoaded(const UGameDefinition* CurrentGameDefinition);

protected:
	UPROPERTY()
	TObjectPtr<const UPawnData> PawnData;

private:
	// The ability system component sub-object used by player characters.
	UPROPERTY(VisibleAnywhere, Category = "TP_MGF|PlayerState")
	TObjectPtr<UTP_MGFAbilitySystemComponent> AbilitySystemComponent;
};
