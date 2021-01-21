// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "Mutators/SMutator.h"
#include "SGameMode.generated.h"

class ASPlayerState;
class APlayerStart;

/**
 * 
 */
UCLASS()
class SURVIVALGAME_API ASGameMode : public AGameMode
{
	GENERATED_BODY()

protected:

	ASGameMode();

	virtual void PreInitializeComponents() override;

	virtual void InitGameState();

	virtual void DefaultTimer();
	
	virtual void StartMatch();

	virtual void OnNightEnded();

	virtual void SpawnDefaultInventory(APawn* PlayerPawn);
	
	/**
	* Make sure pawn properties are back to default
	* Also a good place to modify them on spawn
	*/
	virtual void SetPlayerDefaults(APawn* PlayerPawn) override;

	/* Handle for efficient management of DefaultTimer timer */
	FTimerHandle TimerHandle_DefaultTimer;

	/* Can we deal damage to players in the same team */
	UPROPERTY(EditDefaultsOnly, Category = "Rules")
	bool bAllowFriendlyFireDamage;

	/* Allow zombie spawns to be disabled (for debugging) */
	UPROPERTY(EditDefaultsOnly, Category = "Debug")
	bool bSpawnZombiesAtNight;

	/* Limit the amount of zombies to have at one point in the world (includes players) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug")
	int32 MaxPawnsInZone;

	float BotSpawnInterval;

	/* Called once on every new player that enters the gamemode */
	virtual FString InitNewPlayer(class APlayerController* NewPlayerController, const FUniqueNetIdRepl& UniqueId, const FString& Options, const FString& Portal = TEXT("")) override;

	/* The teamnumber assigned to Players */
	int32 PlayerTeamNum;

	/* Keep reference to the night state of the previous frame */
	bool LastIsNight;

	/* The start time for the gamemode */
	int32 TimeOfDayStart;

	/* The enemy pawn class */
	UPROPERTY(EditDefaultsOnly, Category = "AI")
	TSubclassOf<class APawn> BotPawnClass;

	/* Handle for nightly bot spawning */
	FTimerHandle TimerHandle_BotSpawns;

	/* Handles bot spawning (during nighttime) */
	void SpawnBotHandler();

	/************************************************************************/
	/* Player Spawning                                                      */
	/************************************************************************/

	/* Don't allow spectating of bots */
	virtual bool CanSpectate_Implementation(APlayerController* Viewer, APlayerState* ViewTarget) override;

	virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;

	/* Always pick a random location */
	virtual bool ShouldSpawnAtStartSpot(AController* Player) override;

	virtual bool IsSpawnpointAllowed(APlayerStart* SpawnPoint, AController* Controller);

	virtual bool IsSpawnpointPreferred(APlayerStart* SpawnPoint, AController* Controller);

	/** returns default pawn class for given controller */
	virtual UClass* GetDefaultPawnClassForController_Implementation(AController* InController) override;

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

	/* (Exec only valid when testing in Singleplayer) */
	UFUNCTION(BlueprintCallable, Exec, Category = "GameMode")
	void SpawnNewBot();

	/* Blueprint hook to find a good spawn location for BOTS (Eg. via EQS queries) */
	UFUNCTION(BlueprintImplementableEvent, Category = "GameMode")
	bool FindBotSpawnTransform(FTransform& Transform);

	/* Set all bots back to idle mode */
	void PassifyAllBots();

	/* Set all bots to active patrolling state */
	void WakeAllBots();

public:

	/* Primary sun of the level. Assigned in Blueprint during BeginPlay (BlueprintReadWrite is required as tag instead of EditDefaultsOnly) */
	UPROPERTY(BlueprintReadWrite, Category = "DayNight")
	class ADirectionalLight* PrimarySunLight;

	/* The default weapons to spawn with */
	UPROPERTY(EditDefaultsOnly, Category = "Player")
	TArray<TSubclassOf<class ASWeapon>> DefaultInventoryClasses;

	/************************************************************************/
	/* Modding & Mutators                                                   */
	/************************************************************************/

protected:

	/* Mutators to create when game starts */
 	UPROPERTY(EditAnywhere, Category = "Mutators")
 	TArray<TSubclassOf<class ASMutator>> MutatorClasses;

	/* First mutator in the execution chain */
	ASMutator* BaseMutator;

	void AddMutator(TSubclassOf<ASMutator> MutClass);

	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;

	/** From UT Source: Used to modify, remove, and replace Actors. Return false to destroy the passed in Actor. Default implementation queries mutators.
	* note that certain critical Actors such as PlayerControllers can't be destroyed, but we'll still call this code path to allow mutators
	* to change properties on them
	*/
	UFUNCTION(BlueprintNativeEvent)
	bool CheckRelevance(AActor* Other);

	/* Note: Functions flagged with BlueprintNativeEvent like above require _Implementation for a C++ implementation */
	virtual bool CheckRelevance_Implementation(AActor* Other);
};
