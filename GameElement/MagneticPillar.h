#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MagneticPillar.generated.h"


UENUM(BlueprintType)
enum class EMagneticPillarActivationState : uint8
{
	E_None			UMETA(DisplayName = "None"),
	E_1PReady		UMETA(DisplayName = "1P Ready"),
	E_2PReady		UMETA(DisplayName = "2P Ready"),
	E_1PActivated	UMETA(DisplayName = "1P Activated"),
	E_2PActivated	UMETA(DisplayName = "2P Activated"),
	E_CoolTime		UMETA(DisplayName = "CoolTime"),
	E_Locked		UMETA(DisplayName = "Locked"),
	E_Broken		UMETA(DisplayName = "Broken"),
};


UCLASS()
class TRAPPERPROJECT_API AMagneticPillar : public AActor
{
	GENERATED_BODY()
	
public:	
	AMagneticPillar();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(EEndPlayReason::Type EndPlayReason) override;

public:
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	void SetOutline(bool Value);

public:
	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool IsActivated() const;
	bool IsLocked() const;
	void S2SSetLock(bool Lock);
	void Break();

///-----------------------------------------------------------------------------------------
///										Overlap Events
///-----------------------------------------------------------------------------------------
public:
	UFUNCTION()
	virtual void OnArrowHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, 
		FVector NormalImpulse, const FHitResult& Hit);

	UFUNCTION()
    void OnOverlapBeginMonster(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, 
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
    void OnOverlapEndMonster(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, 
		int32 OtherBodyIndex);

///-----------------------------------------------------------------------------------------
///											Components
///-----------------------------------------------------------------------------------------
#pragma region Components
public:
	UPROPERTY(EditDefaultsOnly, Category = "Components")
	TSubclassOf<AActor> RunestoneFragmentClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	TObjectPtr<class UParticleSystemComponent> ShineOutline;

	UPROPERTY(EditDefaultsOnly, Category = "Components")
	TObjectPtr<class USphereComponent> TriggerArea1;

	UPROPERTY(EditDefaultsOnly, Category = "Components")
	TObjectPtr<class UStaticMeshComponent> Foundation;

	UPROPERTY(EditDefaultsOnly, Category = "Components")
	TObjectPtr<class UStaticMeshComponent> RuneStone;

	UPROPERTY(EditDefaultsOnly, Category = "Components")
	TObjectPtr<USceneComponent> RuneStoneShell;

	UPROPERTY(EditDefaultsOnly, Category = "Components")
	TObjectPtr<USceneComponent> EffectSpawnPoint0;

	UPROPERTY(EditDefaultsOnly, Category = "Components")
	TObjectPtr<USceneComponent> CoolTimeEffectSpawnPoint;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<class UAudioComponent> AudioComponent;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<class UMapMarkerComponent> MarkerComponent;
#pragma endregion Components

///-----------------------------------------------------------------------------------------
///											Property
///-----------------------------------------------------------------------------------------
#pragma region Property
	UPROPERTY(EditDefaultsOnly, Category = "Property")
	float ActivationTime = 2.f;

	UPROPERTY(EditDefaultsOnly, Category = "Property")
	float ActivationWaitTime = 15.f;

	UPROPERTY(EditDefaultsOnly, Category = "Property")
	float CoolTime = 15.f;

	UPROPERTY(EditInstanceOnly, Category = "Property")
	TObjectPtr<AMagneticPillar> ForePillar = nullptr;


	UPROPERTY(EditAnywhere, Category = "Bouncing Property")
	float RotationSpeed = -35.f;

	UPROPERTY(EditAnywhere, Category = "Bouncing Property")
	float InitHeight = 400.f;

	UPROPERTY(EditAnywhere, Category = "Bouncing Property")
	float DeactivatedHeight = 150.f;

	UPROPERTY(EditAnywhere, Category = "Bouncing Property")
	float HeightAmplitude = 30.f;

	UPROPERTY(EditAnywhere, Category = "Bouncing Property")
	float HeightFrequency = 0.2f;

	UPROPERTY(EditAnywhere, Category = "Bouncing Property")
	float MinBouncingAmplitute = 5.f;

	UPROPERTY(EditAnywhere, Category = "Bouncing Property")
	float MaxBouncingAmplitute = 12.f;

	UPROPERTY(EditAnywhere, Category = "Bouncing Property")
	float MinBouncingSpeed = 15.f;

	UPROPERTY(EditAnywhere, Category = "Bouncing Property")
	float MaxBouncingSpeed = 25.f;

	UPROPERTY(EditAnywhere, Category = "Bouncing Property")
	float MinBouncingAngle = 5.0f;

	UPROPERTY(EditAnywhere, Category = "Bouncing Property")
	float MaxBouncingAngle = 10.f;

	UPROPERTY(EditAnywhere, Category = "Bouncing Property")
	float MinBouncingAngleSpeed = 5.f;

	UPROPERTY(EditAnywhere, Category = "Bouncing Property")
	float MaxBouncingAngleSpeed = 10.f;

#pragma endregion Property

///-----------------------------------------------------------------------------------------
///										Sound & Effects
///-----------------------------------------------------------------------------------------
#pragma region Sound & Effect
	UPROPERTY(EditDefaultsOnly, Category = "VFX")
	TObjectPtr<class UNiagaraSystem> Hit1PNiagara;

	UPROPERTY(EditDefaultsOnly, Category = "VFX")
	TObjectPtr<class UNiagaraSystem> Hit2PNiagara;

	UPROPERTY(EditDefaultsOnly, Category = "VFX")
	TObjectPtr<class UNiagaraSystem> Activation1PNiagara;

	UPROPERTY(EditDefaultsOnly, Category = "VFX")
	TObjectPtr<class UNiagaraSystem> Activation2PNiagara;

	UPROPERTY(EditDefaultsOnly, Category = "VFX")
	TObjectPtr<class UNiagaraSystem> LockedNiagara;

	UPROPERTY(EditDefaultsOnly, Category = "VFX")
	TObjectPtr<class UParticleSystem> CoolTimeParticle;

	UPROPERTY(EditDefaultsOnly, Category = "Sound")
	TObjectPtr<class USoundBase> HalfActivateSound;

	UPROPERTY(EditDefaultsOnly, Category = "Sound")
	TObjectPtr<USoundBase> FullActivateSound;
#pragma endregion Sound & Effect

public:
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Overlapping")
	TSet<TObjectPtr<AActor>> OverappingActors;

private:
	// ----- Ref -----
	UPROPERTY() TObjectPtr<class ATrapperGameState> GameState;
	UPROPERTY() TObjectPtr<class UNiagaraComponent> Hit1PEffectRef;
	UPROPERTY() TObjectPtr<UNiagaraComponent> Hit2PEffectRef;
	UPROPERTY() TObjectPtr<UNiagaraComponent> Activation1PEffectRef;
	UPROPERTY() TObjectPtr<UNiagaraComponent> Activation2PEffectRef;

	UPROPERTY() TObjectPtr<UNiagaraComponent> LockedEffectRef;
	UPROPERTY() TObjectPtr<class UParticleSystemComponent> CooldonwEffectRef;

	// ----- State ----- 
	UPROPERTY(Replicated, EditAnywhere)
	EMagneticPillarActivationState ActivationState = EMagneticPillarActivationState::E_None;
	EMagneticPillarActivationState LastActivationState = EMagneticPillarActivationState::E_None;
	
	bool bIsPendingLock = false;
	bool bIsLerpingActivationState = false;

	// ----- 진동 ----- 
	float TransformDeltaSeconds = 0.01f;
	float TimeOffset = 0.f;							// random
	
	FVector BouncingStartPos = FVector();
	FVector BouncingAmplitude = FVector(20,20,20);	// random
	FVector BouncingSpeed = FVector(10,10,10);		// random

	float PitchBouncingAngle = 0.f;					// random
	float RollBouncingAngle = 0.f;					// random
	float PitchBouncingAngleSpeed = 0.f;			// random
	float RollBouncingAngleSpeed = 0.f;				// random

	// ----- 타이머 ----- 
	FTimerHandle ActivationTimer;
	FTimerHandle ActivationWaitTimer;
	FTimerHandle CooldownTimer;
	FTimerHandle TransformUpdaterTimer;
	FTimerHandle LerpActivationStateTimer;

	void SpawnEffects();
	void DeactivateWait();
	void Deactivate();
	void Cooldown();
	void S2SLock();
	void UpdateTransform();
	void LerpActivationState();
	void LockNextTick();

	// 발동 핑 관련
	class UPingMechanics* GetPingMechanicsRef();

	// ----- RPC -------
	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPCActivateEffect(EMagneticPillarActivationState type);
	void MulticastRPCActivateEffect_Implementation(EMagneticPillarActivationState type);
	
	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPCDeactivateEffect(EMagneticPillarActivationState type);
	void MulticastRPCDeactivateEffect_Implementation(EMagneticPillarActivationState type);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPCSetIcon(bool IsActivated);
	void MulticastRPCSetIcon_Implementation(bool IsActivated);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPCBreak();
	void MulticastRPCBreak_Implementation();
};
