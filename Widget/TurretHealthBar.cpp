// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/TurretHealthBar.h"
#include "Components/ProgressBar.h"


void UTurretHealthBar::NativeConstruct()
{
	Super::NativeConstruct();
	HealthBar->SetVisibility(ESlateVisibility::HitTestInvisible);
}

void UTurretHealthBar::UpdateHealth(float Health, float MaxHealth)
{
	HealthBar->SetPercent(Health / MaxHealth);
}
