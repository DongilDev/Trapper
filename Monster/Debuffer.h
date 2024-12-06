// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Monster/BaseMonster.h"
#include "Debuffer.generated.h"

/**
 * 
 */
UCLASS()
class TRAPPERPROJECT_API ADebuffer : public ABaseMonster
{
	GENERATED_BODY()
	
public:
	ADebuffer();

protected:
	virtual void BeginPlay() override;

public:
	void ClearTarget();

	//network
	UFUNCTION(Server, Reliable)
	void ServerRPCGoToTowerState();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPCGoToTowerState();

	UFUNCTION(Server, Reliable)
	void ServerRPCDebuffeState();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPCDebuffeState();

	UFUNCTION()
	void OnOverlapBeginMonsters(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	UFUNCTION()
	void OnOVerlapEndMonsters(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex);

public:
	//Components
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mesh")
	TObjectPtr<class UStaticMeshComponent> PillerMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mesh")
	TObjectPtr<class UStaticMeshComponent> MonsterMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Buffer")
	TObjectPtr<class USphereComponent> BufferRadius;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effect")
	TObjectPtr<class UNiagaraSystem> DebuffeEffect;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effect")
	TObjectPtr<class USceneComponent> DebuffePosition;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effect")
	TObjectPtr<class UNiagaraSystem> MoveEffect;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effect")
	TObjectPtr<class USceneComponent> MovePosition;

	//[Path]
	FVector NextReachTarget;
	int CurrentTarget;
	int LastTargetNumber;

	//State
	uint8 bIsDebuffeStart : 1 = false;
};
