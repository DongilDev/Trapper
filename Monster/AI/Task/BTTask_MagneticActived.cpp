// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster/AI/Task/BTTask_MagneticActived.h"

#include "Monster/BaseMonster.h"
#include "Monster/AI/BaseMonsterAIController.h"

EBTNodeResult::Type UBTTask_MagneticActived::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMenory)
{
	

	Monster = Cast<ABaseMonster>(OwnerComp.GetAIOwner()->GetPawn());

	//받아온 위치로 이동
	//GoToPillar Task

	//2초 멈춤
	WaitTime = Monster->MonsterPillarTime;
	
	//상태 변화
	//IsStunEnd를 넣어서 Service에서 체크 할 예정
	Monster->MonsterState = EMonsterStateType::IsStunEnd;


	return EBTNodeResult::Succeeded;
}

