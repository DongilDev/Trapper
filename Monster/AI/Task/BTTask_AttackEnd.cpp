// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster/AI/Task/BTTask_AttackEnd.h"

#include "Monster/BaseMonster.h"
#include "Monster/AI/BaseMonsterAIController.h"

EBTNodeResult::Type UBTTask_AttackEnd::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMenory)
{

	TObjectPtr<class ABaseMonster> Monster = Cast<ABaseMonster>(OwnerComp.GetAIOwner()->GetPawn());

	if (Monster->bIsAttackEnd == true)
	{
		UAnimInstance* AnimInstance = Monster->GetMesh()->GetAnimInstance();
		AnimInstance->StopAllMontages(0.0f);

		Monster->MonsterState = EMonsterStateType::GoToTower;
		//UE_LOG(LogTemp, Warning, TEXT("AttackEnd Go To Tower"));
	}

	return EBTNodeResult::Succeeded;
}
