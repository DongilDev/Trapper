// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "MapMarkerType.h"
#include "TrapMarkerStruct.generated.h"

/**
 *
 */


USTRUCT(BlueprintType)
struct FTrapMarkerStruct : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	EMapTrapType Type = EMapTrapType::None;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	class UTexture2D* Texture = nullptr;
};