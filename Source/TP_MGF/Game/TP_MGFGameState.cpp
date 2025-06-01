#include "Game/TP_MGFGameState.h"

#include "AbilitySystem/TP_MGFAbilitySystemComponent.h"
#include "Pawn/TP_MGFCharacter.h"
#include "Game/GameDefinition/GameDefinitionManagerComponent.h"


ATP_MGFGameState::ATP_MGFGameState(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	AbilitySystemComponent = ObjectInitializer.CreateDefaultSubobject<UTP_MGFAbilitySystemComponent>(this, TEXT("AbilitySystemComponent"));

	ExperienceManagerComponent = CreateDefaultSubobject<UGameDefinitionManagerComponent>(TEXT("GameDefinitionManagerComponent"));

}

void ATP_MGFGameState::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	check(AbilitySystemComponent);
	AbilitySystemComponent->InitAbilityActorInfo(/*Owner=*/ this, /*Avatar=*/ this);
}

UAbilitySystemComponent* ATP_MGFGameState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}
