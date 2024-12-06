// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Monster/BaseMonster.h"
#include "Monster/Animation/AnimationAttackInterface.h"
#include "Boss_Phase2.generated.h"

/**
 * 
 */
UCLASS()
class TRAPPERPROJECT_API ABoss_Phase2 : public ABaseMonster, public IAnimationAttackInterface
{
	GENERATED_BODY()
	
public:
	ABoss_Phase2();

protected:
	virtual void BeginPlay() override;

	//Attack Hit Section
	virtual void AttackHitCheck() override;

public:
	///-------------------------State-------------------------	
	float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	///------------------------Attck Skills-------------------------
	void SpawnSword();

	void SpawnMonsters();

	///--------------------------Detect----------------------------
	UFUNCTION()
	void OnOverlapBeginCloseSight(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapEndCloseSight(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex);

	UFUNCTION()
	void OnOverlapBeginTrapSight(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapEndTrapSight(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex);

	///----------------------------Montage RPC-----------------------------
	UFUNCTION(Server, Reliable)
	void ServerRPCADCAttack();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPCADCAttack();

	UFUNCTION(Server, Reliable)
	void ServerRPCTrapAttack();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPCTrapAttack();

	UFUNCTION(Server, Reliable)
	void ServerRPCRockAttack();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPCRockAttack();

	UFUNCTION(Server, Reliable)
	void ServerRPCSpawnMonster();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPCSpawnMonster();

	UFUNCTION(Server, Reliable)
	void ServerRPCCDCAttack();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPCCDCAttack();

	UFUNCTION(Server, Reliable)
	void ServerRPCCDCAttackCombo();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPCCDCAttackCombo();

	UFUNCTION(Server, Reliable)
	void ServerRPCCameraShaking();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPCCameraShaking();

	UFUNCTION(Server, Reliable)
	void ServerRPCBossHitEffect();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPCBossHitEffect();

public:
	///-----------------Component-----------------------
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Sight")
	TObjectPtr<class UBoxComponent> CloseAttackSight;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Sight")
	TObjectPtr<class UBoxComponent> TrapAttackSight;

	//원거리 공격
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attact")
	TObjectPtr<class USceneComponent> SwordSpawn;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
	TSubclassOf<class ABossThrowSword> ThrowSword;

	//근거리 공격
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	TSubclassOf<class ABossSword> BossSwordClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	TObjectPtr<class ABossSword> BossSword;

	///------------------Montage----------------------------
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
	TObjectPtr<class UAnimMontage> AttackADCMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
	TObjectPtr<class UAnimMontage> AttackCDCMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
	TObjectPtr<class UAnimMontage> AttackCDCComboMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
	TObjectPtr<class UAnimMontage> AttackTrapMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
	TObjectPtr<class UAnimMontage> AttackRockMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
	TObjectPtr<class UAnimMontage> SpawnMonsterMontage;


	///Detect Trap
	TSet<TObjectPtr<class ATrapBase>> DetectTrapList;
	FVector DetectTrapLocation;
	
	TSet<TObjectPtr<class AMagneticPillar>> DetectMagneticList;
	FVector DetectMagneticLocation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float TrapSkillCoolTime;

	uint8 bIsTrapAttackState : 1 = false;
	void TrapAttackState()
	{
		bIsTrapAttackState = true;
		UE_LOG(LogTemp, Warning, TEXT("Timer Work! Trap"));
		GetWorld()->GetTimerManager().ClearTimer(TimerHandleTrap);
	}
	
	///Spawn Monster
	TObjectPtr<class ATrapperGameState> GameState;
	TObjectPtr<class ASpawner> MonsterSpawner;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster")
	TSubclassOf<class ADetectedMonster> DetectedMonsterClass;

	///CDC Combo
	uint8 bIsCDCComboState : 1 = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "States")
	float CDCSkillCoolTime;

	uint8 bIsCDCAttackState : 1 = false;
	void CDCAttackState() 
	{ 
		bIsCDCAttackState = true; 
		UE_LOG(LogTemp, Warning, TEXT("Timer Work! CDC"));
		GetWorld()->GetTimerManager().ClearTimer(TimerHandleCDC);
	}

	///ADC 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "States")
	float ADCSkillCoolTime;

	uint8 bIsADCAttackState : 1 = false;
	void ADCAttackState()
	{
		bIsADCAttackState = true;
		UE_LOG(LogTemp, Warning, TEXT("Timer Work! ADC"));
		GetWorld()->GetTimerManager().ClearTimer(TimerHandleADC);
	}

	/// Stats
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float AttackCDCDamage = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float MaxHitGauge;

	float HitGauge;

	TObjectPtr<class UMaterialInstanceDynamic> BossMaterial0;
	TObjectPtr<class UMaterialInstanceDynamic> BossMaterial1;

	void ChangeBossMaterial() 
	{
		BossMaterial0->SetScalarParameterValue(TEXT("ONOFFEM"), 0);
		BossMaterial1->SetScalarParameterValue(TEXT("ONOFFEM"), 0);
		GetWorld()->GetTimerManager().ClearTimer(TimerHandleBossHit);
	};

private:
	FTimerHandle TimerHandleTrap;
	FTimerHandle TimerHandleCDC;
	FTimerHandle TimerHandleADC;
	FTimerHandle TimerHandleBossHit;

public:
	uint8 bSafeRanged : 1 = false;
	uint32 ArchNum = 0;

	TObjectPtr<class ATrapperPlayer> DamagedPlayer1P;
	TObjectPtr<class ATrapperPlayer> DamagedPlayer2P;

	///Arch
	TObjectPtr<class AArch> Boss2Arch0;
	TObjectPtr<class AArch> Boss2Arch1;
};
