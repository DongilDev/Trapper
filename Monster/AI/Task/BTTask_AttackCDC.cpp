// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster/AI/Task/BTTask_AttackCDC.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Monster/BaseMonster.h"
#include "Monster/AI/BaseMonsterAIController.h"
#include "Monster/CDC.h"


EBTNodeResult::Type UBTTask_AttackCDC::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	TObjectPtr<ACDC> Monster = Cast<ACDC>(OwnerComp.GetAIOwner()->GetPawn());

	if (IsValid(Monster))
	{
		//플레이어가 공격 범위 내에 들어오면
		if (Monster->MonsterState == EMonsterStateType::IsAttack || Monster->MonsterState == EMonsterStateType::IsTowerAttack)
		{
			if (IsValid(Monster->AttackkActionMontage1) && IsValid(Monster->AttackkActionMontage2) && Monster->MonsterState != EMonsterStateType::IsAttackStart)
			{
				Monster->AttackCDCMontage();
				Monster->MonsterState = EMonsterStateType::IsAttackStart;
			}
		}
	}

	return EBTNodeResult::Succeeded;
}
