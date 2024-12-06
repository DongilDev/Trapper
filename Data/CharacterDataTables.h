// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CharacterDataTables.generated.h"

UENUM()
enum class ECharacterData : uint8
{
	Default			UMETA(DisplayName = "Default"),
	Upgrade			UMETA(DisplayName = "Upgrade"),
};

USTRUCT()
struct FCharacterInfo : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

public:
	FCharacterInfo()
		: Level()
		, MoveSpeed()
		, MaxHealth()
		, HealthCoolTime()
		, RecoveryDelayTime()
		, HealthRecoveryAmount()
		, ChargeTime()
		, ArrowMinSpeed()
		, ArrowMaxSpeed()
		, NormalArrowMinDamage()
		, NormalArrowMaxDamage()
		, MagneticArrowMinDamage()
		, MagneticArrowMaxDamage()
		, MagneticMoveSpeed()
		, StartMoney()

		, SlowReductionRate()
		, InvincibilityTime()
		, RevivalTime()
	{};

	// 레벨
	UPROPERTY(EditAnywhere, BlueprintReadWrite) uint8 Level;

	// 이동 속도
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float MoveSpeed;

	// 최대 체력
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float MaxHealth;

	// 체력 회복 쿨타임
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float HealthCoolTime;

	// 회복이 시작되는 시간
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float RecoveryDelayTime;

	// 체력 회복량
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float HealthRecoveryAmount;

	// 차지 시간
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float ChargeTime;

	// 화살 최소 투사체 속도
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float ArrowMinSpeed;

	// 화살 최대 투사체 속도
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float ArrowMaxSpeed;

	// 일반 화살 최소 공격력
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float NormalArrowMinDamage;

	// 일반 화살 최대 공격력
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float NormalArrowMaxDamage;

	// 자성 화살 최소 공격력
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float MagneticArrowMinDamage;

	// 자성 화살 최대 공격력
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float MagneticArrowMaxDamage;

	// 자성 이동 속도
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float MagneticMoveSpeed;

	// 최초 보유 재화
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int32 StartMoney;

	// 슬로우 이동 속도 감소율
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float SlowReductionRate;

	// 피격 후 무적 시간
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float InvincibilityTime;

	// 부활 시간
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float RevivalTime;
};

UCLASS()
class TRAPPERPROJECT_API ACharacterDataTables : public AActor
{
	GENERATED_BODY()
};
