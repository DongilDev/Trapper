// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/Options/ControlsUMG.h"
#include "Widget/Options/OptionsUMG.h"
#include "Components/Button.h"
#include "Components/EditableText.h"
#include "Components/TextBlock.h"
#include "Components/Slider.h"
#include "Components/Image.h"
#include "Components/Border.h"
#include "Components/CanvasPanel.h"
#include "TrapperProject.h"
#include "TrapperGameInstance.h"
#include "Misc/DefaultValueHelper.h"
#include "GameFramework/InputSettings.h"
#include "Character/TrapperPlayer.h"
#include "Kismet/GameplayStatics.h"


void UControlsUMG::NativeConstruct()
{
	Super::NativeConstruct();

	GeneralSensitivitySlider->OnValueChanged.AddDynamic(this, &UControlsUMG::OnGeneralSensitivitySlider);
	AimSensitivitySlider->OnValueChanged.AddDynamic(this, &UControlsUMG::OnAimSensitivitySlider);
	GeneralSensitivityNumericalText->OnTextCommitted.AddDynamic(this, &UControlsUMG::OnGeneralSensitivityNumericalCommited);
	AimSensitivityNumericalText->OnTextCommitted.AddDynamic(this, &UControlsUMG::OnAimSensitivityNumericalCommited);

	GeneralSensitivity->OnHovered.AddDynamic(this, &UControlsUMG::OnHorverGeneralSensitivity);
	AimSensitivity->OnHovered.AddDynamic(this, &UControlsUMG::OnHorverAimSensitivity);

	FocusButton = GeneralSensitivity;
	FocusImage = GeneralSensitivityBackground;
}

FReply UControlsUMG::NativeOnPreviewKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	if (InKeyEvent.GetKey() == EKeys::Tab)
	{
		return FReply::Handled();
	}

	return Super::NativeOnPreviewKeyDown(InGeometry, InKeyEvent);
}

void UControlsUMG::ImportData(UTrapperGameInstance* GameInstance, UOptionsUMG* InOptionsUMG)
{
	TrapperGameInstance = GameInstance;
	OptionsUMG = InOptionsUMG;
	ImportData();
	ApplyData();
}

void UControlsUMG::ImportData()
{
	bIsChangeOptions = false;

	FControlSettings InControlSettings = TrapperGameInstance->GetControlSettigs();
	GeneralSensitivitySlider->SetValue(InControlSettings.GeneralSensitivitySliderValue);
	AimSensitivitySlider->SetValue(InControlSettings.AimSensitivitySliderValue);

	AllCollapsedWarningText();
}

void UControlsUMG::OnDefaultButton()
{
	GeneralSensitivitySlider->SetValue(0.5);
	AimSensitivitySlider->SetValue(0.5);
}

void UControlsUMG::OnApplyButton()
{
	bIsChangeOptions = false;
	ApplyData();
	TrapperGameInstance->SaveControlsData(ControlSettings);
}

void UControlsUMG::SetSounds(class USoundWave* ClickSound, class USoundWave* HorverSound)
{
	MouseClickSound = ClickSound;
	MouseHoverSound = HorverSound;
}

void UControlsUMG::OnGeneralSensitivitySlider(float Value)
{
	ControlSettings.GeneralSensitivitySliderValue = Value;

	if (bIsUpdatingFromText || bIsUpdatingFromSlider)
	{
		return;
	}

	if (!bIsChangeOptions)
	{
		bIsChangeOptions = true;
		OptionsUMG->SetIsChangeControlOptions(true);
	}

	GeneralSensitivityWarningPanel->SetVisibility(ESlateVisibility::Visible);
	GeneralSensitivityBackground->SetBrushResourceObject(BackgroundNotApply);

	bIsUpdatingFromSlider = true;
	if (Value == 0)
	{
		Value = 0.01;
		GeneralSensitivitySlider->SetValue(Value);
	}
	GeneralSensitivityNumericalText->SetText(FText::FromString(FString::FromInt(Value * 100)));
	bIsUpdatingFromSlider = false;
}

void UControlsUMG::OnAimSensitivitySlider(float Value)
{
	ControlSettings.AimSensitivitySliderValue = Value;

	if (bIsUpdatingFromText || bIsUpdatingFromSlider)
	{
		return;
	}

	if (!bIsChangeOptions)
	{
		bIsChangeOptions = true;
		OptionsUMG->SetIsChangeControlOptions(true);
	}

	AimSensitivityWarningPanel->SetVisibility(ESlateVisibility::Visible);
	AimSensitivityBackground->SetBrushResourceObject(BackgroundNotApply);

	bIsUpdatingFromSlider = true;
	if (Value == 0)
	{
		Value = 0.01;
		AimSensitivitySlider->SetValue(Value);
	}
	AimSensitivityNumericalText->SetText(FText::FromString(FString::FromInt(Value * 100)));
	bIsUpdatingFromSlider = false;
}

