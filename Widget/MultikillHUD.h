// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MultikillHUD.generated.h"

/**
 * 
 */
UCLASS()
class TRAPPERPROJECT_API UMultikillHUD : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void SetMultikillCount(uint8 Count);
	void AllClearTimer();
	
private:
	void HudTimerCallback();
	void OpacityTimerCallback();

private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> MultikillCountText;

	FTimerHandle HudTimerHandle;
	FTimerHandle OpacityTimerHandle;

	uint8 bDownScaleEnd = false;
};
