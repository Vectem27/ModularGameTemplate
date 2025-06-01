#include "Player/TP_MGFPlayerState.h"

// Engine includes
#include "Engine/World.h"
#include "Components/GameFrameworkComponentManager.h"

// Project includes
#include "AbilitySystem/TP_MGFAbilitySet.h"
#include "AbilitySystem/TP_MGFAbilitySystemComponent.h"
#include "Game/GameDefinition/PawnData.h"
#include "Pawn/PawnExtensionComponent.h"
#include "Player/TP_MGFPlayerController.h"
#include "Game/GameDefinition/GameDefinitionManagerComponent.h"
#include "Game/TP_MGFGameMode.h"
#include "TP_MGFLogChannels.h"

const FName ATP_MGFPlayerState::NAME_TP_MGFAbilityReady("TP_MGFAbilitiesReady");


ATP_MGFPlayerState::ATP_MGFPlayerState(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	AbilitySystemComponent = ObjectInitializer.CreateDefaultSubobject<UTP_MGFAbilitySystemComponent>(this, TEXT("AbilitySystemComponent"));
}


void ATP_MGFPlayerState::SetPawnData(const UPawnData* InPawnData)
{
	check(InPawnData);

	if (GetLocalRole() != ROLE_Authority)
	{
		return;
	}

	if (PawnData)
	{
		UE_LOG(LogTP_MGF, Error, TEXT("Trying to set PawnData [%s] on player state [%s] that already has valid PawnData [%s]."), *GetNameSafe(InPawnData), *GetNameSafe(this), *GetNameSafe(PawnData));
		return;
	}

	PawnData = InPawnData;
	/* TODO: Uncomment
	for (const UTP_MGFAbilitySet* AbilitySet : PawnData->AbilitySets)
	{
		if (AbilitySet)
		{
			AbilitySet->GiveToAbilitySystem(AbilitySystemComponent, nullptr);
		}
	}*/

	UGameFrameworkComponentManager::SendGameFrameworkComponentExtensionEvent(this, NAME_TP_MGFAbilityReady);
}

void ATP_MGFPlayerState::OnGameDefinitionLoaded(const UGameDefinition* CurrentGameDefinition)
{
	if (ATP_MGFGameMode* TP_MGFGameMode = GetWorld()->GetAuthGameMode<ATP_MGFGameMode>())
	{
		/* TODO: Uncomment
		if (const UTP_MGFPawnData* NewPawnData = TP_MGFGameMode->GetPawnDataForController(GetOwningController()))
		{
			SetPawnData(NewPawnData);
		}
		else
		{
			UE_LOG(LogTP_MGF, Error, TEXT("ATP_MGFPlayerState::OnExperienceLoaded(): Unable to find PawnData to initialize player state [%s]!"), *GetNameSafe(this));
		}*/
	}
}


/////////////////////////////////////////////////////////////////////////////
// AActor interface

void ATP_MGFPlayerState::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	check(AbilitySystemComponent);
	AbilitySystemComponent->InitAbilityActorInfo(this, GetPawn());

	UWorld* World = GetWorld();
	if (World && World->IsGameWorld() && World->GetNetMode() != NM_Client)
	{
		AGameStateBase* GameState = GetWorld()->GetGameState();
		check(GameState);
		UGameDefinitionManagerComponent* GameDefinitionComponent = GameState->FindComponentByClass<UGameDefinitionManagerComponent>();
		check(GameDefinitionComponent);
		GameDefinitionComponent->CallOrRegister_OnGameDefinitionLoaded(FOnGameDefinitionLoaded::FDelegate::CreateUObject(this, &ThisClass::OnGameDefinitionLoaded));
	}

	if (UTP_MGFPawnExtensionComponent* PawnExtComp = UTP_MGFPawnExtensionComponent::FindPawnExtensionComponent(GetPawn()))
	{
		PawnExtComp->CheckDefaultInitialization();
	}
}

/////////////////////////////////////////////////////////////////////////////


ATP_MGFPlayerController* ATP_MGFPlayerState::GetTP_MGFPlayerController() const
{
	return Cast<ATP_MGFPlayerController>(GetOwner());
}

UAbilitySystemComponent* ATP_MGFPlayerState::GetAbilitySystemComponent() const
{
	return GetTP_MGFAbilitySystemComponent();
}