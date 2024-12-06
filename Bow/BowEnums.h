// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/UserDefinedEnum.h"
#include "BowEnums.generated.h"

UENUM(BlueprintType)
enum class EBowDrawState : uint8
{
	E_Idle = 0		UMETA(DisplayName = "Idle"),
	E_Draw			UMETA(DisplayName = "Draw"),
};



UENUM() 
enum class EArrowType : uint8
{
	Default		UMETA(DisplayName = "Default"),
	Magnetic	UMETA(DisplayName = "Magnetic"),
};



UENUM() 
enum class EArrowSocketType : uint8
{
	None		UMETA(DisplayName = "None"),
	RightHand	UMETA(DisplayName = "RightHand"),
	Nocked		UMETA(DisplayName = "Nocked"),
};