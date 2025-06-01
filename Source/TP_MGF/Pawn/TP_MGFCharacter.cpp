#include "Pawn/TP_MGFCharacter.h"

// Engine
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "TimerManager.h"

// Project
#include "AbilitySystem/TP_MGFAbilitySystemComponent.h"

#include "Pawn/PawnExtensionComponent.h"
#include "Pawn/TP_MGFCharacterMovementComponent.h"

#include "Player/TP_MGFPlayerState.h"
#include "Player/TP_MGFPlayerController.h"

#include "System/TP_MGFSignificanceManager.h"

#include "TP_MGFGameplayTags.h"
#include "TP_MGFLogChannels.h"

static FName NAME_TP_MGFCharacterCollisionProfile_Capsule(TEXT("TP_MGFPawnCapsule"));
static FName NAME_TP_MGFCharacterCollisionProfile_Mesh(TEXT("TP_MGFPawnMesh"));

ATP_MGFCharacter::ATP_MGFCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UTP_MGFCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	// Avoid ticking characters if possible.
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

	UCapsuleComponent* CapsuleComp = GetCapsuleComponent();
	check(CapsuleComp);
	CapsuleComp->InitCapsuleSize(40.0f, 90.0f);
	CapsuleComp->SetCollisionProfileName(NAME_TP_MGFCharacterCollisionProfile_Capsule);

	USkeletalMeshComponent* MeshComp = GetMesh();
	check(MeshComp);
	MeshComp->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));  // Rotate mesh to be X forward since it is exported as Y forward.
	MeshComp->SetCollisionProfileName(NAME_TP_MGFCharacterCollisionProfile_Mesh);

	UTP_MGFCharacterMovementComponent* TP_MGFMoveComp = CastChecked<UTP_MGFCharacterMovementComponent>(GetCharacterMovement());
	TP_MGFMoveComp->GravityScale = 1.0f;
	TP_MGFMoveComp->MaxAcceleration = 2400.0f;
	TP_MGFMoveComp->BrakingFrictionFactor = 1.0f;
	TP_MGFMoveComp->BrakingFriction = 6.0f;
	TP_MGFMoveComp->GroundFriction = 8.0f;
	TP_MGFMoveComp->BrakingDecelerationWalking = 1400.0f;
	TP_MGFMoveComp->bUseControllerDesiredRotation = false;
	TP_MGFMoveComp->bOrientRotationToMovement = false;
	TP_MGFMoveComp->RotationRate = FRotator(0.0f, 720.0f, 0.0f);
	TP_MGFMoveComp->bAllowPhysicsRotationDuringAnimRootMotion = false;
	TP_MGFMoveComp->GetNavAgentPropertiesRef().bCanCrouch = true;
	TP_MGFMoveComp->bCanWalkOffLedgesWhenCrouching = true;
	TP_MGFMoveComp->SetCrouchedHalfHeight(65.0f);

	//PawnExtComponent = CreateDefaultSubobject<UTP_MGFPawnExtensionComponent>(TEXT("PawnExtensionComponent"));
	//PawnExtComponent->OnAbilitySystemInitialized_RegisterAndCall(FSimpleMulticastDelegate::FDelegate::CreateUObject(this, &ThisClass::OnAbilitySystemInitialized));
	//PawnExtComponent->OnAbilitySystemUninitialized_Register(FSimpleMulticastDelegate::FDelegate::CreateUObject(this, &ThisClass::OnAbilitySystemUninitialized));

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;

	BaseEyeHeight = 80.0f;
	CrouchedEyeHeight = 50.0f;
}

////////////////////////////////////////////////////////////////////////////
// AActor Methods

void ATP_MGFCharacter::BeginPlay()
{
	Super::BeginPlay();

	UWorld* World = GetWorld();

	check(World);

	if (UTP_MGFSignificanceManager* SignificanceManager = USignificanceManager::Get<UTP_MGFSignificanceManager>(World))
	{
//@TODO:    SignificanceManager->RegisterObject(this, (EFortSignificanceType)SignificanceType);
	}
}

void ATP_MGFCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	UWorld* World = GetWorld();
	
	check(World);

	if (UTP_MGFSignificanceManager* SignificanceManager = USignificanceManager::Get<UTP_MGFSignificanceManager>(World))
	{
		SignificanceManager->UnregisterObject(this);
	}
}

void ATP_MGFCharacter::Reset()
{
	DisableMovementAndCollision();

	K2_OnReset();

	UninitAndDestroy();
}

void ATP_MGFCharacter::FellOutOfWorld(const class UDamageType& dmgType)
{
	// TODO: Adding death logic here.
}

