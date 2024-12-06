// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster/AI/Task/BTTask_SpawnArrow.h"
#include "Monster/BaseMonster.h"
#include "Monster/AI/BaseMonsterAIController.h"
#include "Monster/AI/VisualizePathAIController.h"
#include "GameElement/PathArrow.h"

EBTNodeResult::Type UBTTask_SpawnArrow::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	TObjectPtr<ABaseMonster> PathMonster = Cast<ABaseMonster>(OwnerComp.GetAIOwner()->GetPawn());
	TObjectPtr<AVisualizePathAIController> PathAI = Cast<AVisualizePathAIController>(OwnerComp.GetAIOwner());
	

	if (IsValid(PathMonster))
	{
		FVector StartVector = PathMonster->GetActorLocation();
		FRotator StartRotator = PathMonster->GetActorRotation();

		if (IsValid(PathAI))
		{
			APathArrow* PathArrow = GetWorld()->SpawnActor<APathArrow>(PathAI->PathArrow, StartVector, StartRotator);
			PathArrow->SetOwner(PathMonster);
		}

		
	}
	return EBTNodeResult::Succeeded;
}
