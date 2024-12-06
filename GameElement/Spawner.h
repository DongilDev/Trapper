// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Spawner.generated.h"

UCLASS()
class TRAPPERPROJECT_API ASpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASpawner();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	void SpawnMonsters(int32 Skeleton, int32 Mymmy, int32 Zombie, int32 Debuffer, int32 Boss);

public:
	void SpawnSkeleton	(int32 Skeleton);
	void SpawnMymmy		(int32 Mymmy);
	void SpawnZombie	(int32 Zombie);
	void SpawnDebuffer	(int32 Debuffer);
	void SpawnBoss		(int32 Boss);

public:
	//Components
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<class UStaticMeshComponent> Spawner;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<class USceneComponent> SpawnPoint1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<class USceneComponent> SpawnPoint2;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<class USceneComponent> SpawnPoint3;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<class USceneComponent> SpawnPoint4;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<class USceneComponent> SpawnPoint5;


public:
	UFUNCTION(Server, Reliable)
	void ServerRPCTeleportTo(FVector SpawnLocation);
	
	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPCTeleportTo(FVector SpawnLocation);

	UFUNCTION(Server, Reliable)
	void ServerRPCTeleportToADC(FVector SpawnLocation);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPCTeleportToADC(FVector SpawnLocation);

	UFUNCTION(Server, Reliable)
	void ServerRPCTeleportToDetect(FVector SpawnLocation);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPCTeleportToDetect(FVector SpawnLocation);

	UFUNCTION(Server, Reliable)
	void ServerRPCTeleportToDebuffer(FVector SpawnLocation);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPCTeleportToDebuffer(FVector SpawnLocation);

	///Upgrade
	void UpadateSpeed();
	void UpdateHealth();
	void UpdateDamage();

public:
	//GameState
	class ATrapperGameState* GameState;

	//SaveMonster
	TArray<TObjectPtr<class AADC>> ADCList;
	TArray<TObjectPtr<class ACDC>> CDCList;
	TArray<TObjectPtr<class ADetectedMonster>> DetectedList;
	TArray<TObjectPtr<class ADebuffer>> DebufferList;
	TArray<TObjectPtr<class ABoss_Phase1>> BossPhase1List;
	TArray<TObjectPtr<class ABoss_Phase2>> BossPhase2List;

	//Monster Class
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster")
	TSubclassOf<class ACDC> CDCMonsterClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster")
	TSubclassOf<class AADC> ADCMonsterClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster")
	TSubclassOf<class ADetectedMonster> DetectedMonsterClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster")
	TSubclassOf<class ADebuffer> DebufferMonsterClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster")
	TSubclassOf<class ABoss_Phase1> Boss_Phase1Class;

	//SpawnMonster
	//TArray<TObjectPtr<class ABaseMonster>> SpawnMonsterList;
	TSet<TObjectPtr<class ABaseMonster>> SpawnMonsterList;

	//Spawn Location
	FVector SpawnLocation1;
	FVector SpawnLocation2;
	FVector SpawnLocation3;
	FVector SpawnLocation4;
	FVector SpawnLocation5;

	int32 SpawnPosition = 0;


};
