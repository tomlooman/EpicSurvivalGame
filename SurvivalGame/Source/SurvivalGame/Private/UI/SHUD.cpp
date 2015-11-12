// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "SurvivalGame.h"
#include "SHUD.h"
#include "SCharacter.h"
#include "SUsableActor.h"
#include "SPlayerController.h"


ASHUD::ASHUD(const class FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	/* You can use the FObjectFinder in C++ to reference content directly in code. Although it's advisable to avoid this and instead assign content through Blueprint child classes. */
	static ConstructorHelpers::FObjectFinder<UTexture2D> HUDCenterDotObj(TEXT("/Game/UI/HUD/T_CenterDot_M.T_CenterDot_M"));
	CenterDotIcon = UCanvas::MakeIcon(HUDCenterDotObj.Object);
}


void ASHUD::DrawHUD()
{
	Super::DrawHUD();

	DrawCenterDot();
}


void ASHUD::DrawCenterDot()
{
	float CenterX = Canvas->ClipX / 2;
	float CenterY = Canvas->ClipY / 2;
	float CenterDotScale = 0.07f;

	ASCharacter* Pawn = Cast<ASCharacter>(GetOwningPawn());
	if (Pawn && Pawn->IsAlive())
	{
		// Boost size when hovering over a usable object.
		ASUsableActor* Usable = Pawn->GetUsableInView();
		if (Usable)
		{
			CenterDotScale *= 1.5f;
		}

		Canvas->SetDrawColor(255, 255, 255, 255);
		Canvas->DrawIcon(CenterDotIcon,
			CenterX - CenterDotIcon.UL*CenterDotScale / 2.0f,
			CenterY - CenterDotIcon.VL*CenterDotScale / 2.0f, CenterDotScale);
	}
}



void ASHUD::OnStateChanged_Implementation(EHUDState NewState)
{
	CurrentState = NewState;
}


EHUDState ASHUD::GetCurrentState()
{
	return CurrentState;
}
