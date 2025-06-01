#include "AbilitySystemComponentExtension.h"

#include "InputAction.h"
#include "EnhancedInputComponent.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"

namespace AbilitySystemComponentExtension_Impl
{
	constexpr int32 InvalidInputID = 0;
	int32 IncrementingInputID = InvalidInputID;

	static int32 GetNextInputID()
	{
		return ++IncrementingInputID;
	}
}

void UAbilitySystemComponentExtension::SetInputBinding(UInputAction* InputAction, FGameplayAbilitySpecHandle AbilityHandle)
{
	using namespace AbilitySystemComponentExtension_Impl;

	FGameplayAbilitySpec* BindingAbility = FindAbilitySpec(AbilityHandle);

	FAbilityInputBinding* AbilityInputBinding = MappedAbilities.Find(InputAction);
	if (AbilityInputBinding)
	{
		FGameplayAbilitySpec* OldBoundAbility = FindAbilitySpec(AbilityInputBinding->BoundAbilitiesStack.Top());
		if (OldBoundAbility && OldBoundAbility->InputID == AbilityInputBinding->InputID)
		{
			OldBoundAbility->InputID = InvalidInputID;
		}
	}
	else
	{
		AbilityInputBinding = &MappedAbilities.Add(InputAction);
		AbilityInputBinding->InputID = GetNextInputID();
	}

	if (BindingAbility)
	{
		BindingAbility->InputID = AbilityInputBinding->InputID;
	}

	AbilityInputBinding->BoundAbilitiesStack.Push(AbilityHandle);
	TryBindAbilityInput(InputAction, *AbilityInputBinding);
}

void UAbilitySystemComponentExtension::ClearInputBinding(FGameplayAbilitySpecHandle AbilityHandle)
{
	using namespace AbilitySystemComponentExtension_Impl;

	if (FGameplayAbilitySpec* FoundAbility = FindAbilitySpec(AbilityHandle))
	{
		// Find the mapping for this ability
		auto MappedIterator = MappedAbilities.CreateIterator();
		while (MappedIterator)
		{
			if (MappedIterator.Value().InputID == FoundAbility->InputID)
			{
				break;
			}

			++MappedIterator;
		}

		if (MappedIterator)
		{
			FAbilityInputBinding& AbilityInputBinding = MappedIterator.Value();

			if (AbilityInputBinding.BoundAbilitiesStack.Remove(AbilityHandle) > 0)
			{
				if (AbilityInputBinding.BoundAbilitiesStack.Num() > 0)
				{
					FGameplayAbilitySpec* StackedAbility = FindAbilitySpec(AbilityInputBinding.BoundAbilitiesStack.Top());
					if (StackedAbility && StackedAbility->InputID == 0)
					{
						StackedAbility->InputID = AbilityInputBinding.InputID;
					}
				}
				else
				{
					// NOTE: This will invalidate the `AbilityInputBinding` ref above
					RemoveEntry(MappedIterator.Key());
				}
				// DO NOT act on `AbilityInputBinding` after here (it could have been removed)


				FoundAbility->InputID = InvalidInputID;
			}
		}
	}
}

void UAbilitySystemComponentExtension::ClearAbilityBindings(UInputAction* InputAction)
{
	RemoveEntry(InputAction);
}



void UAbilitySystemComponentExtension::OnAbilityInputPressed(UInputAction* InputAction)
{
	using namespace AbilitySystemComponentExtension_Impl;

	FAbilityInputBinding* FoundBinding = MappedAbilities.Find(InputAction);
	if (FoundBinding && ensure(FoundBinding->InputID != InvalidInputID))
	{
		AbilityLocalInputPressed(FoundBinding->InputID);
	}
}

void UAbilitySystemComponentExtension::OnAbilityInputReleased(UInputAction* InputAction)
{
	using namespace AbilitySystemComponentExtension_Impl;

	FAbilityInputBinding* FoundBinding = MappedAbilities.Find(InputAction);
	if (FoundBinding && ensure(FoundBinding->InputID != InvalidInputID))
	{
		AbilityLocalInputReleased(FoundBinding->InputID);
	}
}

