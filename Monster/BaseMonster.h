// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Data/MonsterDataTables.h"
#include "BaseMonster.generated.h"

UENUM(BlueprintType)
enum class EMonsterStateType : uint8
{
	GoToTower		UMETA(DisplayName = "GoToTower"),		// 0
	IsDetect		UMETA(DisplayName = "Detect"),			// 1
	IsAttack		UMETA(DisplayName = "Attack"),			// 2
	IsStun			UMETA(DisplayName = "Stun"),			// 3
	IsDead			UMETA(DisplayName = "Dead"),			// 4
	ISArrowShoot	UMETA(DisplayName = "Shoot"),			// 5
	IsTowerReach	UMETA(DisplayName = "Reach"),			// 6
	IsTowerAttack	UMETA(DisplayName = "TowerAttack"),		// 7
	Idle			UMETA(DisplayName = "Idle"),			// 8
	IsPillarActived UMETA(DisplayName = "PillarActived"),	// 9
	IsStunEnd		UMETA(DisplayName = "StunEnd"),			// 10
	IsHit			UMETA(DisplayName = "Hit"),				// 11
	IsDetectStart	UMETA(DisplayName = "DetectStart"),		// 12
	IsAttackStart	UMETA(DisplayName = "AttackStart"),		// 13
	ISAttackEnd		UMETA(DisplayName = "AttackEnd"),		// 14
	IsDestroy		UMETA(DisplayName = "Destroy"),			// 15

	//For Boss State
	IsCloseAttack	UMETA(DisplayName = "CloseAttack"),		// 16
	IsTrapAttack	UMETA(DisplayName = "TrapAttack"),		// 17
	IsMonsterSpawn	UMETA(DisplayName = "SpawnMonster"),	// 18 
	IsTrapDetect	UMETA(DisplayName = "TrapDetect"),		// 19
	IsPhaseChange	UMETA(DisplayName = "PhaseChange"),		// 20
	IsPhaseChanging UMETA(DisplayName = "PhaseChanging"),	// 21

	//For Debuffer State
	IsDebuffing		UMETA(DisplayName = "Debuffing"),		// 22

	//Turret Detect
	IsTurretDetect	UMETA(DisplayName = "TurrentDetect"),	// 23
	IsTowerDetect	UMETA(DisplayName = "TowerDetect"),		// 24

	//Stuck
	IsStuck			UMETA(DisplayName = "Stuck"),			// 25

	IsRockAttack	UMETA(displayName = "RockAttack"),		// 26

};

