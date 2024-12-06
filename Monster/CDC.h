// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Monster/BaseMonster.h"
#include "Monster/Animation/AnimationAttackInterface.h"
#include "CDC.generated.h"

/**
 * 
 */
UCLASS()
class TRAPPERPROJECT_API ACDC : public ABaseMonster, public IAnimationAttackInterface
{
	GENERATED_BODY()
	
public:
	ACDC();

protected:
	virtual void BeginPlay() override;

//Attack Hit Section
protected:
	virtual void AttackHitCheck() override;
	virtual float TakeDamage(float DamageAmout, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

public:
	//Animation Montage
	void AttackCDCMontage();

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

	//Upgrade Monster Damage
	void ChangeCDCDamage();

	void CDCDeadByTrap();

public:
	//Montage
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
	TObjectPtr<class UAnimMontage> AttackkActionMontage1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
	TObjectPtr<class UAnimMontage> AttackkActionMontage2;

	//Sound
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Sound)
	TObjectPtr<USoundBase> CDCAttackHitSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Damage)
	float AttackDamage = 50.f;

	//Sword
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sword")
	TSubclassOf<class ASword> SwordClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sword")
	TObjectPtr<class ASword> Sword;

	//DeadbodyParts
	//Head
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Deadbody")
	TObjectPtr<class USceneComponent> DeadbodyHeadSpawnPoint;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Deadbody")
	TSubclassOf<class ADeadbodyParts> DeadbodyHead;

	//Spine
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Deadbody")
	TObjectPtr<class USceneComponent> DeadbodySpineSpawnPoint;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Deadbody")
	TSubclassOf<class ADeadbodyParts> DeadbodySpine;

	//Lung
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Deadbody")
	TObjectPtr<class USceneComponent> DeadbodyLungSpawnPoint;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Deadbody")
	TSubclassOf<class ADeadbodyParts> DeadbodyLung;

	//Hip
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Deadbody")
	TObjectPtr<class USceneComponent> DeadbodyHipSpawnPoint;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Deadbody")
	TSubclassOf<class ADeadbodyParts> DeadbodyHip;

	//RightArm
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Deadbody")
	TObjectPtr<class USceneComponent> DeadbodyRightArmSpawnPoint;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Deadbody")
	TSubclassOf<class ADeadbodyParts> DeadbodyRightArm;

	//LeftArm
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Deadbody")
	TObjectPtr<class USceneComponent> DeadbodyLeftArmSpawnPoint;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Deadbody")
	TSubclassOf<class ADeadbodyParts> DeadbodyLeftArm;

	//RightHand
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Deadbody")
	TObjectPtr<class USceneComponent> DeadbodyRightHandSpawnPoint;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Deadbody")
	TSubclassOf<class ADeadbodyParts> DeadbodyRightHand;

	//LeftHand
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Deadbody")
	TObjectPtr<class USceneComponent> DeadbodyLeftHandSpawnPoint;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Deadbody")
	TSubclassOf<class ADeadbodyParts> DeadbodyLeftHand;

	//LeftThigh
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Deadbody")
	TObjectPtr<class USceneComponent> DeadbodyLeftThighSpawnPoint;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Deadbody")
	TSubclassOf<class ADeadbodyParts> DeadbodyLeftThigh;

	//RightLeg
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Deadbody")
	TObjectPtr<class USceneComponent> DeadbodyRightLegSpawnPoint;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Deadbody")
	TSubclassOf<class ADeadbodyParts> DeadbodyRightLeg;

	//LeftLeg
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Deadbody")
	TObjectPtr<class USceneComponent> DeadbodyLeftLegSpawnPoint;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Deadbody")
	TSubclassOf<class ADeadbodyParts> DeadbodyLeftLeg;
};
