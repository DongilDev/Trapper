// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DialogHUD.generated.h"

/**
 * 
 */
UCLASS()
class TRAPPERPROJECT_API UDialogHUD : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget), VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<class UOverlay> GodDialogBox;

	UPROPERTY(meta = (BindWidget), VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<class UImage> GodBG;

	UPROPERTY(meta = (BindWidget), VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<class UTextBlock> GodText;

	UPROPERTY(meta = (BindWidget), VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<class UVerticalBox> PlayerDialogBox;

	UPROPERTY(meta = (BindWidget), VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<class UTextBlock> NameText;

	UPROPERTY(meta = (BindWidget), VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<class UTextBlock> DialogText;

public:
	void ShowPlayerDialog(bool Value);
	void SetPlayerText(FString InName, FString InDialog);

	void ShowGodDialog(bool Value);
	void SetGodDialog(int32 BackgroundIndex, FString InDialog);
};
