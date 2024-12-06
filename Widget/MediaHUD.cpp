// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/MediaHUD.h"
#include "Components/TextBlock.h"


void UMediaHUD::SetSkipText(int8 Value)
{
	FString SkipString = FString::Printf(TEXT("F키를 눌러 스킵 (%d/2)"), Value);
	SkipText->SetText(FText::FromString(SkipString));

	if (Value > 0)
	{
		SkipText->SetColorAndOpacity(FSlateColor(FLinearColor(1.f,1.f,1.f,1.f)));
	}
	else
	{
		SkipText->SetColorAndOpacity(FSlateColor(FLinearColor(1.f,1.f,1.f,0.3f)));
	}
}
