// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Turret.generated.h"

UCLASS()
class TRAPPERPROJECT_API ATurret : public APawn
{
	GENERATED_BODY()

public:
	ATurret();

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void BeginDestroy() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	//DetectMonster
	UFUNCTION()
	void OnOverlapBeginMonster(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapEndMonster(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex);

	UFUNCTION()
	void OnOverlapBeginPlayer(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepHitResult);

	UFUNCTION()
	void OnOverlapEndPlayer(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

protected:
	UFUNCTION()
	void OnSeePawn(APawn* Pawn);

	float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

private:
	void FireAtClosestPawn();
	void UpdateHealthHUDRotation();

public:
	TSet<TObjectPtr<APawn>> DetectedPawns;

private:
	UPROPERTY(EditAnywhere, Category = "Trap")
	TObjectPtr<class USceneComponent> DummyRoot;

	UPROPERTY(EditAnywhere, Category = "Trap")
	TObjectPtr<class UStaticMeshComponent> Mesh;

	UPROPERTY(EditAnywhere, Category = "Turret")
	TObjectPtr<class UStaticMeshComponent> MeshHit;

	UPROPERTY(EditAnywhere, Category = "Trap")
	TObjectPtr<class UArrowComponent> ProjectileSpawnLocation;

	UPROPERTY(EditAnywhere, Category = "Trap")
	TObjectPtr<class UPawnSensingComponent> PawnSensing;

	UPROPERTY(EditAnywhere, Category = "Trap")
	TSubclassOf<class ATurretProjectile> Projectile;

	UPROPERTY(EditDefaultsOnly, Category = "HUD")
	TObjectPtr<class UWidgetComponent> WidgetComponent;

	//Monster Detect
	UPROPERTY(EditAnywhere, Category = "Monster")
	TObjectPtr<class UBoxComponent> MonsterDetectCollider;

	UPROPERTY(EditAnywhere, Category = "Monster")
	TObjectPtr<class UStaticMeshComponent> MonsterAttackPoint; 

	UPROPERTY(EditAnywhere, Category = "Player")
	TObjectPtr<class USphereComponent> PlayerDetectCollider;

	UPROPERTY(EditAnywhere, Category = "Niagara")
	TObjectPtr<class UNiagaraComponent> NiagaraComponent;



	UPROPERTY(EditAnywhere, Category = "Stats")
	float MaxHP = 1200.f;

	UPROPERTY(EditAnywhere, Category = "Stats")
	float Damage = 10.f;

	UPROPERTY(ReplicatedUsing = OnRep_CurrentHP)
	float CurrentHP = 0.f;

	UFUNCTION()
	void OnRep_CurrentHP();

	FTimerHandle UpdateHealthHUDRotationTimerHandle;
	FTimerHandle FireAtClosestPawnNextTickTimerHandle;

	TObjectPtr<class UTurretHealthBar> HealthHUDRef;
	TObjectPtr<APlayerCameraManager> PlayerCameraManager;

	uint8 bHasSeenPawns : 1 = false;
	uint8 bSetFireTimerNextTick : 1 = false;

	uint8 OverlapPlayerNum = 0;

	uint8 bIsNotSafe : 1 = false;

	UPROPERTY(EditAnywhere, Category = "GC")
	TSubclassOf<class AActor> TurretGC;

	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPCSpawnGC();

	/// RPC �ѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤ�


public:
	///MonsterDetected
	FVector TurretAttackPoint;

	///Detect ADC
	TSet<TObjectPtr<class AADC>> DetectADCList;

	UPROPERTY(EditAnywhere, Category = "Orders")
	uint32 TargetNumber = 0;

};
