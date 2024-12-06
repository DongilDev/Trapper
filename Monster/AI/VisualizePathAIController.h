// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Monster/AI/BaseMonsterAIController.h"
#include "VisualizePathAIController.generated.h"

/**
 * 
 */
UCLASS()
class TRAPPERPROJECT_API AVisualizePathAIController : public ABaseMonsterAIController
{
	GENERATED_BODY()
	
public:
	virtual void OnPossess(APawn* InPawn) override;

	void VisualizePath();

protected:
	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY()
	UNavigationPath* CurrentPath;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Arrow")
	TSubclassOf<class APathArrow> PathArrow;
};
