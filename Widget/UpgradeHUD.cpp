// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/UpgradeHUD.h"
#include "UpgradePartsHUD.h"

#include "Upgrade/UpgradeMechanics.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Animation/WidgetAnimation.h"

void UUpgradeHUD::OnHovered(uint8 parts)
{
	switch(parts)
	{
	case 0:
		Parts1->SetColorAndOpacity(FLinearColor(1,0.887327, 0.521173,1));
		break;
	case 1:
		Parts2->SetColorAndOpacity(FLinearColor(1, 0.887327, 0.521173, 1));
		break;
	case 2:
		Parts3->SetColorAndOpacity(FLinearColor(1, 0.887327, 0.521173, 1));
		break;	
	case 3:
		Parts4->SetColorAndOpacity(FLinearColor(1, 0.887327, 0.521173, 1));
		break;

	}
}

void UUpgradeHUD::OnUnhovered(uint8 parts)
{
	switch(parts)
	{
	case 0:
		Parts1->SetColorAndOpacity(FLinearColor(1, 1, 1, 1));
		break;
	case 1:
		Parts2->SetColorAndOpacity(FLinearColor(1, 1, 1, 1));
		break;
	case 2:
		Parts3->SetColorAndOpacity(FLinearColor(1, 1, 1, 1));
		break;
	case 3:
		Parts4->SetColorAndOpacity(FLinearColor(1, 1, 1, 1));
		break;

	}
}

void UUpgradeHUD::PlayPopupAnimation()
{
	if (!PopupUpgrade)
	{
		UE_LOG(LogTemp, Error, TEXT("PopupUpgrade animation is not assigned!"));
		return;
	}

	// 애니메이션 재생
	PlayAnimation(PopupUpgrade);
}

void UUpgradeHUD::PlayExitAnimation()
{
	if (!ExitUpgrade)
	{
		UE_LOG(LogTemp, Error, TEXT("ExitUpgrade animation is not assigned!"));
		return;
	}

	// 애니메이션이 끝났을 때 호출될 함수 바인딩
	FWidgetAnimationDynamicEvent ExitAnimationEvent;
	ExitAnimationEvent.BindUFunction(this, FName(FString(TEXT("UpgradeHUDCollapsed"))));
	BindToAnimationFinished(ExitUpgrade, ExitAnimationEvent);

	// 애니메이션 재생
	PlayAnimation(ExitUpgrade);
}

void UUpgradeHUD::UpgradeHUDCollapsed()
{
	SetVisibility(ESlateVisibility::Collapsed);
}

void UUpgradeHUD::SetLastTime(int32 time)
{
	LastTime->SetText(FText::AsNumber(time));
}

void UUpgradeHUD::SetPillarTextVisibility(bool Arg)
{
	if(Arg)
	{
		PillarText->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
	else
	{
		PillarText->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UUpgradeHUD::SetTutorialBackgroundVisibility(bool Arg)
{
	if(Arg)
	{
		TutorialBackground->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
	else
	{
		TutorialBackground->SetVisibility(ESlateVisibility::Collapsed);
	}
}