void UControlsUMG::OnGeneralSensitivityNumericalCommited(const FText& Text, ETextCommit::Type CommitMethod)
{
	if (bIsUpdatingFromSlider || bIsUpdatingFromText)
	{
		return;
	}

	if (!bIsChangeOptions)
	{
		bIsChangeOptions = true;
		OptionsUMG->SetIsChangeControlOptions(true);
	}

	GeneralSensitivityWarningPanel->SetVisibility(ESlateVisibility::Visible);
	GeneralSensitivityBackground->SetBrushResourceObject(BackgroundNotApply);

	float ParsedValue = 0;
	FDefaultValueHelper::ParseFloat(Text.ToString(), ParsedValue);

	bool bShouldBeFixedValue = false;
	if (ParsedValue < 1)
	{
		bShouldBeFixedValue = true;
		ParsedValue = 1;
	}
	else if (ParsedValue > 100)
	{
		bShouldBeFixedValue = true;
		ParsedValue = 100;
	}

	bIsUpdatingFromText = true;
	if (bShouldBeFixedValue)
	{
		GeneralSensitivityNumericalText->SetText(FText::FromString(FString::FromInt(ParsedValue)));
	}
	ParsedValue /= 100;
	GeneralSensitivitySlider->SetValue(ParsedValue);
	bIsUpdatingFromText = false;
}

void UControlsUMG::OnAimSensitivityNumericalCommited(const FText& Text, ETextCommit::Type CommitMethod)
{
	if (bIsUpdatingFromSlider || bIsUpdatingFromText)
	{
		return;
	}

	if (!bIsChangeOptions)
	{
		bIsChangeOptions = true;
		OptionsUMG->SetIsChangeControlOptions(true);
	}

	AimSensitivityWarningPanel->SetVisibility(ESlateVisibility::Visible);
	AimSensitivityBackground->SetBrushResourceObject(BackgroundNotApply);

	float ParsedValue = 0;
	FDefaultValueHelper::ParseFloat(Text.ToString(), ParsedValue);

	bool bShouldBeFixedValue = false;
	if (ParsedValue < 1)
	{
		bShouldBeFixedValue = true;
		ParsedValue = 1;
	}
	else if (ParsedValue > 100)
	{
		bShouldBeFixedValue = true;
		ParsedValue = 100;
	}

	bIsUpdatingFromText = true;
	if (bShouldBeFixedValue)
	{
		AimSensitivityNumericalText->SetText(FText::FromString(FString::FromInt(ParsedValue)));
	}
	ParsedValue /= 100;
	AimSensitivitySlider->SetValue(ParsedValue);
	bIsUpdatingFromText = false;
}

void UControlsUMG::OnHorverGeneralSensitivity()
{
	UGameplayStatics::PlaySound2D(this, MouseHoverSound);
	FocusHorverChange(GeneralSensitivity, GeneralSensitivityBackground);
}

void UControlsUMG::OnHorverAimSensitivity()
{
	UGameplayStatics::PlaySound2D(this, MouseHoverSound);
	FocusHorverChange(AimSensitivity, AimSensitivityBackground);
}

void UControlsUMG::ApplyData()
{
	AllCollapsedWarningText();

	ATrapperPlayer* Player = Cast<ATrapperPlayer>(GetOwningPlayerPawn());
	if (Player)
	{
		Player->SetSensivityScale(
			ControlSettings.GeneralSensitivitySliderValue,
			ControlSettings.AimSensitivitySliderValue);
	}
}

void UControlsUMG::AllCollapsedWarningText()
{
	FocusButton->SetVisibility(ESlateVisibility::Visible);

	if (GeneralSensitivityWarningPanel->IsVisible())
	{
		GeneralSensitivityWarningPanel->SetVisibility(ESlateVisibility::Collapsed);
		GeneralSensitivityBackground->SetBrushResourceObject(BackgroundNormal);
	}
	if (AimSensitivityWarningPanel->IsVisible())
	{
		AimSensitivityWarningPanel->SetVisibility(ESlateVisibility::Collapsed);
		AimSensitivityBackground->SetBrushResourceObject(BackgroundNormal);
	}
}

void UControlsUMG::FocusHorverChange(class UButton* HorverButton, class UImage* HorverBackground)
{
	if (!HorverBackground || !HorverButton)
	{
		return;
	}
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