void UAbilitySystemComponentExtension::RemoveEntry(UInputAction* InputAction)
{
	auto InputComponent = GetEnhancedInputComponent();


	if (FAbilityInputBinding* Bindings = MappedAbilities.Find(InputAction))
	{
		if (InputComponent)
		{
			InputComponent->RemoveBindingByHandle(Bindings->OnPressedHandle);
			InputComponent->RemoveBindingByHandle(Bindings->OnReleasedHandle);
		}

		for (FGameplayAbilitySpecHandle AbilityHandle : Bindings->BoundAbilitiesStack)
		{
			using namespace AbilitySystemComponentExtension_Impl;

			FGameplayAbilitySpec* AbilitySpec = FindAbilitySpec(AbilityHandle);
			if (AbilitySpec && AbilitySpec->InputID == Bindings->InputID)
			{
				AbilitySpec->InputID = InvalidInputID;
			}
		}

		MappedAbilities.Remove(InputAction);
	}
}

FGameplayAbilitySpec* UAbilitySystemComponentExtension::FindAbilitySpec(FGameplayAbilitySpecHandle Handle)
{
	FGameplayAbilitySpec* FoundAbility = nullptr;
	FoundAbility = FindAbilitySpecFromHandle(Handle);
	return FoundAbility;
}

void UAbilitySystemComponentExtension::TryBindAbilityInput(UInputAction* InputAction, FAbilityInputBinding& AbilityInputBinding)
{
	UEnhancedInputComponent* InputComponent = GetEnhancedInputComponent();
	if (!InputComponent)
	{
		TWeakObjectPtr<UAbilitySystemComponentExtension> WeakThis(this);

		GetWorld()->GetTimerManager().SetTimerForNextTick([WeakThis, InputAction, InputID = AbilityInputBinding.InputID]()
			{
				if (WeakThis.IsValid())
				{
					if (FAbilityInputBinding* FoundBinding = WeakThis->MappedAbilities.Find(InputAction))
					{
						WeakThis->TryBindAbilityInput(InputAction, *FoundBinding);
					}
				}
			});
		return;
	}

	// Vérifie si déjà lié au même InputComponent
	if (AbilityInputBinding.IsStillBoundTo(InputComponent))
	{
		return;
	}

	// Unbind ancien si différent
	if (AbilityInputBinding.OnPressedHandle != 0 || AbilityInputBinding.OnReleasedHandle != 0)
	{
		if (UEnhancedInputComponent* OldInput = AbilityInputBinding.BoundInputComponent.Get())
		{
			OldInput->RemoveBindingByHandle(AbilityInputBinding.OnPressedHandle);
			OldInput->RemoveBindingByHandle(AbilityInputBinding.OnReleasedHandle);
		}

		AbilityInputBinding.Reset();
	}

	AbilityInputBinding.OnPressedHandle = InputComponent->BindAction(
		InputAction,
		ETriggerEvent::Started,
		this,
		&UAbilitySystemComponentExtension::OnAbilityInputPressed,
		InputAction
	).GetHandle();

	AbilityInputBinding.OnReleasedHandle = InputComponent->BindAction(
		InputAction,
		ETriggerEvent::Completed,
		this,
		&UAbilitySystemComponentExtension::OnAbilityInputReleased,
		InputAction
	).GetHandle();

	AbilityInputBinding.BoundInputComponent = InputComponent;
}



UEnhancedInputComponent* UAbilitySystemComponentExtension::GetEnhancedInputComponent() const
{
	AActor* Owner = GetOwner();
	if (IsValid(Owner) && Owner->InputComponent)
	{
		return CastChecked<UEnhancedInputComponent>(Owner->InputComponent);
	}

	return nullptr;
}