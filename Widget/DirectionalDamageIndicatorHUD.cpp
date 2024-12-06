// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/DirectionalDamageIndicatorHUD.h"
#include "Components/Image.h"

void UDirectionalDamageIndicatorHUD::NativeConstruct()
{
	Super::NativeConstruct();
	ImageMaterial = Direction->GetDynamicMaterial();
}

void UDirectionalDamageIndicatorHUD::SetDirection(float Degree, float Time)
{
	Direction->SetRenderTransformAngle(Degree);
	ImageMaterial->SetScalarParameterValue("Clip", Time );
}
