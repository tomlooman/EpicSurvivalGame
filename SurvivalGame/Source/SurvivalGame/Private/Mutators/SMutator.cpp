// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.


#include "Mutators/SMutator.h"





bool ASMutator::CheckRelevance_Implementation(AActor* Other)
{
	if (NextMutator)
	{
		return NextMutator->CheckRelevance(Other);
	}

	return true;
}


void ASMutator::InitGame_Implementation(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	if (NextMutator)
	{
		NextMutator->InitGame(MapName, Options, ErrorMessage);
	}
}
