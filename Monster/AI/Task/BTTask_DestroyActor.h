// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_DestroyActor.generated.h"

/**
 * 
 */
UCLASS()
class TRAPPERPROJECT_API UBTTask_DestroyActor : public UBTTaskNode
{
	GENERATED_BODY()

public:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMenory) override;

public:
	//GameState
	class ATrapperGameState* GameState;

};
