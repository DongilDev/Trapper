// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/Options/SoundsUMG.h"
#include "Widget/Options/OptionsUMG.h"
#include "Components/Button.h"
#include "Components/EditableText.h"
#include "Components/TextBlock.h"
#include "Components/Slider.h"
#include "TrapperProject.h"
#include "TrapperGameInstance.h"
#include "Misc/DefaultValueHelper.h"
#include "Sound/SoundClass.h"
#include "Sound/SoundMix.h"
#include "Kismet/GameplayStatics.h"
#include "Components/Image.h"
#include "Components/CanvasPanel.h"
#include "Kismet/GameplayStatics.h"


void USoundsUMG::NativeConstruct()
{
	Super::NativeConstruct();

	MasterSoundSlider->OnValueChanged.AddDynamic(this, &USoundsUMG::OnMasterSoundSlider);
	EffectSoundSlider->OnValueChanged.AddDynamic(this, &USoundsUMG::OnEffectSoundSlider);
	BackgroundSoundSlider->OnValueChanged.AddDynamic(this, &USoundsUMG::OnBackgroundSoundSlider);
	VoiceDialogSoundSlider->OnValueChanged.AddDynamic(this, &USoundsUMG::OnVoiceDialogSoundSlider);
	MasterSoundNumericalText->OnTextCommitted.AddDynamic(this, &USoundsUMG::OnMasterSoundNumericalCommited);
	EffectSoundNumericalText->OnTextCommitted.AddDynamic(this, &USoundsUMG::OnEffectSoundNumericalCommited);
	BackgroundSoundNumericalText->OnTextCommitted.AddDynamic(this, &USoundsUMG::OnBackgroundSoundNumericalCommited);
	VoiceDialogSoundNumericalText->OnTextCommitted.AddDynamic(this, &USoundsUMG::OnVoiceDialogNumericalCommited);

	MasterSound->OnHovered.AddDynamic(this, &USoundsUMG::OnHorverMasterSound);
	EffectSound->OnHovered.AddDynamic(this, &USoundsUMG::OnHorverEffectSound);
	BackgroundSound->OnHovered.AddDynamic(this, &USoundsUMG::OnHorverBackgroundSound);
	VoiceDialogSound->OnHovered.AddDynamic(this, &USoundsUMG::OnHorverVoiceDialogSound);

	FocusButton = MasterSound;
	FocusImage = MasterSoundBackground;
}


FReply USoundsUMG::NativeOnPreviewKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	if (InKeyEvent.GetKey() == EKeys::Tab)
	{
		return FReply::Handled();
	}

	return Super::NativeOnPreviewKeyDown(InGeometry, InKeyEvent);
}

void USoundsUMG::ImportData(UTrapperGameInstance* GameInstance, UOptionsUMG* InOptionsUMG)
{
	TrapperGameInstance = GameInstance;
	OptionsUMG = InOptionsUMG;

	SoundMix = TrapperGameInstance->GetSoundClassMix();
	MasterSoundClass = TrapperGameInstance->GetMasterSoundClass();
	EffectSoundClass = TrapperGameInstance->GetEffectSoundClass();
	BackgroundSoundClass = TrapperGameInstance->GetBackgroundSoundClass();
	VoiceDialogSoundClass = TrapperGameInstance->GetVoiceDialogSoundClass();

	ImportData();
	ApplyData();
	//SetAllIsChangedSoundValueFalse();
}

void USoundsUMG::ImportData()
{
	bIsChangeOptions = false;

	FSoundSettings InSoundSettings = TrapperGameInstance->GetSoundSettings();

	//UE_LOG(LogTrap, Warning, TEXT("InSoundSettings : %f"), InSoundSettings.MasterSoundsSliderValue);

	MasterSoundSlider->SetValue(InSoundSettings.MasterSoundsSliderValue);
	EffectSoundSlider->SetValue(InSoundSettings.EffectSoundsSliderValue);
	BackgroundSoundSlider->SetValue(InSoundSettings.BackgroundSoundsSliderValue);
	VoiceDialogSoundSlider->SetValue(InSoundSettings.VoiceDialogSoundsSliderValue);

	AllCollapsedWarningText();
}

void USoundsUMG::OnDefaultButton()
{
	MasterSoundSlider->SetValue(1);
	EffectSoundSlider->SetValue(1);
	BackgroundSoundSlider->SetValue(1);
	VoiceDialogSoundSlider->SetValue(1);
}

void USoundsUMG::OnApplyButton()
{
	bIsChangeOptions = false;
	TrapperGameInstance->SaveSoundsData(SoundSettings);

	ApplyData();
}

