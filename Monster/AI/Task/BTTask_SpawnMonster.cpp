// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster/AI/Task/BTTask_SpawnMonster.h"

#include "Monster/AI/BaseMonsterAIController.h"
#include "Monster/Boss/Boss_Phase2.h"
EBTNodeResult::Type UBTTask_SpawnMonster::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	TObjectPtr<ABoss_Phase2>Monster = Cast<ABoss_Phase2>(OwnerComp.GetAIOwner()->GetPawn());

	if (IsValid(Monster))
	{
		//플레이어가 공격 범위 내에 들어오면
		if (Monster->MonsterState == EMonsterStateType::IsMonsterSpawn)
		{
			if (IsValid(Monster->SpawnMonsterMontage))
			{
				Monster->ServerRPCSpawnMonster();
				Monster->MonsterState = EMonsterStateType::IsAttackStart;
			}
		}
	}

	return EBTNodeResult::Type();
}
