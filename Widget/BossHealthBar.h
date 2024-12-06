// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BossHealthBar.generated.h"

/**
 * 
 */
UCLASS()
class TRAPPERPROJECT_API UBossHealthBar : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;
	void UpdateHealth(float Health, float MaxHealth);

public:
	UPROPERTY(meta = (BindWidget), VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<class UProgressBar> HealthBar;

public:
	FTimerHandle HitTimer;
	void HideHUD();
	void ShowHUD();
	bool bOnHit = false;
	bool bIsOnSight = false;
};
