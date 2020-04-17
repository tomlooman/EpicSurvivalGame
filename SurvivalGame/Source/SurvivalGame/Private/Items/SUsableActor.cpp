// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.


#include "Items/SUsableActor.h"
#include "Components/StaticMeshComponent.h"


ASUsableActor::ASUsableActor()
{
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = MeshComp;
} 


void ASUsableActor::OnUsed(APawn* InstigatorPawn)
{
	// Nothing to do here...
}


void ASUsableActor::OnBeginFocus()
{
	// Used by custom PostProcess to render outlines
	MeshComp->SetRenderCustomDepth(true);
}


void ASUsableActor::OnEndFocus()
{
	// Used by custom PostProcess to render outlines
	MeshComp->SetRenderCustomDepth(false);
}

