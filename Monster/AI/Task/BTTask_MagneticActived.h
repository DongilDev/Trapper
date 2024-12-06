// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_Wait.h"
#include "BTTask_MagneticActived.generated.h"

/**
 * 
 */
UCLASS()
class TRAPPERPROJECT_API UBTTask_MagneticActived : public UBTTask_Wait
{
	GENERATED_BODY()

public:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMenory) override;

public:
	TObjectPtr<class ABaseMonster> Monster;
};
