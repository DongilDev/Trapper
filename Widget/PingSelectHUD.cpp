// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/PingSelectHUD.h"
#include "Components/Image.h"


FVector2D UPingSelectHUD::GetSize()
{
	//Default->GetDesiredSize();
	//Default->GetCachedGeometry().GetLocalSize();
	return Default->GetCachedGeometry().GetDrawSize();
}
