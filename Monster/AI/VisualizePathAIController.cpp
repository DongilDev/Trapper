// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster/AI/VisualizePathAIController.h"
#include "NavigationSystem.h"
#include "NavigationPath.h"
#include "DrawDebugHelpers.h"
#include "Tower/Tower.h"
#include "GameElement/PathArrow.h"

void AVisualizePathAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

    // 현재 경로 초기화
    CurrentPath = nullptr;
}

void AVisualizePathAIController::VisualizePath()
{
    // 경로를 업데이트
    if (APawn* MyPawn = GetPawn())
    {
        FVector Destination = FVector(Tower->GetActorLocation()); // 목적지를 원하는 위치로 설정
        CurrentPath = UNavigationSystemV1::FindPathToLocationSynchronously(this, MyPawn->GetActorLocation(), Destination);

        if (CurrentPath && CurrentPath->IsValid())
        {
            for (int32 i = 0; i < CurrentPath->PathPoints.Num() - 1; i++)
            {
                FVector Start = CurrentPath->PathPoints[i];
                FRotator StartRotator = FRotator::ZeroRotator;
                FVector End = CurrentPath->PathPoints[i + 1];
                //DrawDebugLine(GetWorld(), Start, End, FColor::Green, false, -1.f, 0, 5.f);
                // DrawArrow = GetWorld()->SpawnActor<APathArrow>(PathArrow, Start, StartRotator);

                //Path Arrow Spawn

            }
        }
    }

}

void AVisualizePathAIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Update and visualize the path
	VisualizePath();
}
