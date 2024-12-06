// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/MultikillHUD.h"
#include "Components/TextBlock.h"
#include "TrapperProject.h"


void UMultikillHUD::SetMultikillCount(uint8 Count)
{
	//UE_LOG(LogTrap, Warning, TEXT("UMultikillHUD::SetMultikillCount"));
	//UE_LOG(LogTrap, Warning, TEXT("Count : %d"), Count);

	MultikillCountText->SetText(FText::FromString(FString::FromInt(Count)));
	SetRenderOpacity(1);
	SetRenderScale(FVector2D(1.1, 1.1));
	bDownScaleEnd = false;

	GetWorld()->GetTimerManager().ClearTimer(HudTimerHandle);
	GetWorld()->GetTimerManager().ClearTimer(OpacityTimerHandle);
	SetVisibility(ESlateVisibility::HitTestInvisible);

	GetWorld()->GetTimerManager().SetTimer(HudTimerHandle, this, &UMultikillHUD::HudTimerCallback, 0.03 , true);
}

void UMultikillHUD::AllClearTimer()
{
	GetWorld()->GetTimerManager().ClearAllTimersForObject(this);
}

void UMultikillHUD::HudTimerCallback()
{
	float CurrentScale = GetRenderTransform().Scale.X;

	if (!bDownScaleEnd)
	{
		if (CurrentScale > 0.91f)
		{
			SetRenderScale(FVector2D(FMath::Lerp(CurrentScale, 0.9f, 0.3f)));
		}
		else
		{
			bDownScaleEnd = true;
		}
	}
	else
	{
		if (CurrentScale < 0.99f)
		{
			SetRenderScale(FVector2D(FMath::Lerp(CurrentScale, 1.0f, 0.3f)));
		}
		else
		{
			GetWorld()->GetTimerManager().ClearTimer(HudTimerHandle);
			GetWorld()->GetTimerManager().SetTimer(OpacityTimerHandle, this, &UMultikillHUD::OpacityTimerCallback, 0.03, true);
		}
	}
}

void UMultikillHUD::OpacityTimerCallback()
{
	float Opacity = GetRenderOpacity();
	Opacity -= 0.03;
	if (Opacity <= 0)
	{
		GetWorld()->GetTimerManager().ClearTimer(OpacityTimerHandle);
		SetVisibility(ESlateVisibility::Collapsed);
	}
	SetRenderOpacity(Opacity);
}
