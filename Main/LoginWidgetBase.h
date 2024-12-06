// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LoginWidgetBase.generated.h"

/**
 * 
 */
UCLASS()
class TRAPPERPROJECT_API ULoginWidgetBase : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	class UEditableTextBox* UserID;

	UPROPERTY(meta = (BindWidget))
	class UEditableTextBox* Password;

	UPROPERTY(meta = (BindWidget))
	class UButton* ShowPasswordButton;

	UPROPERTY(meta = (BindWidget))
	class UButton* LoginButton;

	UPROPERTY(meta = (BindWidget))
	class UButton* JoinButton;

	UPROPERTY(meta = (BindWidget))
	class UButton* AutoLoginButton;
	
	UPROPERTY(meta = (BindWidget))
	class UCanvasPanel* WarningCanvasPanel;

	UFUNCTION()
	void OnShowPasswordButton();
	
	UFUNCTION()
	void OnJoinButton();

	UFUNCTION()
	void OnLoginButton();

	UFUNCTION()
	void OnAutoLoginButton();

	void InitializeUI();

	void ChangeLoginFailedUI();

	void LoginSuccessSound();

	// Sound
	UPROPERTY(EditDefaultsOnly, Category = "Sound")
	TObjectPtr<class USoundWave> MouseClickSound;

	UPROPERTY(EditDefaultsOnly, Category = "Sound")
	TObjectPtr<class USoundWave> MouseHoverSound;

	UPROPERTY(EditDefaultsOnly, Category = "Sound")
	TObjectPtr<class USoundWave> LoginFailedSound;
};
