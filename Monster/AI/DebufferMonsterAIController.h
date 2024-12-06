// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Monster/AI/BaseMonsterAIController.h"
#include "DebufferMonsterAIController.generated.h"

/**
 * 
 */
UCLASS()
class TRAPPERPROJECT_API ADebufferMonsterAIController : public ABaseMonsterAIController
{
	GENERATED_BODY()
	
protected:
	virtual void BeginPlay() override;

public:
	void ClearTarget();

	static bool CompareDebufferPathTargets(const class ADebufferPathTarget& A, const class ADebufferPathTarget& B);

public:
	//PathTarget
	TArray<TObjectPtr<class ADebufferPathTarget>> DebufferTargets;
};
