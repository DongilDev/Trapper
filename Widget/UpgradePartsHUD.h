// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UpgradePartsHUD.generated.h"

/**
 * 
 */
UCLASS()
class TRAPPERPROJECT_API UUpgradePartsHUD : public UUserWidget
{
	GENERATED_BODY()
public:
	void Init(FString InName, FString InDescription, UTexture2D* InIcon);

public:
	UPROPERTY(meta = (BindWidget), VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<class UTextBlock> Name;

	UPROPERTY(meta = (BindWidget), VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<class UTextBlock> Description;

	UPROPERTY(meta=(BindWidget), VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<class UImage> Icon;
};
