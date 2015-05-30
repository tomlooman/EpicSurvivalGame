// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "SurvivalGame.h"
#include "SCarryObjectComponent.h"


USCarryObjectComponent::USCarryObjectComponent(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	MaxPickupDistance = 600;
	RotateSpeed = 10.0f;

	bUsePawnControlRotation = true;
	bDoCollisionTest = false;

	//SetNetAddressable();
	SetIsReplicated(true);
}


void USCarryObjectComponent::TickComponent(float DeltaSeconds, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{

	if (IsCarryingActor())
	{
		// perform an overlap check of the actor vs. environment. must have 0 overlaps from Static and Dynamic and Pawn channels.

		// Use param NAME to update the color between red and green
	}

	if (APawn* OwningPawn = Cast<APawn>(GetOwner()))
	{
		if (OwningPawn->IsLocallyControlled())
		{
			Super::TickComponent(DeltaSeconds, TickType, ThisTickFunction);

		}
		else
		{
			if (bUsePawnControlRotation)
			{

				{
					/* Re-map uint8 to 360 degrees */
					const float PawnViewPitch = (OwningPawn->RemoteViewPitch / 255.f)*360.f;
					if (PawnViewPitch != GetComponentRotation().Pitch)
					{
						FRotator NewRotation = GetComponentRotation();
						NewRotation.Pitch = PawnViewPitch;
						SetWorldRotation(NewRotation);
					}
				}
			}

			UpdateDesiredArmLocation(bDoCollisionTest, bEnableCameraLag, bEnableCameraRotationLag, DeltaSeconds);
		}
	}
}


void USCarryObjectComponent::Pickup()
{
	/* Drop if we are already carrying an Actor */
	if (IsCarryingActor())
	{
		Drop();
		return;
	}

	if (GetOwner()->Role < ROLE_Authority)
	{
		ServerPickup();
		return;
	}

	AActor* FocusActor = GetActorInView();
	OnPickupMulticast(FocusActor);
}


void USCarryObjectComponent::ServerPickup_Implementation()
{
	Pickup();
}


bool USCarryObjectComponent::ServerPickup_Validate()
{
	return true;
}


void USCarryObjectComponent::Drop()
{
	if (GetOwner()->Role < ROLE_Authority)
	{
		ServerDrop();
	}

	OnDropMulticast();
}


AActor* USCarryObjectComponent::GetActorInView()
{
	APawn* PawnOwner = Cast<APawn>(GetOwner());
	AController* Controller = PawnOwner->Controller;
	if (Controller == nullptr)
	{
		return nullptr;
	}

	FVector CamLoc;
	FRotator CamRot;
	Controller->GetPlayerViewPoint(CamLoc, CamRot);

	const FVector TraceStart = CamLoc;
	const FVector Direction = CamRot.Vector();
	const FVector TraceEnd = TraceStart + (Direction * MaxPickupDistance);

	FCollisionQueryParams TraceParams(TEXT("TraceActor"), true, PawnOwner);
	TraceParams.bTraceAsyncScene = true;
	TraceParams.bReturnPhysicalMaterial = false;
	TraceParams.bTraceComplex = false;

	FHitResult Hit(ForceInit);
	GetWorld()->LineTraceSingle(Hit, TraceStart, TraceEnd, ECC_Visibility, TraceParams);

	//DrawDebugLine(GetWorld(), TraceStart, TraceEnd, FColor::Red, false, 1.0f);

	return Hit.GetActor();
}


AActor* USCarryObjectComponent::GetCarriedActor()
{
	USceneComponent* ChildComp = GetChildComponent(0);
	if (ChildComp)
	{
		return ChildComp->GetOwner();
	}

	return nullptr;
}


UStaticMeshComponent* USCarryObjectComponent::GetCarriedMeshComp()
{
	USceneComponent* ChildComp = GetChildComponent(0);
	if (ChildComp)
	{
		AActor* OwningActor = ChildComp->GetOwner();
		if (OwningActor)
		{
			return Cast<UStaticMeshComponent>(OwningActor->GetComponentByClass(UStaticMeshComponent::StaticClass()));
		}
	}

	return nullptr;
}


void USCarryObjectComponent::Throw()
{
	if (!IsCarryingActor())
		return;

	if (GetOwner()->Role < ROLE_Authority)
	{
		/* Detach and re-enable collision */
		Drop();

		ServerThrow();
	}

	UStaticMeshComponent* MeshComp = GetCarriedMeshComp();
	if (MeshComp)
	{
		Drop();

		APawn* PawnOwner = Cast<APawn>(GetOwner());
		AController* Controller = PawnOwner->Controller;
		if (Controller == nullptr)
		{
			return;
		}

		FVector CamLoc;
		FRotator CamRot;
		Controller->GetPlayerViewPoint(CamLoc, CamRot);

		/* Apply physics impulse, ignores mass */
		MeshComp->AddImpulse(CamRot.Vector() * 1000, NAME_None, true);
	}
}


bool USCarryObjectComponent::IsCarryingActor()
{
	return GetChildComponent(0) != nullptr;
}


void USCarryObjectComponent::Rotate(float Direction)
{
	AActor* CarriedActor = GetCarriedActor();
	if (CarriedActor)
	{
		CarriedActor->AddActorWorldRotation(FRotator(Direction * RotateSpeed, 0, 0));
	}
}


void USCarryObjectComponent::OnPickupMulticast_Implementation(AActor* FocusActor)
{
// 	if (GEngine)
// 	{
// 		if (FocusActor == nullptr)
// 		{
// 			GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Red, "No object traced");
// 		}
// 		else
// 		{
// 			GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Red, "Object traced");
// 		}
// 	}

	if (FocusActor && FocusActor->IsRootComponentMovable())
	{
		/* Find the static mesh (if any) to disable physics simulation while carried
		Filter by objects that are physically simulated and can therefor be picked up */
		UStaticMeshComponent* MeshComp = Cast<UStaticMeshComponent>(FocusActor->GetComponentByClass(UStaticMeshComponent::StaticClass()));
		if (MeshComp && MeshComp->IsSimulatingPhysics())
		{
			MeshComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
			MeshComp->SetSimulatePhysics(false);
		}

		FocusActor->AttachRootComponentTo(this, NAME_None, EAttachLocation::KeepWorldPosition);
	}
}


void USCarryObjectComponent::OnDropMulticast_Implementation()
{
	AActor* CarriedActor = GetCarriedActor();
	if (CarriedActor)
	{
		/* Find the static mesh (if any) to re-enable physics simulation */
		UStaticMeshComponent* MeshComp = Cast<UStaticMeshComponent>(CarriedActor->GetComponentByClass(UStaticMeshComponent::StaticClass()));
		if (MeshComp)
		{
			MeshComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
			MeshComp->SetSimulatePhysics(true);
		}

		CarriedActor->GetRootComponent()->DetachFromParent(true);
	}
}


void USCarryObjectComponent::ServerDrop_Implementation()
{
	Drop();
}


bool USCarryObjectComponent::ServerDrop_Validate()
{
	return true;
}


void USCarryObjectComponent::ServerThrow_Implementation()
{
	Throw();
}


bool USCarryObjectComponent::ServerThrow_Validate()
{
	return true;
}