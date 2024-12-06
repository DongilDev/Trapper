// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SystemMenuUMG.generated.h"

/**
 *
 */
UCLASS()
class TRAPPERPROJECT_API USystemMenuUMG : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

public:
	void OnBack();
	void OnMouseClickSound();

private:
	UFUNCTION()
	void OnClickBackButton();

	UFUNCTION()
	void OnClickOptionButton();

	UFUNCTION()
	void OnClickExitButton();

	UFUNCTION()
	void OnClickRealExitButton();

	UFUNCTION()
	void OnClickCancelButton();

	UFUNCTION()
	void OnHorverBackButton();

	UFUNCTION()
	void OnHorverOptionButton();

	UFUNCTION()
	void OnHorverExitButton();

	UFUNCTION()
	void OnUnHorverBackButton();

	UFUNCTION()
	void OnUnHorverOptionButton();

	UFUNCTION()
	void OnUnHorverExitButton();

	UFUNCTION()
	void OnHorverRealExitButton();

	UFUNCTION()
	void OnHorverCancelButton();

	UFUNCTION()
	void OnVisibilityChangedFunction(ESlateVisibility InVisibility);

	/*UFUNCTION()
	void OnClickReStartButton();

	UFUNCTION()
	void OnHorverReStartButton();

	UFUNCTION()
	void OnUnHorverReStartButton();

	UFUNCTION()
	void OnClickRealReStartButton();

	UFUNCTION()
	void OnClickReStartCancelButton();*/

private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UOptionsUMG> OptionsUMG;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> BackButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> OptionsButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> GameExitButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UCanvasPanel> GameExitPanel;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> LobbyExitButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> RealExitButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> CancelButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> BackText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> OptionText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> GameExitText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> LobbyExitText;

	/*UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UCanvasPanel> GameReStartPanel;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> GameReStartButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> GameReStartText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> ReStartCancelButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> RealReStartButton;*/

	uint8 bInLobby : 1 = false;

	UPROPERTY(EditDefaultsOnly, Category = "Sound")
	TObjectPtr<class USoundWave> MouseClickSound;

	UPROPERTY(EditDefaultsOnly, Category = "Sound")
	TObjectPtr<class USoundWave> MouseHoverSound;

	uint8 bOnSound : 1 = true;


public:
	void SetLobbyProperty(class UBorder* Border, class UEditableTextBox* TextBox);

private:
	TWeakObjectPtr<class UBorder> ChatBoxBorder;
	TWeakObjectPtr<class UEditableTextBox> ChatInput;
};
