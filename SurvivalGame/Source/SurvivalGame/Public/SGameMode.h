// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GameFramework/GameMode.h"
#include "SGameMode.generated.h"

/**
 * 
 */
UCLASS()
class SURVIVALGAME_API ASGameMode : public AGameMode
{
	GENERATED_BODY()

	ASGameMode(const FObjectInitializer& ObjectInitializer);

	virtual void DefaultTimer() override;

	/* End the match when all players are dead */
	void CheckMatchEnd();

	/* End the match, with a delay before returning to the main menu */
	void FinishMatch();

	/* Can we deal damage to players in the same team */
	bool bAllowFriendlyFireDamage;

	/* Called once on every new player that enters the gamemode */
	virtual FString InitNewPlayer(class APlayerController* NewPlayerController, const TSharedPtr<FUniqueNetId>& UniqueId, const FString& Options, const FString& Portal /* = TEXT("") */);

	/* The teamnumber assigned to Players */
	int32 PlayerTeamNum;

	/* Keep reference to the night state of the previous frame */
	bool bWasNight;

	/* The enemy pawn class */
	UPROPERTY(EditDefaultsOnly, Category = "AI")
	TSubclassOf<class APawn> BotPawnClass;

	/************************************************************************/
	/* Player Spawning                                                      */
	/************************************************************************/

protected:

	virtual AActor* ChoosePlayerStart(AController* Player) override;

	/* Always pick a random location */
	virtual bool ShouldSpawnAtStartSpot(AController* Player) override;

	virtual bool IsSpawnpointAllowed(APlayerStart* SpawnPoint, AController* Controller);

	virtual bool IsSpawnpointPreferred(APlayerStart* SpawnPoint, AController* Controller);

	/** returns default pawn class for given controller */
	virtual UClass* GetDefaultPawnClassForController(AController* InController) override;

	/************************************************************************/
	/* Damage & Killing                                                     */
	/************************************************************************/

public: 

	virtual void Killed(AController* Killer, AController* VictimPlayer, APawn* VictimPawn, const UDamageType* DamageType);

	/* Can the player deal damage according to gamemode rules (eg. friendly-fire disabled) */
	virtual bool CanDealDamage(class ASPlayerState* DamageCauser, class ASPlayerState* DamagedPlayer) const;

	virtual float ModifyDamage(float Damage, AActor* DamagedActor, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) const;

	/************************************************************************/
	/* Bots                                                                 */
	/************************************************************************/

protected:

	void SpawnNewBot();

public:

	/* Primary sun of the level. Assigned in Blueprint during BeginPlay (BlueprintReadWrite is required as tag instead of EditDefaultsOnly) */
	UPROPERTY(BlueprintReadWrite, Category = "DayNight")
	ADirectionalLight* PrimarySunLight;

};