////////////////////////////////////////////////////////////////////////////////
// APawn Methods

void ATP_MGFCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	PawnExtComponent->HandleControllerChanged();
}

void ATP_MGFCharacter::UnPossessed()
{
	Super::UnPossessed();

	PawnExtComponent->HandleControllerChanged();
}

void ATP_MGFCharacter::OnRep_Controller()
{
	Super::OnRep_Controller();

	PawnExtComponent->HandleControllerChanged();
}

void ATP_MGFCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	PawnExtComponent->HandlePlayerStateReplicated();
}

void ATP_MGFCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PawnExtComponent->SetupPlayerInputComponent();
}


///////////////////////////////////////////////////////////////////////////////
// ACharacter Methods

void ATP_MGFCharacter::OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode)
{
	Super::OnMovementModeChanged(PrevMovementMode, PreviousCustomMode);

	UTP_MGFCharacterMovementComponent* TP_MGFMoveComp = CastChecked<UTP_MGFCharacterMovementComponent>(GetCharacterMovement());

	SetMovementModeTag(PrevMovementMode, PreviousCustomMode, false);
	SetMovementModeTag(TP_MGFMoveComp->MovementMode, TP_MGFMoveComp->CustomMovementMode, true);
}

void ATP_MGFCharacter::OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	if (UTP_MGFAbilitySystemComponent* TP_MGFASC = GetTP_MGFAbilitySystemComponent())
	{
		TP_MGFASC->SetLooseGameplayTagCount(TP_MGFGameplayTags::Status_Crouching, 1);
	}


	Super::OnStartCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);
}

void ATP_MGFCharacter::OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	if (UTP_MGFAbilitySystemComponent* TP_MGFASC = GetTP_MGFAbilitySystemComponent())
	{
		TP_MGFASC->SetLooseGameplayTagCount(TP_MGFGameplayTags::Status_Crouching, 0);
	}

	Super::OnEndCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);
}


///////////////////////////////////////////////////////////////////////////////
// This Class Methods

ATP_MGFPlayerController* ATP_MGFCharacter::GetTP_MGFPlayerController() const
{
	return CastChecked<ATP_MGFPlayerController>(Controller, ECastCheckedType::NullAllowed);
}

ATP_MGFPlayerState* ATP_MGFCharacter::GetTP_MGFPlayerState() const
{
	return CastChecked<ATP_MGFPlayerState>(GetPlayerState(), ECastCheckedType::NullAllowed);
}

UTP_MGFAbilitySystemComponent* ATP_MGFCharacter::GetTP_MGFAbilitySystemComponent() const
{
	return Cast<UTP_MGFAbilitySystemComponent>(GetAbilitySystemComponent());
}

UAbilitySystemComponent* ATP_MGFCharacter::GetAbilitySystemComponent() const
{
	if (PawnExtComponent == nullptr)
	{
		return nullptr;
	}

	return PawnExtComponent->GetTP_MGFAbilitySystemComponent();
}

void ATP_MGFCharacter::OnAbilitySystemInitialized()
{
	UTP_MGFAbilitySystemComponent* TP_MGFASC = GetTP_MGFAbilitySystemComponent();
	check(TP_MGFASC);

	InitializeGameplayTags();
}

void ATP_MGFCharacter::OnAbilitySystemUninitialized()
{
	// Empty for now.
}

void ATP_MGFCharacter::InitializeGameplayTags()
{
	// Clear tags that may be lingering on the ability system from the previous pawn.
	if (UTP_MGFAbilitySystemComponent* TP_MGFASC = GetTP_MGFAbilitySystemComponent())
	{
		for (const TPair<uint8, FGameplayTag>& TagMapping : TP_MGFGameplayTags::MovementModeTagMap)
		{
			if (TagMapping.Value.IsValid())
			{
				TP_MGFASC->SetLooseGameplayTagCount(TagMapping.Value, 0);
			}
		}

		for (const TPair<uint8, FGameplayTag>& TagMapping : TP_MGFGameplayTags::CustomMovementModeTagMap)
		{
			if (TagMapping.Value.IsValid())
			{
				TP_MGFASC->SetLooseGameplayTagCount(TagMapping.Value, 0);
			}
		}

		UTP_MGFCharacterMovementComponent* TP_MGFMoveComp = CastChecked<UTP_MGFCharacterMovementComponent>(GetCharacterMovement());
		SetMovementModeTag(TP_MGFMoveComp->MovementMode, TP_MGFMoveComp->CustomMovementMode, true);
	}
}

