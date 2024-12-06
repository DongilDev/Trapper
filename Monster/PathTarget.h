// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PathTarget.generated.h"

UCLASS()
class TRAPPERPROJECT_API APathTarget : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APathTarget();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	UFUNCTION()
	void OnOverlapBeginMonster(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	FVector GetRandomReachPoint();

public:
	///Component
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<class UStaticMeshComponent> MainTarget;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<class UBoxComponent> ReachCheck;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<class UStaticMeshComponent> SubTarget1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<class UStaticMeshComponent> SubTarget2;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<class UStaticMeshComponent> SubTarget3;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<class UStaticMeshComponent> SubTarget4;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<class UStaticMeshComponent> SubTarget5;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<class UStaticMeshComponent> BossTarget;

public:
	///Check Reach
	TArray<class ABaseMonster*> CheckTarget;

	UPROPERTY(EditAnywhere, Category = "Orders")
	uint32 TargetOrders;

	FVector SubTargetLocation1;
	FVector SubTargetLocation2;
	FVector SubTargetLocation3;
	FVector SubTargetLocation4;
	FVector SubTargetLocation5;

	FVector BossTargetLocation;
};
	