// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTask_FindPositionNearPlayer.generated.h"

/**
 * Blackboard Task - Finds a position to a nearby player (if player is nearby & assigned)
 */
UCLASS()
class SURVIVALGAME_API UBTTask_FindPositionNearPlayer : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

};
