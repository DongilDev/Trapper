// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster/AI/Task/BTTask_AttackBossADC.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Net/UnrealNetwork.h"

#include "Monster/BaseMonster.h"
#include "Monster/AI/BaseMonsterAIController.h"
#include "Monster/Boss/Boss_Phase1.h"
#include "Monster/Boss/Boss_Phase2.h"
#include "Monster/Weapon/BossThrowSword.h"

EBTNodeResult::Type UBTTask_AttackBossADC::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	TObjectPtr<ABoss_Phase1>MonsterA = Cast<ABoss_Phase1>(OwnerComp.GetAIOwner()->GetPawn());
	TObjectPtr<ABoss_Phase2>MonsterB = Cast<ABoss_Phase2>(OwnerComp.GetAIOwner()->GetPawn());

	if (IsValid(MonsterA))
	{
		//플레이어가 공격 범위 내에 들어오면
		if (MonsterA->MonsterState == EMonsterStateType::IsAttack ||
			MonsterA->MonsterState == EMonsterStateType::IsTowerAttack)
		{
			if (IsValid(MonsterA->AttackADCMontage))
			{
				//MonsterA->bIsADCAttackState = false;
				MonsterA->ServerRPCADCAttack();
				MonsterA->MonsterState = EMonsterStateType::IsAttackStart;
			}
		}
	}

	if (IsValid(MonsterB))
	{
		//플레이어가 공격 범위 내에 들어오면
		if (MonsterB->MonsterState == EMonsterStateType::IsAttack ||
			MonsterB->MonsterState == EMonsterStateType::IsTowerAttack)
		{
			if (IsValid(MonsterB->AttackADCMontage))
			{
				//MonsterB->bIsADCAttackState = false;
				MonsterB->ServerRPCADCAttack();
				MonsterB->MonsterState = EMonsterStateType::IsAttackStart;
			}
		}
	}

	return EBTNodeResult::Type();
}
