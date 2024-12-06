// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster/AI/Task/BTTask_AttackBossTrap.h"

#include "Monster/BaseMonster.h"
#include "Monster/AI/BaseMonsterAIController.h"
#include "Monster/Boss/Boss_Phase1.h"
#include "Monster/Boss/Boss_Phase2.h"

EBTNodeResult::Type UBTTask_AttackBossTrap::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	TObjectPtr<ABoss_Phase1> Monster1 = Cast<ABoss_Phase1>(OwnerComp.GetAIOwner()->GetPawn());
	TObjectPtr<ABoss_Phase2> Monster2 = Cast<ABoss_Phase2>(OwnerComp.GetAIOwner()->GetPawn());

	if (IsValid(Monster1))
	{
		//플레이어가 공격 범위 내에 들어오면
		if (Monster1->MonsterState == EMonsterStateType::IsTrapAttack ||
			Monster1->MonsterState == EMonsterStateType::IsTowerAttack)
		{
			if (IsValid(Monster1->AttackTrapMontage))
			{
				Monster1->ServerRPCTrapAttack();
				Monster1->MonsterState = EMonsterStateType::IsAttackStart;
			}
		}
	}
	else if(IsValid(Monster2))
	{
		//플레이어가 공격 범위 내에 들어오면
		if (Monster2->MonsterState == EMonsterStateType::IsTrapAttack ||
			Monster2->MonsterState == EMonsterStateType::IsTowerAttack)
		{
			if (IsValid(Monster2->AttackTrapMontage))
			{
				Monster2->ServerRPCTrapAttack();
				Monster2->MonsterState = EMonsterStateType::IsAttackStart;
			}
		}
	}

	return EBTNodeResult::Type();
}
