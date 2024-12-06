// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster/AI/Task/BTTask_AttackBossCDC.h"


#include "Monster/BaseMonster.h"
#include "Monster/AI/BaseMonsterAIController.h"
#include "Monster/Boss/Boss_Phase2.h"
#include "Monster/Weapon/BossThrowSword.h"

EBTNodeResult::Type UBTTask_AttackBossCDC::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	TObjectPtr<ABoss_Phase2>Monster = Cast<ABoss_Phase2>(OwnerComp.GetAIOwner()->GetPawn());

	if (IsValid(Monster))
	{
		//플레이어가 공격 범위 내에 들어오면
		if (Monster->MonsterState == EMonsterStateType::IsCloseAttack ||
			Monster->MonsterState == EMonsterStateType::IsTowerAttack ||
			Monster->MonsterState == EMonsterStateType::IsTrapDetect)
		{
			if (IsValid(Monster->AttackCDCMontage))
			{
				Monster->ServerRPCCDCAttack();
				Monster->MonsterState = EMonsterStateType::IsAttackStart;
			}
		}
	}

	return EBTNodeResult::Type();
}
