// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster/AI/DebufferMonsterAIController.h"

#include "Kismet/GameplayStatics.h"
#include "EngineUtils.h"
#include "BehaviorTree/BlackboardComponent.h"

#include "Monster/DebufferPathTarget.h"
#include "Monster/Debuffer.h"

void ADebufferMonsterAIController::BeginPlay()
{
	Super::BeginPlay();

	//Debuffer Path Target 찾기
	for (TActorIterator<AActor> MyActor(GetWorld()); MyActor; ++MyActor)
	{
		if (MyActor->ActorHasTag("DebufferPath"))
		{
			AActor* CurrentActor = *MyActor;
			DebufferTargets.Add(Cast<ADebufferPathTarget>(CurrentActor));

		}
	}

	//DebufferPathTarget의 순서대로 Sorting
	DebufferTargets.Sort(CompareDebufferPathTargets);

	TObjectPtr<ADebuffer> OwnerMonster = Cast<ADebuffer>(GetPawn());

	if (IsValid(OwnerMonster))
	{
		//Monster내의 TargetCheck하는 곳 크기만큼 만들어주기
		//첫번째 도착지점 설정해주기
		if (DebufferTargets.Num() > 0)
		{
			OwnerMonster->NextReachTarget = DebufferTargets[0]->GetActorLocation();
			OwnerMonster->LastTargetNumber = DebufferTargets.Num();
			OwnerMonster->CurrentTarget = 0;
		}

		//Blackboard에 필요한 변수들 선언해주기
		if (PathTargets.Num() > 0)
		{
			GetBlackboardComponent()->SetValueAsVector(TEXT("BufferTargetLocation"), DebufferTargets[OwnerMonster->CurrentTarget]->GetActorLocation());
		}
	}

	
}

void ADebufferMonsterAIController::ClearTarget()
{
	for (int i = 0; i < DebufferTargets.Num(); i++)
	{
		DebufferTargets[i]->CheckTarget.Remove(Cast<ADebuffer>(GetPawn()));
		//Cast<ADebuffer>(GetPawn())->bIsLastTarget = false;
	}
	//UE_LOG(LogTemp, Warning, TEXT("Work?"));
}

bool ADebufferMonsterAIController::CompareDebufferPathTargets(const ADebufferPathTarget& A, const ADebufferPathTarget& B)
{
	return A.TargetOrder < B.TargetOrder; 
}
