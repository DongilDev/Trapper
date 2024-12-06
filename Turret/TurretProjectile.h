// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TurretProjectile.generated.h"

UCLASS()
class TRAPPERPROJECT_API ATurretProjectile : public AActor
{
	GENERATED_BODY()

public:
	ATurretProjectile();

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
	FORCEINLINE void SetHomingTarget(AActor* Target) { HomingTarget = Target; }
	void StopHomingTarget();
	void LaunchingProjectile();
	FORCEINLINE void SetDamage(float Value) { Damage = Value; }
	FORCEINLINE void EnhancedProjectile() { bEnhance = true; }

protected:
	UFUNCTION()
	void OnOverlapBeginCharacter(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepHitResult);

	UFUNCTION()
	void OnNiagaraFinished(UNiagaraComponent* PSystem);

private:
	void UpdateRotationTimer();

private:
	UPROPERTY(EditAnywhere, Category = "Trap")
	TObjectPtr<class UProjectileMovementComponent> ProjectileMovement;

	UPROPERTY(EditAnywhere, Category = "Trap")
	TObjectPtr<class UStaticMeshComponent> Mesh;

	UPROPERTY(EditAnywhere, Category = "Niagara")
	TObjectPtr<class UNiagaraComponent> NormalNiagaraComponent;

	UPROPERTY(EditAnywhere, Category = "Niagara")
	TObjectPtr<class UNiagaraComponent> EnhanceNiagaraComponent;

	UPROPERTY(EditAnywhere, Category = "Niagara")
	TObjectPtr<class UNiagaraSystem> HitNiagara;

	UPROPERTY(EditDefaultsOnly, Category = "Sound")
	TObjectPtr<class UAudioComponent> AudioComponent;

	UPROPERTY(EditDefaultsOnly, Category = "Sound")
	TObjectPtr<class USoundWave> SpawnSound;

	UPROPERTY(EditDefaultsOnly, Category = "Sound")
	TObjectPtr<class USoundWave> HitSound;

	UPROPERTY(Replicated)
	uint8 bEnhance : 1 = false;

	float Damage = 0;

	UPROPERTY(ReplicatedUsing = OnRep_HomingTarget)
	TObjectPtr<class AActor> HomingTarget;

	UFUNCTION()
	void OnRep_HomingTarget();

	FTimerHandle RotationUpdateTimerHandle;

	uint8 bHitCharacter : 1 = false;

	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPCSafeClearTimer(FVector ImpactPoint);

};
