// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DebufferPathTarget.generated.h"

UCLASS()
class TRAPPERPROJECT_API ADebufferPathTarget : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADebufferPathTarget();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	UFUNCTION()
	void OnOverlapBeginDebufferMonster(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapEndDebufferMonster(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex);

public:
	///Component
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<class UStaticMeshComponent> MainTarget;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<class UBoxComponent> ReachCheck;

	
	//존재할 수 있는 몬스터의 수
	UPROPERTY(EditAnywhere, Category = "Monster Counts")
	uint32 DebufferMonsterCount = 0;

	uint32 RemainMonsterCount = 0;

	///Check Reach
	TArray<class ADebuffer*> CheckTarget;

	UPROPERTY(EditAnywhere, Category = "Orders")
	uint32 TargetOrder;
};
