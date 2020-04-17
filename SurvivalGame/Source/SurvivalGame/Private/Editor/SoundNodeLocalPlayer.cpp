// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.


#include "Editor/SoundNodeLocalPlayer.h"
#include "SoundDefinitions.h"


void USoundNodeLocalPlayer::ParseNodes(FAudioDevice* AudioDevice, const UPTRINT NodeWaveInstanceHash, FActiveSound& ActiveSound, const FSoundParseParameters& ParseParams, TArray<FWaveInstance*>& WaveInstances)
{
	// The accesses to the Pawn will be unsafe once we thread audio, deal with this at that point
	check(IsInGameThread());

	AActor* SoundOwner = ActiveSound.GetAudioComponentID() ? UAudioComponent::GetAudioComponentFromID(ActiveSound.GetAudioComponentID())->GetOwner() : nullptr;
	APlayerController* PCOwner = Cast<APlayerController>(SoundOwner);
	APawn* PawnOwner = (PCOwner ? PCOwner->GetPawn() : Cast<APawn>(SoundOwner));

	const bool bLocallyControlled = PawnOwner && PawnOwner->IsLocallyControlled() && Cast<APlayerController>(PawnOwner->Controller);
	const int32 PlayIndex = bLocallyControlled ? 0 : 1;

	if (PlayIndex < ChildNodes.Num() && ChildNodes[PlayIndex])
	{
		ChildNodes[PlayIndex]->ParseNodes(AudioDevice, GetNodeWaveInstanceHash(NodeWaveInstanceHash, ChildNodes[PlayIndex], PlayIndex), ActiveSound, ParseParams, WaveInstances);
	}
}


void USoundNodeLocalPlayer::CreateStartingConnectors()
{
	InsertChildNode(ChildNodes.Num());
	InsertChildNode(ChildNodes.Num());
}


#if WITH_EDITOR
FText USoundNodeLocalPlayer::GetInputPinName(int32 PinIndex) const
{
	return (PinIndex == 0) ? NSLOCTEXT("Survival", "InputPinName", "Local") : NSLOCTEXT("Survival", "InputPinName", "Remote");
}
#endif


int32 USoundNodeLocalPlayer::GetMaxChildNodes() const
{
	return 2;
}


int32 USoundNodeLocalPlayer::GetMinChildNodes() const
{
	return 2;
}


