#pragma once

#include "NativeGameplayTags.h"

namespace TP_MGFGameplayTags
{
	TP_MGF_API	FGameplayTag FindTagByString(const FString& TagString, bool bMatchPartialString = false);

	// Declare all of the custom native tags that TP_MGF will use
	TP_MGF_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_ActivateFail_IsDead);
	TP_MGF_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_ActivateFail_Cooldown);
	TP_MGF_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_ActivateFail_Cost);
	TP_MGF_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_ActivateFail_TagsBlocked);
	TP_MGF_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_ActivateFail_TagsMissing);
	TP_MGF_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_ActivateFail_ActivationGroup);

	TP_MGF_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Move);
	TP_MGF_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Look_Mouse);
	TP_MGF_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Look_Stick);
	TP_MGF_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Crouch);

	TP_MGF_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(InitState_Spawned);
	TP_MGF_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(InitState_DataAvailable);
	TP_MGF_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(InitState_DataInitialized);
	TP_MGF_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(InitState_GameplayReady);

	TP_MGF_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayEvent_Death);
	TP_MGF_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayEvent_Reset);
	TP_MGF_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayEvent_RequestReset);

	TP_MGF_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Status_Crouching);
	TP_MGF_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Status_Death);
	TP_MGF_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Status_Death_Dying);
	TP_MGF_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Status_Death_Dead);

	// These are mappings from MovementMode enums to GameplayTags associated with those enums (below)
	TP_MGF_API	extern const TMap<uint8, FGameplayTag> MovementModeTagMap;
	TP_MGF_API	extern const TMap<uint8, FGameplayTag> CustomMovementModeTagMap;

	TP_MGF_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Movement_Mode_Walking);
	TP_MGF_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Movement_Mode_NavWalking);
	TP_MGF_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Movement_Mode_Falling);
	TP_MGF_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Movement_Mode_Swimming);
	TP_MGF_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Movement_Mode_Flying);

	TP_MGF_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Movement_Mode_Custom);
}