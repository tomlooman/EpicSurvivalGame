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

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerPickup();

	void ServerPickup_Implementation();

	bool ServerPickup_Validate();

	UFUNCTION(Reliable, NetMulticast)
	void OnPickupMulticast(AActor* FocusActor);

	void OnPickupMulticast_Implementation(AActor* FocusActor);

	/* Release the currently attached object, if not blocked by environment */
	void Drop();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerDrop();

	void ServerDrop_Implementation();

	bool ServerDrop_Validate();

	UFUNCTION(Reliable, NetMulticast)
	void OnDropMulticast();

	void OnDropMulticast_Implementation();

	/* Throw any currently carried Actor in the current view direction */
	void Throw();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerThrow();

	void ServerThrow_Implementation();

	bool ServerThrow_Validate();

	/* Is currently holding an Actor */
	bool GetIsCarryingActor();

	/* Rotate the Actor around the Up-axis.  */
	void Rotate(float Direction);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerRotate(float Direction);

	void ServerRotate_Implementation(float Direction);

	bool ServerRotate_Validate(float Direction);

	UFUNCTION(Reliable, NetMulticast)
	void OnRotateMulticast(float Direction);

	void OnRotateMulticast_Implementation(float Direction);
};
