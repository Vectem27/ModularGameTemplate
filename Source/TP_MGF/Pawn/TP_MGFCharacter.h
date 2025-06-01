#pragma once

#include "ModularCharacter.h"
#include "GameplayTagAssetInterface.h"
#include "AbilitySystemInterface.h"
#include "GameplayCueInterface.h"

#include "TP_MGFCharacter.generated.h"

class ATP_MGFPlayerController;
class ATP_MGFPlayerState;
class UTP_MGFAbilitySystemComponent;
class UTP_MGFPawnExtensionComponent;
class UTP_MGFCameraComponent;

/**
 * ATP_MGFCharacter
 *
 *	The base character pawn class used by this project.
 *	Responsible for sending events to pawn components.
 *	New behavior should be added via pawn components when possible.
 */
UCLASS(Config = Game, Meta = (ShortTooltip = "The base character pawn class used by this project."))
class TP_MGF_API ATP_MGFCharacter : public AModularCharacter, 
	public IAbilitySystemInterface, public IGameplayCueInterface, public IGameplayTagAssetInterface
{
	GENERATED_BODY()

public: 
	ATP_MGFCharacter(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintCallable, Category = "TP_MGF|Character")
	ATP_MGFPlayerController* GetTP_MGFPlayerController() const;

	UFUNCTION(BlueprintCallable, Category = "TP_MGF|Character")
	ATP_MGFPlayerState* GetTP_MGFPlayerState() const;

	UFUNCTION(BlueprintCallable, Category = "TP_MGF|Character")
	UTP_MGFAbilitySystemComponent* GetTP_MGFAbilitySystemComponent() const;

	//~IAbilitySystemInterface interface
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	//~End of IAbilitySystemInterface interface
	
	
	//~IGameplayTagAssetInterface interface
	virtual void GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const override;
	virtual bool HasMatchingGameplayTag(FGameplayTag TagToCheck) const override;
	virtual bool HasAllMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const override;
	virtual bool HasAnyMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const override;
	//~End of IGameplayTagAssetInterface interface

	//~AActor interface
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void Reset() override;
	//~End of AActor interface

	void ToggleCrouch();

protected:
	//~AActor interface
	virtual void FellOutOfWorld(const class UDamageType& dmgType) override;
	//~End of AActor interface
	
	//~APawn interface
	virtual void PossessedBy(AController* NewController) override;
	virtual void UnPossessed() override;
	virtual void OnRep_Controller() override;
	virtual void OnRep_PlayerState() override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	//~End of APawn interface

	//~ACharacter interface
	virtual void OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode) override;
	virtual void OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;
	virtual void OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;
	//~End of ACharacter interface

	virtual void OnAbilitySystemInitialized();
	virtual void OnAbilitySystemUninitialized();

	void InitializeGameplayTags();

	// Begins the death sequence for the character (disables collision, disables movement, etc...)
	UFUNCTION()
	virtual void OnDeathStarted(AActor* OwningActor);

	// Ends the death sequence for the character (detaches controller, destroys pawn, etc...)
	UFUNCTION()
	virtual void OnDeathFinished(AActor* OwningActor);

	void DisableMovementAndCollision();
	void DestroyDueToDeath();
	void UninitAndDestroy();

	// Called when the death sequence for the character has completed
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "OnDeathFinished"))
	void K2_OnDeathFinished();

	void SetMovementModeTag(EMovementMode MovementMode, uint8 CustomMovementMode, bool bTagEnabled);

	virtual bool CanJumpInternal_Implementation() const;


private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "TP_MGF|Character", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UTP_MGFPawnExtensionComponent> PawnExtComponent;
};
