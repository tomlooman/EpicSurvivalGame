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
	PatrolLocationKeyName = "PatrolLocation";
	CurrentWaypointKeyName = "CurrentWaypoint";
	BotTypeKeyName = "BotType";
	TargetEnemyKeyName = "TargetEnemy";

	/* Initializes PlayerState so we can assign a team index to AI */
	bWantsPlayerState = true;
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

			/* Make sure the Blackboard has the type of bot we possessed */
			SetBlackboardBotType(ZombieBot->BotType);
		}

		BehaviorComp->StartTree(*ZombieBot->BehaviorTree);
	}
}


void ASZombieAIController::UnPossess()
{
	Super::UnPossess();

	/* Stop any behavior running as we no longer have a pawn to control */
	BehaviorComp->StopTree();
}


void ASZombieAIController::SetWaypoint(ASBotWaypoint* NewWaypoint)
{
	if (BlackboardComp)
	{
		BlackboardComp->SetValueAsObject(CurrentWaypointKeyName, NewWaypoint);
	}
}


void ASZombieAIController::SetTargetEnemy(APawn* NewTarget)
{
	if (BlackboardComp)
	{
		BlackboardComp->SetValueAsObject(TargetEnemyKeyName, NewTarget);
	}
}


ASBotWaypoint* ASZombieAIController::GetWaypoint()
{
	if (BlackboardComp)
	{
		return Cast<ASBotWaypoint>(BlackboardComp->GetValueAsObject(CurrentWaypointKeyName));
	}

	return nullptr;
}


ASBaseCharacter* ASZombieAIController::GetTargetEnemy()
{
	if (BlackboardComp)
	{
		return Cast<ASBaseCharacter>(BlackboardComp->GetValueAsObject(TargetEnemyKeyName));
	}

	return nullptr;
}


void ASZombieAIController::SetBlackboardBotType(EBotBehaviorType NewType)
{
	if (BlackboardComp)
	{
		BlackboardComp->SetValueAsEnum(BotTypeKeyName, (uint8)NewType);
	}
}
