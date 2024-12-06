// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HitIndicatorComponent.generated.h"


USTRUCT()
struct FHitIndicatorData
{
	GENERATED_BODY()

	UPROPERTY()
	TObjectPtr<class ABaseMonster> Monster;

	UPROPERTY()
	TObjectPtr<class UDirectionalDamageIndicatorHUD> HUD;
	float LifeTime = 0.f;

	FHitIndicatorData()
		: Monster(nullptr), HUD(nullptr), LifeTime(0)
	{
	}

	FHitIndicatorData(ABaseMonster* MonsterRef, UDirectionalDamageIndicatorHUD* HUDRef)
		: Monster(MonsterRef), HUD(HUDRef), LifeTime(0)
	{
	}
};


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TRAPPERPROJECT_API UHitIndicatorComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UHitIndicatorComponent();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:	
	UPROPERTY(EditDefaultsOnly, Category = "HUD")
	TSubclassOf<class UDirectionalDamageIndicatorHUD> IndicatorHUDClass;

	void OnPlayerHit(class ABaseMonster* Monster);

private:
	UPROPERTY() 
	TObjectPtr<class ATrapperPlayerController> PlayerController = nullptr;

	UPROPERTY()
	TObjectPtr<class ATrapperPlayer> Player = nullptr;

	UPROPERTY()
	TMap<int32, FHitIndicatorData> Indicators;
	
	UPROPERTY()
	TArray<int32> ForDeletes;

	FTimerHandle UpdateTimer;
	void UpdateIndicator();
	bool bIsUpdating = false;
	float UpdateTime = 0.01f;
};
