// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "SurvivalGame.h"
#include "SZombieAIController.h"
#include "SZombieCharacter.h"

/* AI Specific includes */
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "Perception/AIPerceptionComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"


ASZombieAIController::ASZombieAIController(const class FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	BehaviorComp = ObjectInitializer.CreateDefaultSubobject<UBehaviorTreeComponent>(this, TEXT("BehaviorComp"));
	BlackboardComp = ObjectInitializer.CreateDefaultSubobject<UBlackboardComponent>(this, TEXT("BlackboardComp"));

	/* Setup sight sense config */
	UAISenseConfig_Sight* SightConfig = ObjectInitializer.CreateDefaultSubobject<UAISenseConfig_Sight>(this, TEXT("Sight Config"));
	SightConfig->SightRadius = 2000;
	SightConfig->LoseSightRadius = 2500;
	SightConfig->PeripheralVisionAngleDegrees = 60.0f;
	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightConfig->SetMaxAge(2.5f);

	/* Setup hearing sense config */
	UAISenseConfig_Hearing* HearingConfig = ObjectInitializer.CreateDefaultSubobject<UAISenseConfig_Hearing>(this, TEXT("Hearing Config"));
	HearingConfig->HearingRange = 600.0f;
	HearingConfig->LoSHearingRange = 1200.0f;
	HearingConfig->DetectionByAffiliation.bDetectEnemies = true;
	HearingConfig->DetectionByAffiliation.bDetectFriendlies = true;
	HearingConfig->SetMaxAge(2.5f);

	/* Configure AI perception */
	UAIPerceptionComponent* AIPerceptionComp = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerceptionComp"));
	AIPerceptionComp->OnTargetPerceptionUpdated.AddDynamic(this, &ASZombieAIController::OnTargetPerceptionUpdated);
	AIPerceptionComp->ConfigureSense(*SightConfig);
	AIPerceptionComp->SetDominantSense(SightConfig->GetSenseImplementation());
	AIPerceptionComp->ConfigureSense(*HearingConfig);
	SetPerceptionComponent(*AIPerceptionComp);

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


void ASZombieAIController::OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	ASBaseCharacter* Target = Cast<ASBaseCharacter>(Actor);

	if (Target && (!Target->IsAlive() || !Stimulus.IsActive()))
	{
		Target = nullptr;
	}

	SetTargetEnemy(Target);
	ASZombieCharacter* ZombieBot = Cast<ASZombieCharacter>(GetPawn());
	if (ZombieBot)
	{
		ZombieBot->SetSensedTarget(Target ? true : false);
	}
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


ETeamAttitude::Type ASZombieAIController::GetTeamAttitudeTowards(const AActor & Other) const
{
	return Cast<ASCharacter>(&Other) ? ETeamAttitude::Type::Hostile : ETeamAttitude::Type::Friendly;
}
