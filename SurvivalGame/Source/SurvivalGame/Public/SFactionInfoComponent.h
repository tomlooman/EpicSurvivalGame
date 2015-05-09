// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Components/ActorComponent.h"
#include "SFactionInfoComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SURVIVALGAME_API USFactionInfoComponent : public UActorComponent
{
	GENERATED_BODY()
	
	UPROPERTY(Transient, Replicated)
	int32 TeamNumber;

	UFUNCTION(BlueprintCallable, Category = "Faction")
	int32 GetTeamNumber();

	void SetTeamNumber(int32 NewTeamNumber);

public:	
	// Sets default values for this component's properties
	USFactionInfoComponent();

	// Called when the game starts
	virtual void InitializeComponent() override;
	
};
