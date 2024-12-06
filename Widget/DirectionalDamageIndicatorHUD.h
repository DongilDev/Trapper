// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DirectionalDamageIndicatorHUD.generated.h"

/**
 * 
 */
UCLASS()
class TRAPPERPROJECT_API UDirectionalDamageIndicatorHUD : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	void SetDirection(float Degree, float Time);

public:
	UPROPERTY(meta=(BindWidget), VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<class UImage> Direction;

	TObjectPtr<class UMaterialInstanceDynamic> ImageMaterial;
};
