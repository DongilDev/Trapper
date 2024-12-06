// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Monster/BaseMonster.h"
#include "Monster/Animation/AnimationAttackInterface.h"
#include "DetectedMonster.generated.h"

/**
 * 
 */
UCLASS()
class TRAPPERPROJECT_API ADetectedMonster : public ABaseMonster, public IAnimationAttackInterface
{
	GENERATED_BODY()
	
public:
	ADetectedMonster();

protected:
	virtual void BeginPlay() override;

protected:
	virtual void AttackHitCheck() override;

public:
	void DetectedDeadByTrap();

public:
	//Animation Montage
	void AttackDetectedMontage();

	//network
	//Animation Montage RPC
	UFUNCTION(Server, Reliable)
	void ServerRPCAttack();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPCAttack();

	UFUNCTION(Server, Reliable)
	void ServerRPCAttackHitSoundPlay();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPCAttackHitSoundPlay();

	///Upgrade
	void ChangeDetectedDamage();

public:
	//Montage
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
	TObjectPtr<class UAnimMontage> AttackkActionMontage1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
	TObjectPtr<class UAnimMontage> AttackkActionMontage2;

	//Sound
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Sound)
	TObjectPtr<USoundBase> DetectAttackHitSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Damage)
	float AttackDamage = 50.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attack")
	TObjectPtr<class UBoxComponent> AttackTriggerBox;

	//DeadbodyParts
	//Head
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Deadbody")
	TObjectPtr<class USceneComponent> DeadbodyHeadSpawnPoint;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Deadbody")
	TSubclassOf<class ADeadbodyParts> DeadbodyHead;

	//Thorax
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Deadbody")
	TObjectPtr<class USceneComponent> DeadbodyThoraxSpawnPoint;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Deadbody")
	TSubclassOf<class ADeadbodyParts> DeadbodyThorax;

	//Leg
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Deadbody")
	TObjectPtr<class USceneComponent> DeadbodyLegSpawnPoint;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Deadbody")
	TSubclassOf<class ADeadbodyParts> DeadbodyLeg;
};
