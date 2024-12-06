// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WaveDataTables.generated.h"

USTRUCT()
struct FWaveInfo : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()
public:
	FWaveInfo()
	: bLastSubWave()
	, bLastLargeWave()
	, Skeleton()
	, Mummy()
	, Zombie()
	, Debuffer()
	, Boss()
	, NextWaveLeftTime()
	, Memo()
	{}

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite) uint8 bLastSubWave : 1 = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) uint8 bLastLargeWave : 1 = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int32 Skeleton;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int32 Mummy;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int32 Zombie;

	UPROPERTY(EditAnywhere, BlueprintReadWrite) int32 Debuffer;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int32 Boss;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float NextWaveLeftTime;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FText Memo;
};

UCLASS()
class TRAPPERPROJECT_API AWaveDataTables : public AActor
{
	GENERATED_BODY()
};