void USoundsUMG::SetSounds(class USoundWave* ClickSound, class USoundWave* HorverSound)
{
	MouseClickSound = ClickSound;
	MouseHoverSound = HorverSound;
}

void USoundsUMG::SetAllIsChangedSoundValueFalse()
{
	bIsChangedMasterSound = false;
	bIsChangedEffectSound = false;
	bIsChangedBackgroundSound = false;
	bIsChangedVoiceDialogSound = false;
}

void USoundsUMG::FocusHorverChange(class UButton* HorverButton, class UImage* HorverBackground)
{
	FocusButton->SetVisibility(ESlateVisibility::Visible);
	FocusButton = HorverButton;

	if (FocusImage->GetBrush().GetResourceObject() != BackgroundNotApply)
	{
		FocusImage->SetBrushResourceObject(BackgroundNormal);
	}
	FocusImage = HorverBackground;

	if (HorverBackground->GetBrush().GetResourceObject() != BackgroundNotApply)
	{
		HorverBackground->SetBrushResourceObject(BackgroundImpact);
	}
	HorverButton->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
}

void USoundsUMG::OnMasterSoundSlider(float Value)
{
	//UE_LOG(LogTrap, Warning, TEXT("OnMasterSoundSlider %f"), Value);
	if (bIsUpdatingFromText || bIsUpdatingFromSlider)
	{
		//UE_LOG(LogTrap, Warning, TEXT(" return?"));
		return;
	}

	if (Value < 0.01)
	{
		Value = 0;
	}
	SoundSettings.MasterSoundsSliderValue = Value;
	bIsChangedMasterSound = true;

	if (!bIsChangeOptions)
	{
		bIsChangeOptions = true;
		OptionsUMG->SetIsChangeSoundOptions(true);
	}

	MasterSoundWarningPanel->SetVisibility(ESlateVisibility::Visible);
	MasterSoundBackground->SetBrushResourceObject(BackgroundNotApply);

	bIsUpdatingFromSlider = true;
	MasterSoundNumericalText->SetText(FText::FromString(FString::FromInt(Value * 100)));
	bIsUpdatingFromSlider = false;
}

void USoundsUMG::OnEffectSoundSlider(float Value)
{
	if (bIsUpdatingFromText || bIsUpdatingFromSlider)
	{
		return;
	}

	if (Value < 0.01)
	{
		Value = 0;
	}
	SoundSettings.EffectSoundsSliderValue = Value;
	bIsChangedEffectSound = true;

	if (!bIsChangeOptions)
	{
		bIsChangeOptions = true;
		OptionsUMG->SetIsChangeSoundOptions(true);
	}

	EffectSoundWarningPanel->SetVisibility(ESlateVisibility::Visible);
	EffectSoundBackground->SetBrushResourceObject(BackgroundNotApply);

	bIsUpdatingFromSlider = true;
	EffectSoundNumericalText->SetText(FText::FromString(FString::FromInt(Value * 100)));
	bIsUpdatingFromSlider = false;
}

void USoundsUMG::OnBackgroundSoundSlider(float Value)
{
	if (bIsUpdatingFromText || bIsUpdatingFromSlider)
	{
		return;
	}

	if (Value < 0.01)
	{
		Value = 0;
	}
	SoundSettings.BackgroundSoundsSliderValue = Value;
	bIsChangedBackgroundSound = true;

	if (!bIsChangeOptions)
	{
		bIsChangeOptions = true;
		OptionsUMG->SetIsChangeSoundOptions(true);
	}

	BackgroundSoundWarningPanel->SetVisibility(ESlateVisibility::Visible);
	BackgroundSoundBackground->SetBrushResourceObject(BackgroundNotApply);

	bIsUpdatingFromSlider = true;
	BackgroundSoundNumericalText->SetText(FText::FromString(FString::FromInt(Value * 100)));
	bIsUpdatingFromSlider = false;
}

void USoundsUMG::OnVoiceDialogSoundSlider(float Value)
{
	if (bIsUpdatingFromText || bIsUpdatingFromSlider)
	{
		return;
	}

	if (Value < 0.01)
	{
		Value = 0;
	}
	SoundSettings.VoiceDialogSoundsSliderValue = Value;
	bIsChangedVoiceDialogSound = true;

	if (!bIsChangeOptions)
	{
		bIsChangeOptions = true;
		OptionsUMG->SetIsChangeSoundOptions(true);
	}

	VoiceDialogSoundWarningPanel->SetVisibility(ESlateVisibility::Visible);
	VoiceDialogSoundBackground->SetBrushResourceObject(BackgroundNotApply);

	bIsUpdatingFromSlider = true;
	VoiceDialogSoundNumericalText->SetText(FText::FromString(FString::FromInt(Value * 100)));
	bIsUpdatingFromSlider = false;
}

