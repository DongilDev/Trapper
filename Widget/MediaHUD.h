// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MediaHUD.generated.h"

/**
 * 
 */
UCLASS()
class TRAPPERPROJECT_API UMediaHUD : public UUserWidget
{
	GENERATED_BODY()

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> SkipText;

public:
	void SetSkipText(int8 Value);
};
