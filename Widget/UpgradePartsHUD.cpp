// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/UpgradePartsHUD.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"

void UUpgradePartsHUD::Init(FString InName, FString InDescription, UTexture2D* InIcon)
{
	Name->SetText(FText::FromString(InName));
	Description->SetText(FText::FromString(InDescription));
	Icon->SetBrushFromTexture(InIcon);
}
