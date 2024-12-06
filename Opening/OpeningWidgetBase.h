// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "OpeningWidgetBase.generated.h"

/**
 * 
 */
UCLASS()
class TRAPPERPROJECT_API UOpeningWidgetBase : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;

	UFUNCTION()
	void OnMediaFinished();

	void PlayOpeningMediaWB();

	// ·Îµù
	UPROPERTY(meta = (BindWidget))
	class UCanvasPanel* LoadingCanvasPanel;

	UPROPERTY(meta = (BindWidget))
	class UImage* BackgroundImage;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* TipsTextBlock;
	
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* SkipTextBlock;

public:
	void SetSkipCount(int32 Count);

	void ShowLoading();
};
