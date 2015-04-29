// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "AIController.h"
#include "SCharacter.h"
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

	UBehaviorTreeComponent* BehaviorComp;

	UBlackboardComponent* BlackboardComp;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	FName TargetToFollowKeyName;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	FName NoiseLocationKeyName;

public:

	void SetNoiseLocation(FVector NoiseHeardLocation);

	FVector GetNoiseLocaton();

	void SetTargetPlayer(APawn* Pawn);

	/* Returns the current TargetToFollow from the blackboard */
	class ASCharacter* GetTargetPlayer();

	/** Returns BehaviorComp subobject **/
	FORCEINLINE UBehaviorTreeComponent* GetBehaviorComp() const { return BehaviorComp; }

	FORCEINLINE UBlackboardComponent* GetBlackboardComp() const { return BlackboardComp; }
};
