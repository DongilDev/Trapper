// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "UpgradeTable.generated.h"

/**
 *
 */


USTRUCT(BlueprintType)
struct FUpgradeTable : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FString ProgrammingName = "";

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FString Name = "";

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FString Description = "";

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	UTexture2D* IconTextrue = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float EffectAmount = 0.f;
};


USTRUCT(BlueprintType)
struct FPillarUpgradeTable : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FString ProgrammingName = "";

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FString Name = "";

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FString Description = "";

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	UTexture2D* IconTextrue = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	int32 EffectAmount = 0;
};