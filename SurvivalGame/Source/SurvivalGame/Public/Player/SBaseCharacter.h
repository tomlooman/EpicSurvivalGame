// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CartesianPlot.h"
#include "GameFramework/Character.h"
#include "SurvivalGame/STypes.h"
#include "SBaseCharacter.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDeathDelegate, APawn*, Killer);

class USoundCue;


UCLASS(ABSTRACT)
class SURVIVALGAME_API ASBaseCharacter : public ACharacter
{
	GENERATED_BODY()

	/* Tracks noise data used by the pawn sensing component */
	UPawnNoiseEmitterComponent* NoiseEmitterComp;

public:

	// Sets default values for this character's properties
	ASBaseCharacter(const class FObjectInitializer& ObjectInitializer);

	UPROPERTY(EditDefaultsOnly, Category = "Sound")
	USoundCue* SoundTakeHit;

	UPROPERTY(EditDefaultsOnly, Category = "Sound")
	USoundCue* SoundDeath;

	/************************************************************************/
	/* Graphs                                                               */
	/************************************************************************/

	UPROPERTY(BlueprintReadWrite, Category = "Graphs")
		TArray<FName> CartesianVars;

	UPROPERTY(BlueprintReadWrite, Category = "Graphs")
		TArray<FName> TimeVars;

	UPROPERTY(BlueprintReadWrite, Category = "Graphs")
		TArray<FName> CategoryVars;

	UPROPERTY(BlueprintReadWrite, Category = "Graphs")
		FCartesianDatasource CartesianDatasource;

	UPROPERTY(BlueprintReadWrite, Category = "Graphs")
		FCartesianDatasource TimeDatasource;

	UPROPERTY(BlueprintReadWrite, Category = "Graphs")
		FCategoryDatasource CategoryDatasource;

	//UPROPERTY(BlueprintReadWrite, Category = "Graphs", Replicated)
		//UKantanSimpleCartesianDatasource* Datasource = UKantanSimpleCartesianDatasource::NewSimpleCartesianDatasource();

	UFUNCTION(BlueprintCallable, Category = "Charts", meta = (WorldContext = "WorldContextObject"))
		void CreateSeries(const UObject* WorldContextObject, bool Cartesian, bool Time, bool Category, TArray<FName> VariablesList);

	UFUNCTION(BlueprintCallable, Category = "Charts", meta = (WorldContext = "WorldContextObject"))
		void AddCartesianDatapoint(const UObject* WorldContextObject, FName SeriesID, FVector2D Point);

	UFUNCTION(BlueprintCallable, Category = "Charts", meta = (WorldContext = "WorldContextObject"))
		void AddTimeDatapoint(const UObject* WorldContextObject, FName SeriesID, float Value);

	UFUNCTION(BlueprintCallable, Category = "Charts", meta = (WorldContext = "WorldContextObject"))
		void AddCategoryDatapoint(const UObject* WorldContextObject, FName SeriesID, float Value);


	/************************************************************************/
	/* Health                                                               */
	/************************************************************************/

	UFUNCTION(BlueprintCallable, Category = "PlayerCondition")
	float GetMaxHealth() const;

	UFUNCTION(BlueprintCallable, Category = "PlayerCondition")
	float GetHealth() const;

	UFUNCTION(BlueprintCallable, Category = "PlayerCondition")
	bool IsAlive() const;

	UFUNCTION(BlueprintCallable, Category = "Movement")
	virtual bool IsSprinting() const;

	/* Client/local call to update sprint state  */
	virtual void SetSprinting(bool NewSprinting);

	float GetSprintingSpeedModifier() const;

protected:

	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float SprintingSpeedModifier;

	/* Character wants to run, checked during Tick to see if allowed */
	UPROPERTY(Transient, Replicated)
	bool bWantsToRun;

	/* Server side call to update actual sprint state */
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerSetSprinting(bool NewSprinting);

	void ServerSetSprinting_Implementation(bool NewSprinting);

	bool ServerSetSprinting_Validate(bool NewSprinting);

	/************************************************************************/
	/* Targeting                                                            */
	/************************************************************************/

	void SetTargeting(bool NewTargeting);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerSetTargeting(bool NewTargeting);

	void ServerSetTargeting_Implementation(bool NewTargeting);

	bool ServerSetTargeting_Validate(bool NewTargeting);

	UPROPERTY(Transient, Replicated)
	bool bIsTargeting;

	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	float TargetingSpeedModifier;

public:

	/* Is player aiming down sights */
	UFUNCTION(BlueprintCallable, Category = "Targeting")
	bool IsTargeting() const;

	float GetTargetingSpeedModifier() const;

	/* Retrieve Pitch/Yaw from current camera */
	UFUNCTION(BlueprintCallable, Category = "Targeting")
	FRotator GetAimOffsets() const;

	/************************************************************************/
	/* Damage & Death                                                       */
	/************************************************************************/

	UPROPERTY(BlueprintAssignable, Category = "PlayerCondition")
		FDeathDelegate DeathDelegate;

	//UFUNCTION(BlueprintImplementableEvent, Category = "PlayerCondition", meta = (DisplayName = "OnDeath"))
	//	void RecievedOnDeath();

	UPROPERTY(BlueprintReadWrite, Category = "PlayerCondition", Replicated)
		bool IsDead;
	//UFUNCTION(BlueprintCallable)
	//float TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, class AActor* DamageCauser) override;

	UFUNCTION(BlueprintCallable)
	void SetHealth(float newHealth);



protected:

	UPROPERTY(EditDefaultsOnly, Category = "PlayerCondition", Replicated)
	float Health;

	/* Take damage & handle death */
	UFUNCTION(BlueprintCallable)
	virtual float TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, class AActor* DamageCauser) override;

	virtual bool CanDie(float KillingDamage, FDamageEvent const& DamageEvent, AController* Killer, AActor* DamageCauser) const;

	UFUNCTION(BlueprintCallable)
	virtual bool Die(float KillingDamage, FDamageEvent const& DamageEvent, AController* Killer, AActor* DamageCauser);

	UFUNCTION(BlueprintCallable)
	virtual void OnDeath(float KillingDamage, FDamageEvent const& DamageEvent, APawn* PawnInstigator, AActor* DamageCauser);

	virtual void FellOutOfWorld(const class UDamageType& DmgType) override;

	void SetRagdollPhysics();

	virtual void PlayHit(float DamageTaken, struct FDamageEvent const& DamageEvent, APawn* PawnInstigator, AActor* DamageCauser, bool bKilled);

	void ReplicateHit(float DamageTaken, struct FDamageEvent const& DamageEvent, APawn* PawnInstigator, AActor* DamageCauser, bool bKilled);

	/* Holds hit data to replicate hits and death to clients */
	UPROPERTY(Transient, ReplicatedUsing = OnRep_LastTakeHitInfo)
	struct FTakeHitInfo LastTakeHitInfo;

	UFUNCTION()
	void OnRep_LastTakeHitInfo();

	bool bIsDying;
};
