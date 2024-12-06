#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "MapMarkerType.h"
#include "MapMarkerStruct.generated.h"

/**
 *
 */
USTRUCT(BlueprintType)
struct FMapMarkerStruct : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	EMapObjectType Type = EMapObjectType::None;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	class UTexture2D* Texture = nullptr;
};