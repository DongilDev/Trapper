// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster/AI/Task/BTTask_BossRockAttack.h"

#include "Monster/BaseMonster.h"
#include "Monster/AI/BaseMonsterAIController.h"
#include "Monster/Boss/Boss_Phase1.h"
#include "Monster/Boss/Boss_Phase2.h"

EBTNodeResult::Type UBTTask_BossRockAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	TObjectPtr<ABoss_Phase1> Monster1 = Cast<ABoss_Phase1>(OwnerComp.GetAIOwner()->GetPawn());
	TObjectPtr<ABoss_Phase2> Monster2 = Cast<ABoss_Phase2>(OwnerComp.GetAIOwner()->GetPawn());

	if (IsValid(Monster1))
	{
		Monster1->ServerRPCRockAttack();
		Monster1->MonsterState = EMonsterStateType::IsAttackStart;
	}

	if (IsValid(Monster2))
	{
		Monster2->ServerRPCRockAttack();
		Monster2->MonsterState = EMonsterStateType::IsAttackStart;
	}

	return EBTNodeResult::Type();
}
