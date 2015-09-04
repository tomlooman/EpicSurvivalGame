// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "AIController.h"
#include "SCharacter.h"
#include "SBotWaypoint.h"
#include "SZombieAIController.generated.h"

/**
 * 
 */
UCLASS()
class SURVIVALGAME_API ASZombieAIController : public AAIController
{
	GENERATED_BODY()

	ASZombieAIController(const class FObjectInitializer& ObjectInitializer);

	/* Called whenever the controller possesses a character bot */
	virtual void Possess(class APawn* InPawn) override;

	virtual void UnPossess() override;

	UBehaviorTreeComponent* BehaviorComp;

	UBlackboardComponent* BlackboardComp;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	FName TargetEnemyKeyName;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	FName PatrolLocationKeyName;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	FName CurrentWaypointKeyName;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	FName BotTypeKeyName;

public:

	ASBotWaypoint* GetWaypoint();

	ASBaseCharacter* GetTargetEnemy();

	void SetWaypoint(ASBotWaypoint* NewWaypoint);

	void SetTargetEnemy(APawn* NewTarget);

	void SetBlackboardBotType(EBotBehaviorType NewType);

	/** Returns BehaviorComp subobject **/
	FORCEINLINE UBehaviorTreeComponent* GetBehaviorComp() const { return BehaviorComp; }

	FORCEINLINE UBlackboardComponent* GetBlackboardComp() const { return BlackboardComp; }
};
