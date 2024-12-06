// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Monster/BaseMonster.h"
#include "ADC.generated.h"

/**
 * 
 */
UCLASS()
class TRAPPERPROJECT_API AADC : public ABaseMonster
{
	GENERATED_BODY()

public:
	AADC();

protected:
	virtual void BeginPlay() override;

public:
	//Attack
	void SpawnMagicBall();

	//Animation Montage
	void AttackADCMontage();

	//network
	//Animation Montage RPC
	UFUNCTION(Server, Reliable)
	void ServerRPCAttack();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPCAttack();

	///Upgrade
	void ChangeADCDamage();

	void ADCDeadByTrap();

public:
	//Montage
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
	TObjectPtr<class UAnimMontage> AttackkActionMontage1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
	TObjectPtr<class UAnimMontage> AttackkActionMontage2;

	//Wand
	UPROPERTY(EditAnywhere, Category = "Wand")
	TSubclassOf<class AWand> WandClass;

	UPROPERTY(EditAnywhere, Category = "Wand")
	TObjectPtr<class AWand> Wand;

	float AttackDamage = 50.f;


	///-------------------------DeadbodyParts------------------------
	//Horn
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Deadbody")
	TObjectPtr<class USceneComponent> DeadbodyHornSpawnPoint;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Deadbody")
	TSubclassOf<class ADeadbodyParts> DeadbodyHorn;
	
	//Head
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Deadbody")
	TObjectPtr<class USceneComponent> DeadbodyHeadSpawnPoint;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Deadbody")
	TSubclassOf<class ADeadbodyParts> DeadbodyHead;

	//Chest
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Deadbody")
	TObjectPtr<class USceneComponent> DeadbodyChestSpawnPoint;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Deadbody")
	TSubclassOf<class ADeadbodyParts> DeadbodyChest;

	//LeftHand
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Deadbody")
	TObjectPtr<class USceneComponent> DeadbodyLeftHandSpawnPoint;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Deadbody")
	TSubclassOf<class ADeadbodyParts> DeadbodyLeftHand;

	//RightHand
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Deadbody")
	TObjectPtr<class USceneComponent> DeadbodyRightHandSpawnPoint;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Deadbody")
	TSubclassOf<class ADeadbodyParts> DeadbodyRightHand;

	//Leg
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Deadbody")
	TObjectPtr<class USceneComponent> DeadbodyLegSpawnPoint;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Deadbody")
	TSubclassOf<class ADeadbodyParts> DeadbodyLeg;

	//RightLeg
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Deadbody")
	TObjectPtr<class USceneComponent> DeadbodyRightLegSpawnPoint;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Deadbody")
	TSubclassOf<class ADeadbodyParts> DeadbodyRightLeg;

	//Foot
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Deadbody")
	TObjectPtr<class USceneComponent> DeadbodyFootSpawnPoint;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Deadbody")
	TSubclassOf<class ADeadbodyParts> DeadbodyFoot;

};