UCLASS()
class TRAPPERPROJECT_API ABaseMonster : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABaseMonster();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void BeginDestroy() override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public:
	/// ----------------- Monster State ------------------
	float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
	void ActorDie();
	void ItemSpawn(AActor* ItemOwner);
	void MonsterDieCheck();

	UFUNCTION(Server, Reliable)
	void ServerRPCTeleportToDie();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPCTeleportToDie();
	/*UFUNCTION()
	void OnHitArrow(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);*/

	//Data
	UPROPERTY(VisibleAnywhere, Category = "Monster")
	TArray<FMonsterInfo> MonsterInfoTable;

	UDataTable* DataTable;

	///HUD
	void UpdateBossHPBar();

	/// ----------------- UpgradSystem -------------------
	void ChangeMonsterSpeed();
	void ChangeMonsterHealth();

	/// ----------------- Headshoot ----------------------
	UFUNCTION()
	void OnOverlapBeginHead(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnHitHead(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		FVector NormalImpulse, const FHitResult& Hit);

	void OnHeadShootHit();
	void OnNotHeadShootHit();

	/// ----------------- Detect -------------------------
	UFUNCTION()
	void OnOverlapBeginCharacter(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapEndCharacter(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex);

	UFUNCTION()
	void OnOverlapBeginAttackCharacter(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapEndAttackCharacter(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex);

	/// ----------------- Network ------------------------
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	UFUNCTION() void OnRep_IsDamaged();

	//Animation Montage RPC
	UFUNCTION(Server, Reliable)
	void ServerRPCDead();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPCDead();

	UFUNCTION(Server, Reliable)
	void ServerRPCDetectStart();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPCDetectStart();

	UFUNCTION(Server, Reliable)
	void ServerRPCHit();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPCHit();

	UFUNCTION(Server, Reliable)
	void ServerRPCHeadHit();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPCHeadHit();

	UFUNCTION(Server, Reliable)
	void ServerRPCIdle();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPCIdle();

	UFUNCTION(Server, Reliable)
	void ServerRPCKnockback();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPCKnockback();

	UFUNCTION(Server, Reliable)
	void ServerRPCStunLoop();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPCStunLoop();

	UFUNCTION(Server, Reliable)
	void ServerRPCDownLoop();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPCDownLoop();

	//BossHP
	UFUNCTION(Server, Reliable)
	void ServerRPCShowBossHPBar();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPCShowBossHPBar();

	/// ------------------ Effect ----------------------
	UFUNCTION(NetMulticast, Reliable)
	void ServerRPCPlayDieEffect();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastPRCPlayDieEffect();

	UFUNCTION(NetMulticast, Reliable)
	void ServerRPCPlayArrowHitEffect();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastPRCPlayArrowHitEffect();

	UFUNCTION(Server, Reliable)
	void ServerRPCEffectActive();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPCEffectActive();

	UFUNCTION(Server, Reliable)
	void ServerRPCEffectDeactive();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPCEffectDeactive();

	/// ------------------ Debuffer -------------------
	void CheckDebufferState();

public:
	///----------------Component----------------
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Sight")
	TObjectPtr<class USphereComponent> MainSight;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Sight")
	TObjectPtr<class UBoxComponent> AttackSight;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "HeadShoot")
	TObjectPtr<class UCapsuleComponent> HeadShootCollider;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ItemDrop")
	TObjectPtr<class USceneComponent> ItemDropPoint;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MapMarker")
	TObjectPtr<class UMapMarkerComponent> MapMarker;
	//UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Detect")

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "HUD")
	TObjectPtr<class UWidgetComponent> WidgetComponent;

	// [Particle]
	UPROPERTY(EditDefaultsOnly, Category = "VFX")
	class UParticleSystem* WoodHitParticle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Niagara")
	TObjectPtr<class UNiagaraSystem> DieEffect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Niagara")
	TObjectPtr<class UNiagaraSystem> ArrowHitEffect;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Niagara")
	TObjectPtr<class USceneComponent> DebuffeEffectPosition;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Niagara")
	TObjectPtr<class UNiagaraSystem> DebuffeActiveEffect;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Niagara")
	TObjectPtr<class UNiagaraComponent> DebuffeActiveEffectComponent;

	//[Animation]
	TObjectPtr<class UAnimInstance> AnimInstance;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
	TObjectPtr<class UAnimMontage> DeadMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
	TObjectPtr<class UAnimMontage> DetectStartMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
	TObjectPtr<class UAnimMontage>  HitMontage1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
	TObjectPtr<class UAnimMontage>  HitMontage2;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
	TObjectPtr<class UAnimMontage>  HitMontage3;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
	TObjectPtr<class UAnimMontage> HeadHitMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
	TObjectPtr<class UAnimMontage> IdleMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
	TObjectPtr<class UAnimMontage> KnockbackMontage1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
	TObjectPtr<class UAnimMontage> KnockbackMontage2;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
	TObjectPtr<class UAnimMontage> StunMontageLoop;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
	TObjectPtr<class UAnimMontage> DownMontageLoop;


	//Nav Mesh
	//UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Nav")
	//TObjectPtr<class UNavigationInvokerComponent> NavInvoker;
private:
	TObjectPtr<class ATrapperPlayerController> TrapperPlayerController;

public:
	// [Monster State]
	UPROPERTY(ReplicatedUsing = OnRep_IsDamaged)  float CurrentHP;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MonsterState")
	EMonsterStateType MonsterState = EMonsterStateType::GoToTower;

	uint8 bISArrowShoot : 1 = false;
	uint8 bIsHeadShoot : 1 = false;
	uint8 bIsAttackEnd : 1 = false;
	uint8 bIsSkillStart : 1 = false;
	uint8 bIsDead : 1 = false;
	uint8 bIsDebuffe : 1 = false;
	uint8 bIsDebuffeTimerStart : 1 = false;
	uint8 bIsSpawn : 1 = false;
	uint8 bIsTurretAttack : 1 = false;

	uint8 bIsBossStage : 1 = false;
	uint8 bIsInOil : 1 = false;

	//[Path]
	FVector NextReachTarget;
	int CurrentTarget;
	int LastTargetNumber;
	uint8 bIsLastTarget : 1 = false;

	// [Detect]
	uint8 bIsSightDetect : 1 = false;
	uint8 bIsAttackDetect : 1 = false;

	uint8 bIsPlayerDetected : 1 = false;
	uint8 bIsTurretDetected : 1 = false;
	uint8 bIsTowerDetected  : 1 = false;

	TObjectPtr<class ATrapperPlayer> DetectPlayer;
	TObjectPtr<class ATrapperPlayer> AttackPlayer;
	TObjectPtr<class ATurret>		 DetectTurret;
	uint32							 DetectTurretNumber;
	TObjectPtr<class ATower>		 Tower;
	TObjectPtr<class AActor>		 RealDamageCauser;

	//[HUD]
	TObjectPtr<APlayerCameraManager> PlayerCameraManager;
	
	float HealthHUDVisibleTime = 5.f;
	float HealthHUDSightVisibleTime = 3.f;
	TObjectPtr<class UMonsterHealthBar> HealthHUDRef;
	FTimerHandle UpdateHealthHUDRotationTimer;
	void UpdateHealthHUDRotation();
	void OnPlayerSight();
	void UpdateHealthHUD();
	void UpdateHealthHUDTick();

	//[Player]
	TObjectPtr<class ATrapperPlayer> player;
	TObjectPtr<class AArrow> arrow;

	//DropItem
	UPROPERTY(EditDefaultsOnly, Category = "Item")
	TSubclassOf<class AItem> ItemClass;

	UPROPERTY(EditAnywhere, Category = "Item")
	float ItemMinDropNumber;
	UPROPERTY(EditAnywhere, Category = "Item")
	float ItemMaxDropNumber;
	UPROPERTY(EditAnywhere, Category = "Item")
	float SpawnLocationCorrection;

public:
	FTimerHandle FireRateTimerHandle;
	FTimerHandle DebufferTimerHandle;

public:
	//시작위치(Object pool)
	FVector StartPoint;

	//Magnetic Pillar
	FVector ActivedPillarLocation;

	//최대 체력
	UPROPERTY(EditAnywhere, Category = "Stats")
	float MaxHP;

	//발사 간격
	UPROPERTY(EditAnywhere, Category = "Stats")
	float FireRate;

	//추적 시간
	UPROPERTY(EditAnywhere, Category = "Stats")
	float DetectTime;

	//디버프 시간
	UPROPERTY(EditAnywhere, Category = "Stats")
	float DebuffeTime;

	//몬스터 속도
	UPROPERTY(EditAnywhere, Category = "Stats")
	float MonsterSpeed;

	UPROPERTY(EditAnywhere, Category = "Stats")
	float MonsterDetectSpeed;

	UPROPERTY(EditAnywhere, Category = "Stats")
	float MonsterEffectSpeed;

	//몹몰이 속도
	UPROPERTY(EditAnywhere, Category = "Stats")
	float MonsterPillarSpeed;

	//몹몰이 스턴 시간
	UPROPERTY(EditAnywhere, Category = "Stats")
	float MonsterPillarTime;


protected:
	TWeakObjectPtr<class ATrapperGameState> TrapperGameState;

#pragma region Trap

public:
	FORCEINLINE uint8 GetOverlapOilCount() const { return OverlapOilCount; }
	FORCEINLINE void OverlapOilCountUp() { ++OverlapOilCount; }
	FORCEINLINE void OverlapOilCountDown() { --OverlapOilCount; }

private:
	uint8 OverlapOilCount = 0;

#pragma endregion Trap

#pragma region MultikillHud
private:
	void UpdateKillCount(AActor* DamageCauser);

#pragma endregion MultikillHud

#pragma region Turret

public:
	void AddProjectile(class ATurretProjectile* Projectile);
	void RemoveProjectile(class ATurretProjectile* Projectile);
	void AllProjectileStopHoming();

private:
	UPROPERTY()
	TSet<TObjectPtr<class ATurretProjectile>> TurretProjectileArray;

	uint8 bSafeRangedFor = false;
	TArray<TObjectPtr<class ATurretProjectile>> SafeAddTurretProjectileArray;
	TArray<TObjectPtr<class ATurretProjectile>> SafeRemoveTurretProjectileArray;

#pragma region Turret

#pragma region Score
private:
	void UpdateScore(AActor* DamageCauser);

#pragma endregion MultikillHud
};

