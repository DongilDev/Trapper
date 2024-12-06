// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MonsterHealthBar.generated.h"

/**
 * 
 */
UCLASS()
class TRAPPERPROJECT_API UMonsterHealthBar : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	void UpdateHealth(float Health, float MaxHealth, float VisibleTime);
	void OnPlayerSight(float Health, float MaxHealth, float VisibleTime);

public:
	UPROPERTY(meta = (BindWidget), VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<class UProgressBar> HealthBar;


public:
	FTimerHandle HitTimer;
	FTimerHandle SightTimer;
	void HitHideHUD();
	void SightHideHUD();
	bool bOnHit = false;
	bool bIsOnSight = false;
};
