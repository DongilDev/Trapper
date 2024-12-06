// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Quest/Interact.h"
#include "Tower.generated.h"

UCLASS()
class TRAPPERPROJECT_API ATower : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ATower();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// TEST
	FTimerHandle GameOverHandle;
	UFUNCTION() void GameOverDebug();

public:
	UFUNCTION()
	float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	void CheckFireCondition();
	void Fire();

	void HealingTower(float Value);
	UFUNCTION(Server, Reliable)
	void ServerRPCHealingTower(float Value);
	
	UFUNCTION(NetMulticast, Reliable)
	void MulticastUpgradeTower(int32 TowerLevel);

	//Reach Check
	UFUNCTION()
	void OnOverlapBeginMonster(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	int32 GetTowerHP() { return TowerHP; }
public:
	/// ------------- Components -------------
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<class USceneComponent> RootComp;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<class UStaticMeshComponent> Mesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<class UStaticMeshComponent> TopMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<class UStaticMeshComponent> BottomMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<class USceneComponent> ProjectileSpawn;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attack")
	TSubclassOf<class ATowerProjectile> TowerProjectileClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<class UBoxComponent> ReachTriggerBox;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	TObjectPtr<class UNiagaraComponent> TowerEffect;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
	TObjectPtr<class UNiagaraSystem> TowerEffect1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
	TObjectPtr<class UNiagaraSystem> TowerEffect2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
	TObjectPtr<class UNiagaraSystem> TowerEffect3;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
	TObjectPtr<class UNiagaraSystem> TowerEffect4;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
	TObjectPtr<class UNiagaraSystem> TowerEffect5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
	TObjectPtr<class UNiagaraSystem> TowerEffect6;
	

public:
	FTimerHandle FireRateTimerHandle;

public:
	UPROPERTY(ReplicatedUsing = OnRep_HpChanged)
	float TowerHP;
	
	UFUNCTION() void OnRep_HpChanged();

	//�ִ� ü��
	UPROPERTY(EditAnywhere, Category = "Stats")
	float MaxHP;

	//�߻� ����
	UPROPERTY(EditAnywhere, Category = "Stats")
	float FireRate;
};
