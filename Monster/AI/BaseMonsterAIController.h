// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "BaseMonsterAIController.generated.h"

/**
 * 
 */
UCLASS()
class TRAPPERPROJECT_API ABaseMonsterAIController : public AAIController
{
	GENERATED_BODY()
	
public:
	ABaseMonsterAIController(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void BeginPlay() override;

private:
	void ActorCast();

	static bool ComparePathTargets(const class APathTarget& A, const class APathTarget& B);

private:
	/// ----------------- AI -------------------
	UPROPERTY(EditAnywhere)
	TObjectPtr<class UBehaviorTree> AIBehavior;

public:
	///GameState
	TObjectPtr<class ATrapperGameMode> TrapperGameMode;
	TObjectPtr<class ATrapperGameState> TrapperGameState;

	/// Player
	TObjectPtr<class ATrapperPlayerController> Player1PController;
	TObjectPtr<class ATrapperPlayerController> Player2PController;

	TObjectPtr<class APawn> PlayerPawn1P;
	TObjectPtr<class APawn> PlayerPawn2P;

	/// Tower
	TObjectPtr<class ATower> Tower;

	/// PathTarget
	TArray<TObjectPtr<class APathTarget>> PathTargets;

};


