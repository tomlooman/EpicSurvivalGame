// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "SurvivalGame.h"
#include "BTTask_FindPositionNearPlayer.h"
#include "SZombieAIController.h"
#include "SCharacter.h"

/* AI Module includes */
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
/* This contains includes all key types like UBlackboardKeyType_Vector used below. */
#include "BehaviorTree/Blackboard/BlackboardKeyAllTypes.h"


EBTNodeResult::Type UBTTask_FindPositionNearPlayer::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	ASZombieAIController* MyController = Cast<ASZombieAIController>(OwnerComp.GetOwner());
	if (MyController == nullptr)
	{
		return EBTNodeResult::Failed;
	}

	ASCharacter* SensedPlayer = Cast<ASCharacter>(MyController->GetTargetPlayer());
	if (SensedPlayer)
	{
		/* Ignore death players */
		if (!SensedPlayer->IsAlive())
		{
			return EBTNodeResult::Failed;
		}

		/* Run up directly to player */
		FVector Location = SensedPlayer->GetActorLocation();
		OwnerComp.GetBlackboardComponent()->SetValue<UBlackboardKeyType_Vector>(BlackboardKey.GetSelectedKeyID(), Location);

		return EBTNodeResult::Succeeded;
	}

	return EBTNodeResult::Failed;
}

