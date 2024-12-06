// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "JoinWidgetBase.generated.h"

/**
 * 
 */
UCLASS()
class TRAPPERPROJECT_API UJoinWidgetBase : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	class UEditableTextBox* UserID;

	UPROPERTY(meta = (BindWidget))
	class UEditableTextBox* Password;

	UPROPERTY(meta = (BindWidget))
	class UEditableTextBox* Nickname;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* CheckId;
	
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* CheckPassword;
	
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* CheckNickname;

	UPROPERTY(meta = (BindWidget))
	class UButton* ShowPasswordButton;
	
	UPROPERTY(meta = (BindWidget))
	class UButton* OkButton;

	UPROPERTY(meta = (BindWidget))
	class UButton* CancelButton;

	UPROPERTY(meta = (BindWidget))
	class UCanvasPanel* WarningCanvasPanel;

	UFUNCTION()
	void OnOkButton();

	UFUNCTION()
	void OnCancelButton();

	UFUNCTION()
	void OnShowPasswordButton();

	void ShowDuplicateId(bool Duplicated);
	void JoinFailedUI();
	void InitalizeUI();
	bool ContainsSpecialCharacters(const FString& InputString);

	// Sound
	UPROPERTY(EditDefaultsOnly, Category = "Sound")
	TObjectPtr<class USoundWave> MouseClickSound;

	UPROPERTY(EditDefaultsOnly, Category = "Sound")
	TObjectPtr<class USoundWave> MouseHoverSound;

	UPROPERTY(EditDefaultsOnly, Category = "Sound")
	TObjectPtr<class USoundWave> MouseClickFailedSound;

	UFUNCTION()
	void OnButtonHovered();

private:
	bool IsJoinable = true;
};
