// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster/AI/Task/BTTask_BossPhaseChange.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"

#include "Monster/BaseMonster.h"
#include "Monster/AI/BaseMonsterAIController.h"
#include "Monster/Boss/Boss_Phase1.h"
#include "Monster/Boss/Boss_Phase2.h"
#include "Framework/TrapperGameMode.h"
#include "GameElement/Spawner.h"

EBTNodeResult::Type UBTTask_BossPhaseChange::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMenory)
{
	Super::ExecuteTask(OwnerComp, NodeMenory);
	
	TObjectPtr<ABoss_Phase1>MonsterA = Cast<ABoss_Phase1>(OwnerComp.GetAIOwner()->GetPawn());

	if (IsValid(MonsterA) && MonsterA->MonsterState != EMonsterStateType::IsPhaseChanging)
	{
		MonsterA->MonsterState = EMonsterStateType::IsPhaseChanging;
		MonsterA->ServerRPCPhaseChange();
		//UE_LOG(LogTemp, Warning, TEXT("Animation Check"));
	}

	return EBTNodeResult::Type();
}