void ATP_MGFCharacter::OnDeathStarted(AActor* OwningActor)
{
	DisableMovementAndCollision();
}

void ATP_MGFCharacter::OnDeathFinished(AActor* OwningActor)
{
	GetWorld()->GetTimerManager().SetTimerForNextTick(this, &ThisClass::DestroyDueToDeath);
}

void ATP_MGFCharacter::DisableMovementAndCollision()
{
	if (Controller)
	{
		Controller->SetIgnoreMoveInput(true);
	}

	UCapsuleComponent* CapsuleComp = GetCapsuleComponent();
	check(CapsuleComp);
	CapsuleComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CapsuleComp->SetCollisionResponseToAllChannels(ECR_Ignore);

	UTP_MGFCharacterMovementComponent* TP_MGFMoveComp = CastChecked<UTP_MGFCharacterMovementComponent>(GetCharacterMovement());
	TP_MGFMoveComp->StopMovementImmediately();
	TP_MGFMoveComp->DisableMovement();
}

void ATP_MGFCharacter::DestroyDueToDeath()
{
	K2_OnDeathFinished();

	UninitAndDestroy();
}

void ATP_MGFCharacter::UninitAndDestroy()
{
	if (GetLocalRole() == ROLE_Authority)
	{
		DetachFromControllerPendingDestroy();
		SetLifeSpan(0.1f);
	}

	// Uninitialize the ASC if we're still the avatar actor (otherwise another pawn already did it when they became the avatar actor)
	if (UTP_MGFAbilitySystemComponent* TP_MGFASC = GetTP_MGFAbilitySystemComponent())
	{
		if (TP_MGFASC->GetAvatarActor() == this)
		{
			PawnExtComponent->UninitializeAbilitySystem();
		}
	}

	SetActorHiddenInGame(true);
}

void ATP_MGFCharacter::SetMovementModeTag(EMovementMode MovementMode, uint8 CustomMovementMode, bool bTagEnabled)
{
	if (UTP_MGFAbilitySystemComponent* TP_MGFASC = GetTP_MGFAbilitySystemComponent())
	{
		const FGameplayTag* MovementModeTag = nullptr;
		if (MovementMode == MOVE_Custom)
		{
			MovementModeTag = TP_MGFGameplayTags::CustomMovementModeTagMap.Find(CustomMovementMode);
		}
		else
		{
			MovementModeTag = TP_MGFGameplayTags::MovementModeTagMap.Find(MovementMode);
		}

		if (MovementModeTag && MovementModeTag->IsValid())
		{
			TP_MGFASC->SetLooseGameplayTagCount(*MovementModeTag, (bTagEnabled ? 1 : 0));
		}
	}
}

void ATP_MGFCharacter::ToggleCrouch()
{
	const UTP_MGFCharacterMovementComponent* TP_MGFMoveComp = CastChecked<UTP_MGFCharacterMovementComponent>(GetCharacterMovement());

	if (bIsCrouched || TP_MGFMoveComp->bWantsToCrouch)
	{
		UnCrouch();
	}
	else if (TP_MGFMoveComp->IsMovingOnGround())
	{
		Crouch();
	}
}

bool ATP_MGFCharacter::CanJumpInternal_Implementation() const
{
	// same as ACharacter's implementation but without the crouch check
	return JumpIsAllowedInternal();
}

////////////////////////////////////////////////////////////////////////////
// IGameplayTagAssetInterface Methods

void ATP_MGFCharacter::GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const
{
	if (const UTP_MGFAbilitySystemComponent* TP_MGFASC = GetTP_MGFAbilitySystemComponent())
	{
		TP_MGFASC->GetOwnedGameplayTags(TagContainer);
	}
}

bool ATP_MGFCharacter::HasMatchingGameplayTag(FGameplayTag TagToCheck) const
{
	if (const UTP_MGFAbilitySystemComponent* TP_MGFASC = GetTP_MGFAbilitySystemComponent())
	{
		return TP_MGFASC->HasMatchingGameplayTag(TagToCheck);
	}

	return false;
}

bool ATP_MGFCharacter::HasAllMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const
{
	if (const UTP_MGFAbilitySystemComponent* TP_MGFASC = GetTP_MGFAbilitySystemComponent())
	{
		return TP_MGFASC->HasAllMatchingGameplayTags(TagContainer);
	}

	return false;
}

bool ATP_MGFCharacter::HasAnyMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const
{
	if (const UTP_MGFAbilitySystemComponent* TP_MGFASC = GetTP_MGFAbilitySystemComponent())
	{
		return TP_MGFASC->HasAnyMatchingGameplayTags(TagContainer);
	}

	return false;
}
