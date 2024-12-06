// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Arrow.generated.h"

UCLASS()
class TRAPPERPROJECT_API AArrow : public AActor
{
	GENERATED_BODY()

friend class UBowMachanics;
public:	
	AArrow();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:	
	virtual void Tick(float DeltaTime) override;
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

///-----------------------------------------------------------------------------------------
///											멤버 함수						
///-----------------------------------------------------------------------------------------	
	UFUNCTION() virtual void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()	virtual void OnArrowHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
	UFUNCTION() virtual void OnExplosionOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION() virtual void OnExplosionOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp ,int32 OtherBodyIndex);

	void DeactivateArrow();
	
	bool IsSpawnedBy1P() const	{ return bIsSpawnedBy1P; }
	bool IsActivated() const	{ return bIsActive; }
	bool IsFullCharged() const	{ return bIsFullCharged; }

///-----------------------------------------------------------------------------------------
///											멤버 변수					
///-----------------------------------------------------------------------------------------
public:
	// -------- Property --------
	UPROPERTY(EditDefaultsOnly, Category = "Property") float MinSpeed = 10000.f;
	UPROPERTY(EditDefaultsOnly, Category = "Property") float MaxSpeed = 20000.f;
	UPROPERTY(EditDefaultsOnly, Category = "Property") float MinGravity = 1.00f;
	UPROPERTY(EditDefaultsOnly, Category = "Property") float MaxGravity = 1.00f;
	UPROPERTY(EditDefaultsOnly, Category = "Property") float LifeSpan = 15.f;
	UPROPERTY(EditDefaultsOnly, Category = "Property") float Mass = 0.2f; //kg
	UPROPERTY(EditDefaultsOnly, Category = "Property") float MinDamage = 55.f;  // 자성화살 45 
	UPROPERTY(EditDefaultsOnly, Category = "Property") float MaxDamage = 70.f;  // 자성화살 60

	// -------- 컴포넌트 --------
	UPROPERTY(EditDefaultsOnly) TObjectPtr<class UBoxComponent> CollisionBox;
	UPROPERTY(EditDefaultsOnly) TObjectPtr<class UStaticMeshComponent> ArrowMesh;
	UPROPERTY(EditDefaultsOnly) TObjectPtr<class USphereComponent> AwakenCollisionShepre;

	// -------- 파티클 --------
	UPROPERTY(EditDefaultsOnly, Category = "Component")
	TObjectPtr<USceneComponent> TrailAttachPoint;

	UPROPERTY(EditDefaultsOnly, Category = "Component")
	TObjectPtr<USceneComponent> TrailAttachPointOffset;

	UPROPERTY(EditDefaultsOnly, Category = "Component")
	TObjectPtr<USceneComponent> EffectSpawnPoint;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<class UAudioComponent> AudioComponent;

	UPROPERTY(EditDefaultsOnly, Category = "VFX")
	TObjectPtr<class UNiagaraSystem> HitEffect1P;

	UPROPERTY(EditDefaultsOnly, Category = "VFX")
	TObjectPtr<class UNiagaraSystem> HitEffect2P;

	UPROPERTY(EditDefaultsOnly, Category = "VFX")
	TObjectPtr<class UNiagaraSystem> TrailNiagara;
	
	UPROPERTY(EditDefaultsOnly, Category = "VFX")
	TObjectPtr<class UNiagaraSystem> AwakenParticle1P;

	UPROPERTY(EditDefaultsOnly, Category = "VFX")
	TObjectPtr<class UNiagaraSystem> AwakenParticle2P;
	
	UPROPERTY(EditDefaultsOnly, Category = "VFX")
	TObjectPtr<class UNiagaraSystem> AwakenExplosion1P;

	UPROPERTY(EditDefaultsOnly, Category = "VFX")
	TObjectPtr<class UNiagaraSystem> AwakenExplosion2P;

	UPROPERTY(EditDefaultsOnly, Category = "VFX")
	class UNiagaraSystem* MagneticEffect1P;

	UPROPERTY(EditDefaultsOnly, Category = "VFX")
	class UNiagaraSystem* MagneticEffect2P;
	
	UPROPERTY(EditDefaultsOnly, Category = "VFX")
	TObjectPtr<class UNiagaraComponent> TrailEffectRef = nullptr;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<class UNiagaraComponent> MagneticEffectRef = nullptr;


	// -------- 사운드 --------
	UPROPERTY(EditDefaultsOnly, Category = "Sound")
	TObjectPtr<class USoundCue> HitSound;

	UPROPERTY(EditDefaultsOnly, Category = "Sound")
	TObjectPtr<class USoundBase> FlySound;


///-----------------------------------------------------------------------------------------
///											Protected						
///-----------------------------------------------------------------------------------------
protected:
	virtual void FireArrow(FVector Direction, float Strength);
	void FireArrowReal();
	void ActivateTrailNiagara();
	void SpawnTrailNiagara();
	void SpawnAwakenEffect();
	void SpawnMagneticEffect();
	void ApplyExplosionDamage();

	UPROPERTY() TObjectPtr<class UProjectileMovementComponent> ProjectileCompRef;
	
	UPROPERTY() TObjectPtr<class ATrapperPlayer> OwningActor = nullptr;
	UPROPERTY() TObjectPtr<class ATrapperGameState> GameState = nullptr;

	UPROPERTY(Replicated) 
	bool bIsActive = false;

	float Damage = 0;
	bool bIsSpawnedBy1P = false;
	bool bIsFullCharged = false;
	bool bIsAwaken = false;
	FVector LastTrailAttachPosition = FVector();
	FVector Direction = FVector();
	float Strength = 0.f;

	UPROPERTY()
	TArray<TObjectPtr<AActor>> OverlapingMonsters;

///-----------------------------------------------------------------------------------------
///											RPC						
///-----------------------------------------------------------------------------------------
protected:
	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPCSetClientVisibility();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPCSpawnHitParticle(FVector Location);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPCSpawnExplosionParticle(FVector Location);
};
