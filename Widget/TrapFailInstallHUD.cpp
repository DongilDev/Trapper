// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/TrapFailInstallHUD.h"

#include "Components/TextBlock.h"
#include "TrapperProject.h"


void UTrapFailInstallHUD::NativeDestruct()
{
	Super::NativeDestruct();

	GetWorld()->GetTimerManager().ClearAllTimersForObject(this);
}

void UTrapFailInstallHUD::ShowText(FString string)
{
	TrapText->SetText(FText::FromString(string));
	TrapText->SetVisibility(ESlateVisibility::HitTestInvisible);
	//SetVisibility(ESlateVisibility::HitTestInvisible);
	
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &UTrapFailInstallHUD::SetTextLifeOnTimer, 3, false);
}

void UTrapFailInstallHUD::SetTextLifeOnTimer()
{
	TrapText->SetVisibility(ESlateVisibility::Collapsed);
	//SetVisibility(ESlateVisibility::Collapsed);
}
