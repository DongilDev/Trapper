// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseMonsterAIController.h"

#include "Kismet/GameplayStatics.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "EngineUtils.h"
#include "Navigation/CrowdFollowingComponent.h"

#include "Framework/TrapperGameState.h"
#include "Framework/TrapperGameMode.h"
#include "Framework/TrapperPlayerController.h"
#include "Tower/Tower.h"
#include "Monster/BaseMonster.h"
#include "Monster/PathMonster.h"
#include "Character/TrapperPlayer.h"
#include "Monster/PathTarget.h"
#include "Monster/Boss/Boss_Phase1.h"
#include "Monster/Boss/Boss_Phase2.h"
#include "GameElement/Arch.h"

ABaseMonsterAIController::ABaseMonsterAIController(const FObjectInitializer& ObjectInitializer)
	//:Super(ObjectInitializer.SetDefaultSubobjectClass<UCrowdFollowingComponent>(TEXT("PathFollowingComponent")))
{

}

void ABaseMonsterAIController::BeginPlay()
{
	Super::BeginPlay();

	////CrowdFollowingComponent
	//UCrowdFollowingComponent* CrowdFollowingComponent = FindComponentByClass<UCrowdFollowingComponent>();

	//if (CrowdFollowingComponent)
	//{
	//	CrowdFollowingComponent->SetCrowdSeparation(true);
	//	CrowdFollowingComponent->SetCrowdSeparationWeight(50.f);
	//	CrowdFollowingComponent->SetCrowdAvoidanceRangeMultiplier(1.1f);
	//}

	//미리 필요한 actor들 캐스팅 해두기
	ActorCast();
	
	//this->GetPathFollowingComponent()->Deactivate();


	if (AIBehavior != nullptr)
	{
		RunBehaviorTree(AIBehavior);

		//UE_LOG(LogTemp, Warning, TEXT("RunBehaviorTree"));

		//Blackboard에 필요한 변수들 선언해주기
		if (IsValid(Tower))
		{	
			GetBlackboardComponent()->SetValueAsVector(TEXT("TowerLocation"), Tower->GetActorLocation());
		}

		TObjectPtr<ABaseMonster> OwnerMonster = Cast<ABaseMonster>(GetPawn());
		if (IsValid(OwnerMonster))
		{
			if (PathTargets.Num() > 0)
			{
				GetBlackboardComponent()->SetValueAsVector(TEXT("TargetLocation"), PathTargets[OwnerMonster->CurrentTarget]->GetActorLocation());
			}

			//Stun Time 지정
			GetBlackboardComponent()->SetValueAsFloat(TEXT("StunTime"), OwnerMonster->MonsterPillarTime);
		}
	}


}

void ABaseMonsterAIController::ActorCast()
{
	//UE_LOG(LogTemp, Warning, TEXT("Actor Cast"));
	//GameMode
	TrapperGameMode = Cast<ATrapperGameMode>(UGameplayStatics::GetGameMode(GetWorld()));

	//GamState
	TrapperGameState = Cast<ATrapperGameState>(UGameplayStatics::GetGameState(GetWorld()));

	for (TActorIterator<AActor> MyActor(GetWorld()); MyActor; ++MyActor)
	{
		//UE_LOG(LogTemp, Warning, TEXT("Iterator Check"));

		if (MyActor->ActorHasTag("Tower"))
		{
			AActor* CurrentActor = *MyActor;
			Tower = Cast<ATower>(CurrentActor);
			//UE_LOG(LogTemp, Warning, TEXT("Find Tower"));
		}

		if (MyActor->ActorHasTag("Path"))
		{
			AActor* CurrentActor = *MyActor;
			PathTargets.Add(Cast<APathTarget>(CurrentActor));
			//UE_LOG(LogTemp, Warning, TEXT("Find TargetPath"));
		}
	}

	TObjectPtr<ABaseMonster> OwnerMonster = Cast<ABaseMonster>(GetPawn());
	TObjectPtr<APathMonster> PathMonster = Cast<APathMonster>(GetPawn());
	TObjectPtr<ABoss_Phase1> Boss_Phase1 = Cast<ABoss_Phase1>(GetPawn());
	TObjectPtr<ABoss_Phase2> Boss_Phase2 = Cast<ABoss_Phase2>(GetPawn());

	PathTargets.Sort(ComparePathTargets);

	if (IsValid(OwnerMonster))
	{
		//Monster내의 TargetCheck하는 곳도 크기만큼 만들어주기		
		//첫번째 도착지점 설정
		if (PathTargets.Num() > 0)
		{
			OwnerMonster->NextReachTarget = PathTargets[0]->GetRandomReachPoint();

			if (PathMonster)
			{
				PathMonster->NextReachTarget = PathTargets[0]->SubTargetLocation3;
			}

			if (Boss_Phase1)
			{
				Boss_Phase1->NextReachTarget = PathTargets[0]->BossTargetLocation;
				//UE_LOG(LogTemp, Warning, TEXT("Boss_Phase1 Target"));
			}

			if (Boss_Phase2)
			{
				Boss_Phase2->NextReachTarget = PathTargets[0]->BossTargetLocation;
			}

			OwnerMonster->LastTargetNumber = PathTargets.Num();
			OwnerMonster->CurrentTarget = 0;
			//UE_LOG(LogTemp, Warning, TEXT("%f"), OwnerMonster->CurrentTarget);
		}
		OwnerMonster->MonsterState = EMonsterStateType::GoToTower;
		//UE_LOG(LogTemp, Warning, TEXT("GoToTower : PathTarget BeginPlay"));
	}
}

bool ABaseMonsterAIController::ComparePathTargets(const APathTarget& A, const APathTarget& B)
{
	return A.TargetOrders < B.TargetOrders;
}

