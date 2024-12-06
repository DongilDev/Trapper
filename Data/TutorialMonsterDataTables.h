// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TutorialMonsterDataTables.generated.h"

USTRUCT()
struct FTutorialMonsterInfo : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

public:
	FTutorialMonsterInfo()
	: MonsterPosition()
	, MonsterRotation()
	{}

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FVector MonsterPosition;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FRotator MonsterRotation;
};

UCLASS()
class TRAPPERPROJECT_API ATutorialMonsterDataTables : public AActor
{
	GENERATED_BODY()
};
