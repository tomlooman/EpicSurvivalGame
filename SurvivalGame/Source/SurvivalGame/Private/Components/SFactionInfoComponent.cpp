// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "SurvivalGame.h"
#include "SFactionInfoComponent.h"



USFactionInfoComponent::USFactionInfoComponent()
{
	bWantsInitializeComponent = true;	
	bReplicates = true;
}


void USFactionInfoComponent::InitializeComponent()
{
	Super::InitializeComponent();
}


int32 USFactionInfoComponent::GetTeamNumber()
{
	return TeamNumber;
}


void USFactionInfoComponent::SetTeamNumber(int32 NewTeamNumber)
{
	TeamNumber = NewTeamNumber;
}


void USFactionInfoComponent::GetLifetimeReplicatedProps(TArray< class FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(USFactionInfoComponent, TeamNumber);
}
