// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/UserDefinedEnum.h"
#include "PlayerPingType.generated.h"

UENUM(BlueprintType)
enum class EPlayerPingType : uint8
{
	E_Default1P = 0			UMETA(DisplayName = "Default 1P"),
	E_Default2P				UMETA(DisplayName = "Default 2P"),

	E_TrapInstall1P			UMETA(DisplayName = "Trap Install 1P"),
	E_TrapInstall2P			UMETA(DisplayName = "Trap Install 2P"),

	E_TrapActivate1P		UMETA(DisplayName = "Trap Activation 1P"),
	E_TrapActivate2P		UMETA(DisplayName = "Trap Activation 2P"),

	E_Warning1P				UMETA(DisplayName = "Warning 1P"),
	E_Warning2P				UMETA(DisplayName = "Warning 2P"),

	E_Assemble1P			UMETA(DisplayName = "Assemble 1P"),
	E_Assemble2P			UMETA(DisplayName = "Assemble 2P"),

	E_OtherPlayer1P			UMETA(DisplayName = "Other Player 1P"),
	E_OtherPlayer2P			UMETA(DisplayName = "Other Player 2P"),
	
	E_Quest					UMETA(DisplayName = "Quest"),	

	None					UMETA(DisplayName = "None")
};


UENUM(BlueprintType)
enum class ESelectPingType : uint8
{
	E_Default = 0			UMETA(DisplayName = "Default"),
	E_TrapInstall			UMETA(DisplayName = "Trap Install"),
	E_TrapActivate			UMETA(DisplayName = "Trap Activation"),
	E_Warning				UMETA(DisplayName = "Warning"),
	E_Assemble				UMETA(DisplayName = "Assemble"),

	None					UMETA(DisplayName = "None")
};