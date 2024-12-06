// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/MonsterHealthBar.h"
#include "Components/ProgressBar.h"

void UMonsterHealthBar::NativeConstruct()
{
	Super::NativeConstruct();
	HealthBar->SetVisibility(ESlateVisibility::Collapsed);
}

void UMonsterHealthBar::UpdateHealth(float Health, float MaxHealth, float VisibleTime)
{
	if(Health <= 0)
	{
		HealthBar->SetVisibility(ESlateVisibility::Collapsed);
		bOnHit = false;
		GetWorld()->GetTimerManager().ClearTimer(HitTimer);
	}
	else
	{
		HealthBar->SetPercent(Health / MaxHealth);
		HealthBar->SetVisibility(ESlateVisibility::HitTestInvisible);
		bOnHit = true;
	
		GetWorld()->GetTimerManager().SetTimer(
			HitTimer, this, &UMonsterHealthBar::HitHideHUD , VisibleTime, false);
	}
}


void UMonsterHealthBar::OnPlayerSight(float Health, float MaxHealth, float VisibleTime)
{
	if(Health <= 0)
	{
		HealthBar->SetVisibility(ESlateVisibility::Collapsed);
		bIsOnSight = false;
		GetWorld()->GetTimerManager().ClearTimer(SightTimer);
	}
	else
	{
		HealthBar->SetPercent(Health / MaxHealth);
		HealthBar->SetVisibility(ESlateVisibility::HitTestInvisible);
		bIsOnSight = true;

		GetWorld()->GetTimerManager().SetTimer(
			SightTimer, this, &UMonsterHealthBar::HitHideHUD, VisibleTime, false);
	}
}

void UMonsterHealthBar::HitHideHUD()
{
	bOnHit = false;
	if(!bIsOnSight)
	{
		HealthBar->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UMonsterHealthBar::SightHideHUD()
{
	bIsOnSight = false;
	if(!bOnHit)
	{
		HealthBar->SetVisibility(ESlateVisibility::Collapsed);
	}
}
