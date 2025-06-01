#pragma once

#include "GameFramework/CharacterMovementComponent.h"
#include "NativeGameplayTags.h"

#include "TP_MGFCharacterMovementComponent.generated.h"

TP_MGF_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Gameplay_MovementStopped);

/**
 * FTP_MGFCharacterGroundInfo
 *
 *	Information about the ground under the character.  It only gets updated as needed.
 */
USTRUCT(BlueprintType)
struct FTP_MGFCharacterGroundInfo
{
	GENERATED_BODY()

	FTP_MGFCharacterGroundInfo()
		: LastUpdateFrame(0)
		, GroundDistance(0.0f)
	{
	}

	uint64 LastUpdateFrame;

	UPROPERTY(BlueprintReadOnly)
	FHitResult GroundHitResult;

	UPROPERTY(BlueprintReadOnly)
	float GroundDistance;
};

/**
 * UTP_MGFCharacterMovementComponent
 *
 *	The base character movement component class used by this project.
 */
UCLASS(Config = Game)
class TP_MGF_API UTP_MGFCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

public:
	UTP_MGFCharacterMovementComponent(const FObjectInitializer& ObjectInitializer);

	// Returns the current ground info.  Calling this will update the ground info if it's out of date.
	UFUNCTION(BlueprintCallable, Category = "TP_MGF|CharacterMovement")
	const FTP_MGFCharacterGroundInfo& GetGroundInfo();
	
	//~UMovementComponent interface
	virtual FRotator GetDeltaRotation(float DeltaTime) const override;
	virtual float GetMaxSpeed() const override;
	//~End of UMovementComponent interface

	//~UCharacterMovementComponent
	virtual bool CanAttemptJump() const override;
	//~End of UCharacterMovementComponent

protected:
	// Cached ground info for the character.  Do not access this directly!  It's only updated when accessed via GetGroundInfo().
	FTP_MGFCharacterGroundInfo CachedGroundInfo;
};
