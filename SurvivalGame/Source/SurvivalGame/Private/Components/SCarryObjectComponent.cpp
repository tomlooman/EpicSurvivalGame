// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "SurvivalGame.h"
#include "SCarryObjectComponent.h"


USCarryObjectComponent::USCarryObjectComponent(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	MaxPickupDistance = 600;
	RotateSpeed = 10.0f;
}


void USCarryObjectComponent::TickComponent(float DeltaSeconds, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
	Super::TickComponent(DeltaSeconds, TickType, ThisTickFunction);

	if (IsCarryingActor())
	{
		// perform an overlap check of the actor vs. environment. must have 0 overlaps from Static and Dynamic and Pawn channels.

		// Use param NAME to update the color between red and green
	}
}


void USCarryObjectComponent::Pickup()
{
	/* Drop if we are already carrying an Actor */
	if (GetChildComponent(0) != nullptr)
	{
		Drop();
		return;
	}

	AActor* FocusActor = GetActorInView();
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


void USCarryObjectComponent::Drop()
{
	USceneComponent* ChildComp = GetChildComponent(0);
	if (ChildComp)
	{
		AActor* OwningActor = ChildComp->GetOwner();
		if (OwningActor)
		{
			/* Find the static mesh (if any) to re-enable physics simulation */
			UStaticMeshComponent* MeshComp = Cast<UStaticMeshComponent>(OwningActor->GetComponentByClass(UStaticMeshComponent::StaticClass()));
			if (MeshComp)
			{
				MeshComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
				MeshComp->SetSimulatePhysics(true);
			}

			OwningActor->GetRootComponent()->DetachFromParent(true);
		}
	}
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

	UStaticMeshComponent* MeshComp = GetCarriedMeshComp();
	if (MeshComp)
	{
		/* Detach and re-enable collision */
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
