// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Monster/CDC.h"
#include "TutorialMonster.generated.h"

/**
 * 
 */
UCLASS()
class TRAPPERPROJECT_API ATutorialMonster : public ACDC
{
	GENERATED_BODY()
	
protected:
	virtual void BeginPlay() override;

public:
	void Teleport(FVector Location);
};
