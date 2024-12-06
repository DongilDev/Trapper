// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CreditWidgetBase.generated.h"

/**
 * 
 */
UCLASS()
class TRAPPERPROJECT_API UCreditWidgetBase : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;

private:
	UFUNCTION()
	void CreditFinished();

	UFUNCTION()
	void OnVisibilityChangedFunction(ESlateVisibility InVisibility);

private:
	UPROPERTY(meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* CreditWidgetAnim;
};
