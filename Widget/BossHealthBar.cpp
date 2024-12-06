// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/BossHealthBar.h"
#include "Components/ProgressBar.h"

void UBossHealthBar::NativeConstruct()
{
	Super::NativeConstruct();

}

void UBossHealthBar::UpdateHealth(float Health, float MaxHealth)
{
	if (Health <= 0)
	{
		this->HideHUD();
		
	}
	else
	{
		HealthBar->SetPercent(Health / MaxHealth);		
	}
}

void UBossHealthBar::HideHUD()
{
	this->SetVisibility(ESlateVisibility::Collapsed);
}

void UBossHealthBar::ShowHUD()
{
	this->SetVisibility(ESlateVisibility::Visible);
}
