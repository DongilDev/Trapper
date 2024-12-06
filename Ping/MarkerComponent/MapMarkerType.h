#pragma once

#include "CoreMinimal.h"
#include "Engine/UserDefinedEnum.h"
#include "MapMarkerType.generated.h"


UENUM(BlueprintType)
enum class EMapObjectType : uint8
{
	Player		UMETA(DisplayName = "Player"),
	Monster		UMETA(DisplayName = "Monster"),
	Ping1P		UMETA(DisplayName = "Ping1P"),
	Ping2P		UMETA(DisplayName = "Ping2P"),
	Pillar		UMETA(DisplayName = "Pillar"),
	Temple		UMETA(DisplayName = "Temple"),
	LockedPillar		UMETA(DisplayName = "LockedPillar"),

	E_TrapInstall1P		UMETA(DisplayName = "TrapInstall1P"),
	E_TrapInstall2P		UMETA(DisplayName = "TrapInstall2P"),
	E_TrapActivate1P	UMETA(DisplayName = "TrapActivate1P"),
	E_TrapActivate2P	UMETA(DisplayName = "TrapActivate2P"),
	E_Warning1P			UMETA(DisplayName = "Warning1P"),
	E_Warning2P			UMETA(DisplayName = "Warning2P"),
	E_Assemble1P		UMETA(DisplayName = "Assemble1P"),
	E_Assemble2P		UMETA(DisplayName = "Assemble2P"),

	None		UMETA(DisplayName = "None")
};

UENUM(BlueprintType)
enum class EMapTrapType : uint8
{
	BearAlive				UMETA(DisplayName = "Bear Alive"),
	BearDisable				UMETA(DisplayName = "Bear Disable"),
	BearWreckage			UMETA(DisplayName = "Bear Wreckage"),
	BearWreckageSelect		UMETA(DisplayName = "Bear Wreckage Select"),

	BarrelAlive				UMETA(DisplayName = "Barrel Alive"),
	BarrelDisable			UMETA(DisplayName = "Barrel Disable"),
	BarrelWreckage			UMETA(DisplayName = "Barrel Wreckage"),
	BarrelWreckageSelect	UMETA(DisplayName = "Barrel Wreckage Select"),

	SpikeAlive				UMETA(DisplayName = "Spike Alive"),
	SpikeDisable			UMETA(DisplayName = "Spike Disable"),
	SpikeWreckage			UMETA(DisplayName = "Spike Wreckage"),
	SpikeWreckageSelect		UMETA(DisplayName = "Spike Wreckage Select"),

	SpearAlive				UMETA(DisplayName = "Spear Alive"),
	SpearDisable			UMETA(DisplayName = "Spear Disable"),
	SpearWreckage			UMETA(DisplayName = "Spear Wreckage"),
	SpearWreckageSelect		UMETA(DisplayName = "Spear Wreckage Select"),

	PlankAlive				UMETA(DisplayName = "Plank Alive"),
	PlankDisable			UMETA(DisplayName = "Plank Disable"),
	PlankWreckage			UMETA(DisplayName = "Plank Wreckage"),
	PlankWreckageSelect		UMETA(DisplayName = "Plank Wreckage Select"),

	GuillotineAlive			UMETA(DisplayName = "Guillotine Alive"),
	GuillotineDisable		UMETA(DisplayName = "Guillotine Disable"),
	GuillotineWreckage		UMETA(DisplayName = "Guillotine Wreckage"),
	GuillotineWreckageSelect	UMETA(DisplayName = "Guillotine WreckageSelect"),

	OilBagAlive				UMETA(DisplayName = "OilBag Alive"),
	OilBagDisable			UMETA(DisplayName = "OilBag Disable"),
	OilBagWreckage			UMETA(DisplayName = "OilBag Wreckage"),
	OilBagWreckageSelect	UMETA(DisplayName = "OilBag WreckageSelect"),

	ClapAlive				UMETA(DisplayName = "Clap Alive"),
	ClapDisable				UMETA(DisplayName = "Clap Disable"),
	ClapWreckage			UMETA(DisplayName = "Clap Wreckage"),
	ClapWreckageSelect		UMETA(DisplayName = "Clap WreckageSelect"),

	TurretAlive				UMETA(DisplayName = "Turret Alive"),
	TurretDisable			UMETA(DisplayName = "Turret Disable"),
	TurretWreckage			UMETA(DisplayName = "Turret Wreckage"),
	TurretWreckageSelect	UMETA(DisplayName = "Turret WreckageSelect"),

	None					UMETA(DisplayName = "None")
};
