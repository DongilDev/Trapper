// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster/AI/Task/BTTask_GoToPillar.h"
#include "Monster/BaseMonster.h"
#include "Monster/AI/BaseMonsterAIController.h"

EBTNodeResult::Type UBTTask_GoToPillar::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMenory)
{
	FVector ActorTolerance(0.f, 0.f, 20.f);
	TObjectPtr<class ABaseMonster> Monster = Cast<ABaseMonster>(OwnerComp.GetAIOwner()->GetPawn());
	
	if (IsValid(Monster))
	{
		Monster->AnimInstance->Montage_Stop(0);

		FVector MonsterLocation = Monster->GetActorLocation();
		FVector PillarLocation = Monster->ActivedPillarLocation;
		FVector NewPosition = FMath::VInterpTo(MonsterLocation, PillarLocation, GetWorld()->GetDeltaSeconds(), Monster->MonsterPillarSpeed);

		float Tolerance = 150.f;

		if (Monster->ActorHasTag("Boss"))
		{
			Tolerance = 500.f;
		}
		
		if (MonsterLocation.Equals(PillarLocation, Tolerance))
		{
			Monster->MonsterState = EMonsterStateType::IsStun;
		}

		Monster->SetActorLocation(NewPosition);
	}

	return EBTNodeResult::Succeeded;
}
