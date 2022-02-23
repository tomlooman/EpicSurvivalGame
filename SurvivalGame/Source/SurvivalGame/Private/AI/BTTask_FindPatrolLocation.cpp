// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.


#include "AI/BTTask_FindPatrolLocation.h"
#include "AI/SBotWaypoint.h"
#include "AI/SZombieAIController.h"

/* AI Module includes */
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
/* This contains includes all key types like UBlackboardKeyType_Vector used below. */
#include "BehaviorTree/Blackboard/BlackboardKeyAllTypes.h"
#include "NavigationSystem.h"



EBTNodeResult::Type UBTTask_FindPatrolLocation::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	ASZombieAIController* MyController = Cast<ASZombieAIController>(OwnerComp.GetAIOwner());
	if (MyController == nullptr)
	{
		return EBTNodeResult::Failed;
	}

	AActor* MyWaypoint = MyController->GetWaypoint(); // we make an AActor called MyWayPoint and assign it the value of whatever zombie controller function GetWaypoint returns
	if (MyWaypoint)
	{
		/* Find a position that is close to the waypoint. We add a small random to this position to give build predictable patrol patterns  */
		const float SearchRadius = 200.0f;
		const FVector SearchOrigin = MyWaypoint->GetActorLocation();

		FNavLocation ResultLocation;
		UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetNavigationSystem(MyController);
		if (NavSystem && NavSystem->GetRandomPointInNavigableRadius(SearchOrigin, SearchRadius, ResultLocation))
		{
			/* The selected key should be "PatrolLocation" in the BehaviorTree setup */
			OwnerComp.GetBlackboardComponent()->SetValue<UBlackboardKeyType_Vector>(BlackboardKey.GetSelectedKeyID(), ResultLocation.Location);
			return EBTNodeResult::Succeeded;
		}
	}

	return EBTNodeResult::Failed;
}
