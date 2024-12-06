// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Options.h"
#include "SoundsUMG.generated.h"

/**
 *
 */
UCLASS()
class TRAPPERPROJECT_API USoundsUMG : public UUserWidget
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
	void OnMasterSoundSlider(float Value);

	UFUNCTION()
	void OnEffectSoundSlider(float Value);

	UFUNCTION()
	void OnBackgroundSoundSlider(float Value);

	UFUNCTION()
	void OnVoiceDialogSoundSlider(float Value);

	UFUNCTION()
	void OnMasterSoundNumericalCommited(const FText& Text, ETextCommit::Type CommitMethod);

	UFUNCTION()
	void OnEffectSoundNumericalCommited(const FText& Text, ETextCommit::Type CommitMethod);

	UFUNCTION()
	void OnBackgroundSoundNumericalCommited(const FText& Text, ETextCommit::Type CommitMethod);

	UFUNCTION()
	void OnVoiceDialogNumericalCommited(const FText& Text, ETextCommit::Type CommitMethod);

	UFUNCTION()
	void OnHorverMasterSound();

	UFUNCTION()
	void OnHorverEffectSound();

	UFUNCTION()
	void OnHorverBackgroundSound();

	UFUNCTION()
	void OnHorverVoiceDialogSound();

private:
	void ApplyData();
	void AllCollapsedWarningText();
	void SetAllIsChangedSoundValueFalse();
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
	TObjectPtr<class USlider> MasterSoundSlider;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class USlider> EffectSoundSlider;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class USlider> BackgroundSoundSlider;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class USlider> VoiceDialogSoundSlider;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UEditableText> MasterSoundNumericalText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UEditableText> EffectSoundNumericalText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UEditableText> BackgroundSoundNumericalText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UEditableText> VoiceDialogSoundNumericalText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> MasterSound;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> EffectSound;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> BackgroundSound;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> VoiceDialogSound;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UImage> MasterSoundBackground;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UImage> EffectSoundBackground;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UImage> BackgroundSoundBackground;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UImage> VoiceDialogSoundBackground;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UCanvasPanel> MasterSoundWarningPanel;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UCanvasPanel> EffectSoundWarningPanel;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UCanvasPanel> BackgroundSoundWarningPanel;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UCanvasPanel> VoiceDialogSoundWarningPanel;

	uint8 bIsUpdatingFromSlider : 1 = false;
	uint8 bIsUpdatingFromText : 1 = false;
	uint8 bIsChangeOptions : 1 = false;

	uint8 bIsChangedMasterSound : 1 = false;
	uint8 bIsChangedEffectSound : 1 = false;
	uint8 bIsChangedBackgroundSound : 1 = false;
	uint8 bIsChangedVoiceDialogSound : 1 = false;

	FSoundSettings SoundSettings;

	TWeakObjectPtr<class UTrapperGameInstance> TrapperGameInstance;
	TWeakObjectPtr<class UOptionsUMG> OptionsUMG;

	TObjectPtr<class UButton> FocusButton;
	TObjectPtr<class UImage> FocusImage;

	TWeakObjectPtr<class USoundMix> SoundMix;
	TWeakObjectPtr<class USoundClass> MasterSoundClass;
	TWeakObjectPtr<class USoundClass> EffectSoundClass;
	TWeakObjectPtr<class USoundClass> BackgroundSoundClass;
	TWeakObjectPtr<class USoundClass> VoiceDialogSoundClass;

	TObjectPtr<class USoundWave> MouseClickSound;
	TObjectPtr<class USoundWave> MouseHoverSound;
};
