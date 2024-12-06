// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster/AI/Task/BTTask_AttackADC.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Net/UnrealNetwork.h"

#include "Monster/BaseMonster.h"
#include "Monster/AI/BaseMonsterAIController.h"
#include "Monster/ADC.h"
#include "Monster/Weapon/Wand.h"
#include "Monster/Weapon/MagicBall.h"

EBTNodeResult::Type UBTTask_AttackADC::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Monster = Cast<AADC>(OwnerComp.GetAIOwner()->GetPawn());

	if (IsValid(Monster))
	{
		//플레이어가 공격 범위 내에 들어오면
		if (Monster->MonsterState == EMonsterStateType::IsAttack || 
			Monster->MonsterState == EMonsterStateType::IsTowerAttack)
		{
			if (IsValid(Monster->AttackkActionMontage1) && IsValid(Monster->AttackkActionMontage2))
			{
				Monster->AttackADCMontage();
				Monster->MonsterState = EMonsterStateType::IsAttackStart;
			}
		}

	
	}
	
	return EBTNodeResult::Succeeded;
}

