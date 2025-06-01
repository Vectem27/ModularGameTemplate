#pragma once

#include "AbilitySystemComponent.h"
#include "GameplayAbilitySpec.h" // for FGameplayAbilitySpecHandle
#include "EnhancedInputComponent.h" // for FInputBindingHandle

#include "AbilitySystemComponentExtension.generated.h"

class UInputAction;

USTRUCT()
struct FAbilityInputBinding
{
	GENERATED_BODY()

	int32  InputID = 0;
	uint32 OnPressedHandle = 0;
	uint32 OnReleasedHandle = 0;
	TArray<FGameplayAbilitySpecHandle> BoundAbilitiesStack;

	// Pour détecter les resets d'input component
	TWeakObjectPtr<UEnhancedInputComponent> BoundInputComponent = nullptr;

	bool IsStillBoundTo(UEnhancedInputComponent* InputComp) const
	{
		return BoundInputComponent.IsValid() && BoundInputComponent.Get() == InputComp;
	}

	void Reset()
	{
		OnPressedHandle = 0;
		OnReleasedHandle = 0;
		BoundInputComponent = nullptr;
	}
};


/**
 *
 */
UCLASS(Blueprintable, BlueprintType, Category = "AbilityInput", meta = (BlueprintSpawnableComponent))
class MODULARGAMEABILITYEXTENSION_API UAbilitySystemComponentExtension : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Enhanced Input Abilities")
	void SetInputBinding(UInputAction* InputAction, FGameplayAbilitySpecHandle AbilityHandle);

	UFUNCTION(BlueprintCallable, Category = "Enhanced Input Abilities")
	void ClearInputBinding(FGameplayAbilitySpecHandle AbilityHandle);

	UFUNCTION(BlueprintCallable, Category = "Enhanced Input Abilities")
	void ClearAbilityBindings(UInputAction* InputAction);

private:
	UEnhancedInputComponent* GetEnhancedInputComponent() const;

	void OnAbilityInputPressed(UInputAction* InputAction);

	void OnAbilityInputReleased(UInputAction* InputAction);

	void RemoveEntry(UInputAction* InputAction);

	void TryBindAbilityInput(UInputAction* InputAction, FAbilityInputBinding& AbilityInputBinding);

	FGameplayAbilitySpec* FindAbilitySpec(FGameplayAbilitySpecHandle Handle);

	UPROPERTY(transient)
	TMap<UInputAction*, FAbilityInputBinding> MappedAbilities;
};
