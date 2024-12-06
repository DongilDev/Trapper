// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/PingIndicatorHUD.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"

void UPingIndicatorHUD::NativeConstruct()
{
	Super::NativeConstruct();

	PanelSlot = Cast<UCanvasPanelSlot>(Panel->Slot);
	PanelSlot->SetAnchors(FAnchors(0, 0, 0, 0));
}