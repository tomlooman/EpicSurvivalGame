// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GameFramework/SpringArmComponent.h"
#include "SCarryObjectComponent.generated.h"

/**
 * 
 */
UCLASS(ClassGroup = (Survival), meta = (BlueprintSpawnableComponent))
class SURVIVALGAME_API USCarryObjectComponent : public USpringArmComponent
{
	GENERATED_BODY()

	USCarryObjectComponent(const FObjectInitializer& ObjectInitializer);

private:

	AActor* GetActorInView();

	AActor* GetCarriedActor();

	/* Return the StaticMeshComponent of the carried Actor */
	UStaticMeshComponent* GetCarriedMeshComp();

protected:

	void TickComponent(float DeltaSeconds, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;

	/* Rotation speed */
	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float RotateSpeed;

public:

	UPROPERTY(EditDefaultsOnly, Category = "Pickup")
	float MaxPickupDistance;

	/* Attach the current view focus object that is allowed to be picked up to the spring arm */
	void Pickup();

	/* Release the currently attached object, if not blocked by environment */
	void Drop();

	/* Throw any currently carried Actor in the current view direction */
	void Throw();

	/* Is currently holding an Actor */
	bool IsCarryingActor();

	/* Rotate the Actor around the Up-axis.  */
	void Rotate(float Direction);
};
