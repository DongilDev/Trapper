// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "MyBTService_CheckState.generated.h"

/**
 * 
 */
UCLASS()
class TRAPPERPROJECT_API UMyBTService_CheckState : public UBTService
{
	GENERATED_BODY()
	

public:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

public:
	TObjectPtr<class ATrapperPlayerController> Player1PController;
	TObjectPtr<class ATrapperPlayerController> Player2PController;

	TObjectPtr<class APawn> PlayerPawn1P;
	TObjectPtr<class APawn> PlayerPawn2P;

	TObjectPtr<class ATower> Tower;
	TObjectPtr<class ATurret> Turret;

	TArray<TObjectPtr<class APathTarget>> PathTargets;
	TArray<TObjectPtr<class ADebufferPathTarget>> DebufferTargets;

private:
	uint32 DetectCheckTime = 0;
	uint32 DebufferTime = 0;
	

	TObjectPtr<class ABaseMonsterAIController> OwnerMonsterAI;
	TObjectPtr<class ADebufferMonsterAIController> DebufferMonsterAIController;
	void ClearDebufferTarget();

	void CheckDebuffeState();

private:
	uint8 bIsStop : 1 = false;
	uint8 bIsStuck : 1 = false;
	uint8 bIsTimerStart : 1 = false;
	uint32 StuckCount = 0;
	float MonsterMovingCheckTime = 0;
	

	FVector MonsterStartLocation;
	FVector MonsterCurrentLocation;
	float MonsterMovingDistance = 0;
	float MonsterBeforeMovingDistance = 0;

};
