// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GameFramework/PlayerState.h"
#include "SPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class SURVIVALGAME_API ASPlayerState : public APlayerState
{
	GENERATED_BODY()

	ASPlayerState(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(Transient, Replicated)
	int32 NumKills;

	UPROPERTY(Transient, Replicated)
	int32 NumDeaths;

	/* Team number assigned to player */
	UPROPERTY(Transient, Replicated)
	int32 TeamNumber;

	virtual void Reset() override;

public: 

	void AddKill();

	void AddDeath();

	void ScorePoints(int32 Points);

	void SetTeamNumber(int32 NewTeamNumber);

	UFUNCTION(BlueprintCallable, Category = "Teams")
	int32 GetTeamNumber() const;

	UFUNCTION(BlueprintCallable, Category = "Score")
	int32 GetKills() const;

	UFUNCTION(BlueprintCallable, Category = "Score")
	int32 GetDeaths() const;

	UFUNCTION(BlueprintCallable, Category = "Score")
	float GetScore() const;
	
};
