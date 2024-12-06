//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Options.h"
#include "ControlsUMG.generated.h"

/**
 *
 */
UCLASS()
class TRAPPERPROJECT_API UControlsUMG : public UUserWidget
{
	GENERATED_BODY()

private:
	virtual void NativeConstruct() override;
	virtual FReply NativeOnPreviewKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;

public:
	void ImportData(class UTrapperGameInstance* GameInstance, class UOptionsUMG* InOptionsUMG);
	void ImportData();
	void OnDefaultButton();
	void OnApplyButton();
	void SetSounds(class USoundWave* ClickSound, class USoundWave* HorverSound);

private:
	UFUNCTION()
	void OnGeneralSensitivitySlider(float Value);

	UFUNCTION()
	void OnAimSensitivitySlider(float Value);

	UFUNCTION()
	void OnGeneralSensitivityNumericalCommited(const FText& Text, ETextCommit::Type CommitMethod);

	UFUNCTION()
	void OnAimSensitivityNumericalCommited(const FText& Text, ETextCommit::Type CommitMethod);

	UFUNCTION()
	void OnHorverGeneralSensitivity();

	UFUNCTION()
	void OnHorverAimSensitivity();

private:
	void ApplyData();
	void AllCollapsedWarningText();
	void FocusHorverChange(class UButton* HorverButton, class UImage* HorverBackground);

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TObjectPtr<class UTexture> BackgroundNormal;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TObjectPtr<class UTexture> BackgroundImpact;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TObjectPtr<class UTexture> BackgroundNotApply;

private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> GeneralSensitivity;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class USlider> GeneralSensitivitySlider;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UCanvasPanel> GeneralSensitivityWarningPanel;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UEditableText> GeneralSensitivityNumericalText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> AimSensitivity;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class USlider> AimSensitivitySlider;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UCanvasPanel> AimSensitivityWarningPanel;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UEditableText> AimSensitivityNumericalText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UImage> GeneralSensitivityBackground;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UImage> AimSensitivityBackground;

	uint8 bIsUpdatingFromSlider : 1 = false;
	uint8 bIsUpdatingFromText : 1 = false;
	uint8 bIsChangeOptions : 1 = false;

	FControlSettings ControlSettings;
	TWeakObjectPtr<class UTrapperGameInstance> TrapperGameInstance;
	TWeakObjectPtr<class UOptionsUMG> OptionsUMG;

	TObjectPtr<class UButton> FocusButton;
	TObjectPtr<class UImage> FocusImage;

	TObjectPtr<class USoundWave> MouseClickSound;
	TObjectPtr<class USoundWave> MouseHoverSound;
};
