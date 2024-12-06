// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MonsterDataTables.generated.h"

UENUM()
enum class EMonsterName : uint8
{
	Skeleton	UMETA(DisplayName = "Skeleton"),
	Mummy		UMETA(DisplayName = "Mummy"),
	Zombie		UMETA(DisplayName = "Zombie"),
	Debuffer	UMETA(DisplayName = "Debuffer"),
	BossPhase1	UMETA(DisplayName = "BossPhase1"),
	BossPhase2	UMETA(DisplayName = "BossPahse2")

};

USTRUCT()
struct FMonsterInfo : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

public:
	FMonsterInfo()
		: Monster(EMonsterName::Skeleton)
		, HP(150.f)
		, Attack(50.f)
		, HeadDamage(2.f)
		, NomalDamage(1.f)
		, MoveSpeed(600.f)
		, ChaseSpeed(800.f)
		, LocationTimer(5.f)
		, EffectTimer(10.f)
		, EffectSpeed(800.f)
		, SpawnAmount(5.f)
		, WideCoolTime(3.f)
		, ComboCoolTime(3.f)
		, TrapCoolTime(10.f)
		, ADCCoolTime(20.f)
		, SkillCoolTime1(3.f)
		, SkillCoolTime2(7.f)
		, PillarSpeed(1.f)
		, PillarStunTime(5.f)
	{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite) EMonsterName Monster = EMonsterName::Skeleton;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float HP = 150.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float Attack = 50.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float HeadDamage = 2.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)	float NomalDamage = 1.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float MoveSpeed = 600.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float ChaseSpeed = 800.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float LocationTimer = 5.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float EffectTimer = 10.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float EffectSpeed = 800.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float SpawnAmount = 5.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float WideCoolTime = 3.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float ComboCoolTime = 3.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float TrapCoolTime = 20.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float CDCCoolTime  = 20.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float ADCCoolTime  = 20.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float SkillCoolTime1 = 3.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float SkillCoolTime2 = 7.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float PillarSpeed = 1.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float PillarStunTime = 5.f;

};

UCLASS()
class TRAPPERPROJECT_API AMonsterTables : public AActor
{
	GENERATED_BODY()
};
