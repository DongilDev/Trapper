// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "OpeningGS.generated.h"

/**
 * 
 */
UCLASS()
class TRAPPERPROJECT_API AOpeningGS : public AGameStateBase
{
	GENERATED_BODY()
public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;


	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, ReplicatedUsing = "OnRep_SkipCount", Category = "Network")
	int SkipCount = 0;

	UFUNCTION()
	void OnRep_SkipCount();
};
