// Fill out your copyright notice in the Description page of Project Settings.


#include "Title/CreditWidgetBase.h"


void UCreditWidgetBase::NativeConstruct()
{
	Super::NativeConstruct();

	if (CreditWidgetAnim)
	{
		FWidgetAnimationDynamicEvent AnimationFinishedDelegate;
		AnimationFinishedDelegate.BindDynamic(this, &UCreditWidgetBase::CreditFinished);
		BindToAnimationFinished(CreditWidgetAnim, AnimationFinishedDelegate);
	}

	OnVisibilityChanged.AddDynamic(this, &UCreditWidgetBase::OnVisibilityChangedFunction);
}

void UCreditWidgetBase::CreditFinished()
{
	SetVisibility(ESlateVisibility::Collapsed);
}

void UCreditWidgetBase::OnVisibilityChangedFunction(ESlateVisibility InVisibility)
{
	if (InVisibility == ESlateVisibility::Visible)
	{
		PlayAnimation(CreditWidgetAnim);
	}
	else if (InVisibility == ESlateVisibility::Collapsed)
	{
		if (IsAnimationPlaying(CreditWidgetAnim))
		{
			StopAnimation(CreditWidgetAnim);
		}
	}
}
