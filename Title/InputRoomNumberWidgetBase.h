// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InputRoomNumberWidgetBase.generated.h"

/**
 *
 */
UCLASS()
class TRAPPERPROJECT_API UInputRoomNumberWidgetBase : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Control")
	class UButton* EnterRoomButton;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Control")
	class UButton* QuitInputRoomNumberButton;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Control")
	class UEditableTextBox* InputRoomNumber;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Control")
	class UCanvasPanel* RoomErrorPopUpCanvasPanel;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Control")
	class UTextBlock* RoomErrorMessage;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Control")
	class UButton* OkButton;

	UFUNCTION()
	void OnEnterRoomButton();

	UFUNCTION()
	void OnQuitInputRoomNumberButton();

	UFUNCTION()
	void HideRoomErrorPop();

	void ShowRoomErrorPop(FString Text);
	
};