void USoundsUMG::OnMasterSoundNumericalCommited(const FText& Text, ETextCommit::Type CommitMethod)
{
	if (bIsUpdatingFromSlider || bIsUpdatingFromText)
	{
		return;
	}

	if (!bIsChangeOptions)
	{
		bIsChangeOptions = true;
		OptionsUMG->SetIsChangeSoundOptions(true);
	}

	MasterSoundWarningPanel->SetVisibility(ESlateVisibility::Visible);
	MasterSoundBackground->SetBrushResourceObject(BackgroundNotApply);

	float ParsedFloatValue = 0.0f;
	FDefaultValueHelper::ParseFloat(Text.ToString(), ParsedFloatValue);
	int ParsedValue = FMath::RoundToInt(ParsedFloatValue);

	if (ParsedValue < 0)
	{
		ParsedValue = 0;
	}
	else if (ParsedValue > 100)
	{
		ParsedValue = 100;
	}

	MasterSoundNumericalText->SetText(FText::FromString(FString::FromInt(ParsedValue)));

	bIsUpdatingFromText = true;
	ParsedValue /= 100;
	MasterSoundSlider->SetValue(ParsedValue);
	bIsUpdatingFromText = false;

	SoundSettings.MasterSoundsSliderValue = ParsedValue;
	bIsChangedMasterSound = true;
}

void USoundsUMG::OnEffectSoundNumericalCommited(const FText& Text, ETextCommit::Type CommitMethod)
{
	if (bIsUpdatingFromSlider || bIsUpdatingFromText)
	{
		return;
	}

	if (!bIsChangeOptions)
	{
		bIsChangeOptions = true;
		OptionsUMG->SetIsChangeSoundOptions(true);
	}

	EffectSoundWarningPanel->SetVisibility(ESlateVisibility::Visible);
	EffectSoundBackground->SetBrushResourceObject(BackgroundNotApply);

	float ParsedFloatValue = 0;
	FDefaultValueHelper::ParseFloat(Text.ToString(), ParsedFloatValue);
	int ParsedValue = FMath::RoundToInt(ParsedFloatValue);

	if (ParsedValue < 0)
	{
		ParsedValue = 0;
	}
	else if (ParsedValue > 100)
	{
		ParsedValue = 100;
	}

	EffectSoundNumericalText->SetText(FText::FromString(FString::FromInt(ParsedValue)));

	bIsUpdatingFromText = true;
	ParsedValue /= 100;
	EffectSoundSlider->SetValue(ParsedValue);
	bIsUpdatingFromText = false;

	SoundSettings.EffectSoundsSliderValue = ParsedValue;
	bIsChangedMasterSound = true;
}

void USoundsUMG::OnBackgroundSoundNumericalCommited(const FText& Text, ETextCommit::Type CommitMethod)
{
	if (bIsUpdatingFromSlider || bIsUpdatingFromText)
	{
		return;
	}

	if (!bIsChangeOptions)
	{
		bIsChangeOptions = true;
		OptionsUMG->SetIsChangeSoundOptions(true);
	}

	BackgroundSoundWarningPanel->SetVisibility(ESlateVisibility::Visible);
	BackgroundSoundBackground->SetBrushResourceObject(BackgroundNotApply);

	float ParsedFloatValue = 0;
	FDefaultValueHelper::ParseFloat(Text.ToString(), ParsedFloatValue);
	int ParsedValue = FMath::RoundToInt(ParsedFloatValue);

	if (ParsedValue < 0)
	{
		ParsedValue = 0;
	}
	else if (ParsedValue > 100)
	{
		ParsedValue = 100;
	}

	BackgroundSoundNumericalText->SetText(FText::FromString(FString::FromInt(ParsedValue)));

	bIsUpdatingFromText = true;
	ParsedValue /= 100;
	BackgroundSoundSlider->SetValue(ParsedValue);
	bIsUpdatingFromText = false;

	SoundSettings.BackgroundSoundsSliderValue = ParsedValue;
	bIsChangedMasterSound = true;
}

