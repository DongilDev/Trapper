// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/CrosshairHUD.h"
#include "Components/Image.h"

void UCrosshairHUD::NativeConstruct()
{	
	Super::NativeConstruct();
	
	MagneticArrowFrame->SetVisibility(ESlateVisibility::Collapsed);
	ShiftFrame->SetVisibility(ESlateVisibility::Collapsed);
	TrapInstallFrame->SetVisibility(ESlateVisibility::Collapsed);
	DrawFrame->SetVisibility(ESlateVisibility::Collapsed);
	TrapDemolishFrame->SetVisibility(ESlateVisibility::Collapsed);
	TrapWeckageFrame->SetVisibility(ESlateVisibility::Collapsed);
	SetDrawFrameScale(0, 1);
}

void UCrosshairHUD::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
}

void UCrosshairHUD::SetShiftFrameVisibility(bool bIsVisible)
{
	if(bIsVisible)
	{
		ShiftFrame->SetVisibility(ESlateVisibility::HitTestInvisible);
	}
	else
	{
		ShiftFrame->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UCrosshairHUD::SetMagneticFrameVisibility(bool bIsVisible)
{
	if(bIsVisible)
	{
		MagneticArrowFrame->SetVisibility(ESlateVisibility::HitTestInvisible);
	}
	else
	{
		MagneticArrowFrame->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UCrosshairHUD::SetDrawFrameVisibility(bool bIsVisible)
{
	if(bIsVisible)
	{
		DrawFrame->SetVisibility(ESlateVisibility::HitTestInvisible);
	}
	else
	{
		DrawFrame->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UCrosshairHUD::SetDrawFrameScale(float DrawTime, float MaxDrawTime)
{
	float ratio = DrawTime / MaxDrawTime;
	ratio = FMath::Clamp(ratio, 0, 1);
	
	FLinearColor Color = FLinearColor::LerpUsingHSV(FLinearColor(1, 1, 1, 1), FLinearColor(0, 1, 0, 1), ratio);
	DrawFrame->SetColorAndOpacity(Color);

	ratio *= 0.6;
	float invRatio = 1 - ratio;
	DrawFrame->SetRenderScale(FVector2D(invRatio, invRatio));
}

void UCrosshairHUD::SetTrapFrameVisibility(bool bIsVisible)
{
	if(bIsVisible)
	{
		TrapInstallFrame->SetVisibility(ESlateVisibility::HitTestInvisible);
		TrapRotationImage->SetVisibility(ESlateVisibility::HitTestInvisible);
	}
	else
	{
		TrapInstallFrame->SetVisibility(ESlateVisibility::Collapsed);
		TrapRotationImage->SetVisibility(ESlateVisibility::Collapsed);
	}	
}

void UCrosshairHUD::SetTrapDemolishFrameVisibility(bool bIsVisible)
{
	if (bIsVisible)
	{
		TrapDemolishFrame->SetVisibility(ESlateVisibility::HitTestInvisible);
	}
	else
	{
		TrapDemolishFrame->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UCrosshairHUD::SetTrapWeckageFrameVisibility(bool bIsVisible)
{
	if (bIsVisible)
	{
		TrapWeckageFrame->SetVisibility(ESlateVisibility::HitTestInvisible);
	}
	else
	{
		TrapWeckageFrame->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UCrosshairHUD::SetInteractFrameVisibility(bool bIsVisible)
{
	if (bIsVisible)
	{
		InteractFrame->SetVisibility(ESlateVisibility::HitTestInvisible);
	}
	else
	{
		InteractFrame->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UCrosshairHUD::SetTrapAimFrameVisibility(bool bIsVisible)
{
	if (bIsVisible)
	{
		TrapAimFrame->SetVisibility(ESlateVisibility::HitTestInvisible);
	}
	else
	{
		TrapAimFrame->SetVisibility(ESlateVisibility::Collapsed);
	}
}
