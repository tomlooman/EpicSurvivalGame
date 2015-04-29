// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "SurvivalGame.h"
#include "SZombieAIController.h"
#include "SZombieCharacter.h"

/* AI Specific includes */
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"


ASZombieAIController::ASZombieAIController(const class FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	BehaviorComp = ObjectInitializer.CreateDefaultSubobject<UBehaviorTreeComponent>(this, TEXT("BehaviorComp"));

	BlackboardComp = ObjectInitializer.CreateDefaultSubobject<UBlackboardComponent>(this, TEXT("BlackboardComp"));

	/* Match with the AI/ZombieBlackboard */
	TargetToFollowKeyName = "TargetToFollow";
	NoiseLocationKeyName = "NoiseLocation";
}


void ASZombieAIController::Possess(class APawn* InPawn)
{
	Super::Possess(InPawn);

	ASZombieCharacter* ZombieBot = Cast<ASZombieCharacter>(InPawn);
	if (ZombieBot)
	{
		if (ZombieBot->BehaviorTree->BlackboardAsset)
		{
			BlackboardComp->InitializeBlackboard(*ZombieBot->BehaviorTree->BlackboardAsset);
		}

		BehaviorComp->StartTree(*ZombieBot->BehaviorTree);
	}
}


ASCharacter* ASZombieAIController::GetTargetPlayer()
{
	if (BlackboardComp)
	{
		return Cast<ASCharacter>(BlackboardComp->GetValueAsObject(TargetToFollowKeyName));
	}

	return nullptr;
}

void ASZombieAIController::SetTargetPlayer(APawn* Pawn)
{
	if (BlackboardComp)
	{
		BlackboardComp->SetValueAsObject(TargetToFollowKeyName, Pawn);
		// TODO: Use AiController->SetFocus() ?
	}
}


void ASZombieAIController::SetNoiseLocation(FVector NoiseHeardLocation)
{
	if (BlackboardComp)
	{
		BlackboardComp->SetValueAsVector(NoiseLocationKeyName, NoiseHeardLocation);
	}
}


FVector ASZombieAIController::GetNoiseLocaton()
{
	if (BlackboardComp)
	{
		return BlackboardComp->GetValueAsVector(NoiseLocationKeyName);
	}

	return FVector::ZeroVector;
}