void USoundsUMG::OnVoiceDialogNumericalCommited(const FText& Text, ETextCommit::Type CommitMethod)
{
	if (bIsUpdatingFromSlider || bIsUpdatingFromText)
	{
		return;
	}

	if (!bIsChangeOptions)
	{
		bIsChangeOptions = true;
		OptionsUMG->SetIsChangeSoundOptions(true);
	}

	VoiceDialogSoundWarningPanel->SetVisibility(ESlateVisibility::Visible);
	VoiceDialogSoundBackground->SetBrushResourceObject(BackgroundNotApply);

	float ParsedFloatValue = 0;
	FDefaultValueHelper::ParseFloat(Text.ToString(), ParsedFloatValue);
	int ParsedValue = FMath::RoundToInt(ParsedFloatValue);

	if (ParsedValue < 0)
	{
		ParsedValue = 0;
	}
	else if (ParsedValue > 100)
	{
		ParsedValue = 100;
	}

	VoiceDialogSoundNumericalText->SetText(FText::FromString(FString::FromInt(ParsedValue)));

	bIsUpdatingFromText = true;
	ParsedValue /= 100;
	VoiceDialogSoundSlider->SetValue(ParsedValue);
	bIsUpdatingFromText = false;

	SoundSettings.VoiceDialogSoundsSliderValue = ParsedValue;
	bIsChangedMasterSound = true;
}

void USoundsUMG::OnHorverMasterSound()
{
	UGameplayStatics::PlaySound2D(this, MouseHoverSound);
	FocusHorverChange(MasterSound, MasterSoundBackground);
}

void USoundsUMG::OnHorverEffectSound()
{
	UGameplayStatics::PlaySound2D(this, MouseHoverSound);
	FocusHorverChange(EffectSound, EffectSoundBackground);
}

void USoundsUMG::OnHorverBackgroundSound()
{
	UGameplayStatics::PlaySound2D(this, MouseHoverSound);
	FocusHorverChange(BackgroundSound, BackgroundSoundBackground);
}

void USoundsUMG::OnHorverVoiceDialogSound()
{
	UGameplayStatics::PlaySound2D(this, MouseHoverSound);
	FocusHorverChange(VoiceDialogSound, VoiceDialogSoundBackground);
}

void USoundsUMG::ApplyData()
{
	AllCollapsedWarningText();

	//UE_LOG(LogTrap, Warning, TEXT("ApplyData %f"), SoundSettings.MasterSoundsSliderValue);

	if (bIsChangedMasterSound)
	{
		//UE_LOG(LogTrap, Warning, TEXT("??"));
		bIsChangedMasterSound = false;
		UGameplayStatics::SetSoundMixClassOverride(
			this,
			SoundMix.Get(),
			MasterSoundClass.Get(),
			SoundSettings.MasterSoundsSliderValue,
			1.0f,
			0.0f,
			true
		);
	}
	if (bIsChangedEffectSound)
	{
		bIsChangedEffectSound = false;
		UGameplayStatics::SetSoundMixClassOverride(
			this,
			SoundMix.Get(),
			EffectSoundClass.Get(),
			SoundSettings.EffectSoundsSliderValue,
			1.0f,
			0.0f,
			true
		);
	}
	if (bIsChangedBackgroundSound)
	{
		bIsChangedBackgroundSound = false;
		UGameplayStatics::SetSoundMixClassOverride(
			this,
			SoundMix.Get(),
			BackgroundSoundClass.Get(),
			SoundSettings.BackgroundSoundsSliderValue,
			1.0f,
			0.0f,
			true
		);
	}
	if (bIsChangedVoiceDialogSound)
	{
		bIsChangedVoiceDialogSound = false;
		UGameplayStatics::SetSoundMixClassOverride(
			this,
			SoundMix.Get(),
			VoiceDialogSoundClass.Get(),
			SoundSettings.VoiceDialogSoundsSliderValue,
			1.0f,
			0.0f,
			true
		);
	}

	UGameplayStatics::PushSoundMixModifier(this, SoundMix.Get());
}

void USoundsUMG::AllCollapsedWarningText()
{
	FocusButton->SetVisibility(ESlateVisibility::Visible);

	if (MasterSoundWarningPanel->IsVisible())
	{
		MasterSoundWarningPanel->SetVisibility(ESlateVisibility::Collapsed);
		MasterSoundBackground->SetBrushResourceObject(BackgroundNormal);
	}
	if (EffectSoundWarningPanel->IsVisible())
	{
		EffectSoundWarningPanel->SetVisibility(ESlateVisibility::Collapsed);
		EffectSoundBackground->SetBrushResourceObject(BackgroundNormal);
	}
	if (BackgroundSoundWarningPanel->IsVisible())
	{
		BackgroundSoundWarningPanel->SetVisibility(ESlateVisibility::Collapsed);
		BackgroundSoundBackground->SetBrushResourceObject(BackgroundNormal);
	}
	if (VoiceDialogSoundWarningPanel->IsVisible())
	{
		VoiceDialogSoundWarningPanel->SetVisibility(ESlateVisibility::Collapsed);
		VoiceDialogSoundBackground->SetBrushResourceObject(BackgroundNormal);
	}
}
