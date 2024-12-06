// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster/AI/Service/BTService_SpawnArrow.h"
#include "Monster/BaseMonster.h"
#include "Monster/AI/BaseMonsterAIController.h"
#include "Monster/AI/VisualizePathAIController.h"
#include "GameElement/PathArrow.h"

void UBTService_SpawnArrow::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	TObjectPtr<ABaseMonster> PathMonster = Cast<ABaseMonster>(OwnerComp.GetAIOwner()->GetPawn());
	TObjectPtr<AVisualizePathAIController> PathAI = Cast<AVisualizePathAIController>(OwnerComp.GetAIOwner());


	if (IsValid(PathMonster))
	{
		FVector StartVector = PathMonster->GetActorLocation();
		StartVector.Z = StartVector.Z - 40.f;
		FRotator StartRotator = PathMonster->GetActorRotation();

		if (IsValid(PathAI))
		{
			APathArrow* PathArrow = GetWorld()->SpawnActor<APathArrow>(PathAI->PathArrow, StartVector, StartRotator);
			PathArrow->SetOwner(PathMonster);
		}


	}


}
