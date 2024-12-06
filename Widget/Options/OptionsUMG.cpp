// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/Options/OptionsUMG.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Components/CanvasPanel.h"
#include "TrapperProject.h"
#include "Widget/Options/ControlsUMG.h"
#include "Widget/Options/SoundsUMG.h"
#include "Widget/Options/GraphicsUMG.h"
#include "TrapperGameInstance.h"
#include "Components/CanvasPanelSlot.h"
#include "Kismet/GameplayStatics.h"


void UOptionsUMG::NativeConstruct()
{
	Super::NativeConstruct();

	ControlsButton->OnClicked.AddDynamic(this, &UOptionsUMG::OnClickControlsButton);
	ControlsButton->OnHovered.AddDynamic(this, &UOptionsUMG::OnHorverControlsButton);

	SoundsButton->OnClicked.AddDynamic(this, &UOptionsUMG::OnClickSoundsButton);
	SoundsButton->OnHovered.AddDynamic(this, &UOptionsUMG::OnHorverSoundsButton);

	GraphicsButton->OnClicked.AddDynamic(this, &UOptionsUMG::OnClickGraphicsButton);
	GraphicsButton->OnHovered.AddDynamic(this, &UOptionsUMG::OnHorverGraphicsButton);

	ApplyButton->OnClicked.AddDynamic(this, &UOptionsUMG::OnClickApplyButton);
	ApplyButton->OnHovered.AddDynamic(this, &UOptionsUMG::OnHorverApplyButton);

	DefaultButton->OnClicked.AddDynamic(this, &UOptionsUMG::OnClickDefaultButton);
	DefaultButton->OnHovered.AddDynamic(this, &UOptionsUMG::OnHorverDefaultButton);

	BackButton->OnClicked.AddDynamic(this, &UOptionsUMG::OnClickBackButton);
	BackButton->OnHovered.AddDynamic(this, &UOptionsUMG::OnHorverBackButton);

	OnVisibilityChanged.AddDynamic(this, &UOptionsUMG::OnVisibilityChangedFunction);


	TrapperGameInstance = Cast<UTrapperGameInstance>(GetGameInstance());
	if (TrapperGameInstance.Get())
	{
		ControlsUMG->ImportData(TrapperGameInstance.Get(), this);
		SoundsUMG->ImportData(TrapperGameInstance.Get(), this);

		GraphicsUMG->GraphicOptionInitialize();
		GraphicsUMG->ImportData(TrapperGameInstance.Get(), this);
		GraphicsUMG->SetDefalutValue();
	}

	ControlsUMG->SetSounds(MouseClickSound, MouseHoverSound);
	SoundsUMG->SetSounds(MouseClickSound, MouseHoverSound);
	GraphicsUMG->SetSounds(MouseClickSound, MouseHoverSound);

	FocusButton = ControlsButton;
}

void UOptionsUMG::OnClickControlsButton()
{
	UGameplayStatics::PlaySound2D(this, MouseClickSound);

	ControlsUMG->SetVisibility(ESlateVisibility::Visible);
	SoundsUMG->SetVisibility(ESlateVisibility::Collapsed);
	GraphicsUMG->SetVisibility(ESlateVisibility::Collapsed);

	if (FocusButton != ControlsButton)
	{
		UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(ControlsButton->Slot);
		FButtonStyle Style = ControlsButton->GetStyle();
		Style.Normal.SetResourceObject(ControlButtonClickedTexture);
		Style.Hovered.SetResourceObject(ControlButtonClickedTexture);
		Style.Hovered.SetImageSize(Style.Hovered.GetImageSize());
		Style.Pressed.SetResourceObject(ControlButtonClickedTexture);
		Style.Pressed.SetImageSize(Style.Pressed.GetImageSize());
		ControlsButton->SetStyle(Style);
	}

	if (FocusButton == SoundsButton)
	{
		FButtonStyle Style = SoundsButton->GetStyle();
		Style.Normal.SetResourceObject(SoundButtonNormalTexture);
		Style.Hovered.SetResourceObject(SoundButtonHoveredTexture);
		Style.Hovered.SetImageSize(Style.Hovered.GetImageSize());
		Style.Pressed.SetResourceObject(SoundButtonClickedTexture);
		Style.Pressed.SetImageSize(Style.Pressed.GetImageSize());
		SoundsButton->SetStyle(Style);
	}
	else if (FocusButton == GraphicsButton)
	{
		FButtonStyle Style = GraphicsButton->GetStyle();
		Style.Normal.SetResourceObject(GraphicButtonNormalTexture);
		Style.Hovered.SetResourceObject(GraphicButtonHoveredTexture);
		Style.Hovered.SetImageSize(Style.Hovered.GetImageSize());
		Style.Pressed.SetResourceObject(GraphicButtonClickedTexture);
		Style.Pressed.SetImageSize(Style.Pressed.GetImageSize());
		GraphicsButton->SetStyle(Style);
	}

	FocusButton = ControlsButton;
}

void UOptionsUMG::OnClickSoundsButton()
{
	UGameplayStatics::PlaySound2D(this, MouseClickSound);

	ControlsUMG->SetVisibility(ESlateVisibility::Collapsed);
	SoundsUMG->SetVisibility(ESlateVisibility::Visible);
	GraphicsUMG->SetVisibility(ESlateVisibility::Collapsed);

	if (FocusButton != SoundsButton)
	{
		UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(SoundsButton->Slot);
		FButtonStyle Style = SoundsButton->GetStyle();
		Style.Normal.SetResourceObject(SoundButtonClickedTexture);
		Style.Hovered.SetResourceObject(SoundButtonClickedTexture);
		Style.Hovered.SetImageSize(Style.Hovered.GetImageSize());
		Style.Pressed.SetResourceObject(SoundButtonClickedTexture);
		Style.Pressed.SetImageSize(Style.Pressed.GetImageSize());
		SoundsButton->SetStyle(Style);
	}

	if (FocusButton == ControlsButton)
	{
		FButtonStyle Style = ControlsButton->GetStyle();
		Style.Normal.SetResourceObject(ControlButtonNormalTexture);
		Style.Hovered.SetResourceObject(ControlButtonHoveredTexture);
		Style.Hovered.SetImageSize(Style.Hovered.GetImageSize());
		Style.Pressed.SetResourceObject(ControlButtonClickedTexture);
		Style.Pressed.SetImageSize(Style.Pressed.GetImageSize());
		ControlsButton->SetStyle(Style);
	}
	else if (FocusButton == GraphicsButton)
	{
		FButtonStyle Style = GraphicsButton->GetStyle();
		Style.Normal.SetResourceObject(GraphicButtonNormalTexture);
		Style.Hovered.SetResourceObject(GraphicButtonHoveredTexture);
		Style.Hovered.SetImageSize(Style.Hovered.GetImageSize());
		Style.Pressed.SetResourceObject(GraphicButtonClickedTexture);
		Style.Pressed.SetImageSize(Style.Pressed.GetImageSize());
		GraphicsButton->SetStyle(Style);
	}

	FocusButton = SoundsButton;
}

void UOptionsUMG::OnClickGraphicsButton()
{
	UGameplayStatics::PlaySound2D(this, MouseClickSound);

	ControlsUMG->SetVisibility(ESlateVisibility::Collapsed);
	SoundsUMG->SetVisibility(ESlateVisibility::Collapsed);
	GraphicsUMG->SetVisibility(ESlateVisibility::Visible);

	if (FocusButton != GraphicsButton)
	{
		FButtonStyle Style = GraphicsButton->GetStyle();
		Style.Normal.SetResourceObject(GraphicButtonClickedTexture);
		Style.Hovered.SetResourceObject(GraphicButtonClickedTexture);
		Style.Hovered.SetImageSize(Style.Hovered.GetImageSize());
		Style.Pressed.SetResourceObject(GraphicButtonClickedTexture);
		Style.Pressed.SetImageSize(Style.Pressed.GetImageSize());
		GraphicsButton->SetStyle(Style);
	}

	if (FocusButton == ControlsButton)
	{
		FButtonStyle Style = ControlsButton->GetStyle();
		Style.Normal.SetResourceObject(ControlButtonNormalTexture);
		Style.Hovered.SetResourceObject(ControlButtonHoveredTexture);
		Style.Hovered.SetImageSize(Style.Hovered.GetImageSize());
		Style.Pressed.SetResourceObject(ControlButtonClickedTexture);
		Style.Pressed.SetImageSize(Style.Pressed.GetImageSize());
		ControlsButton->SetStyle(Style);
		//ChangeButtonPosition(ControlsButton, false);
	}
	else if (FocusButton == SoundsButton)
	{
		FButtonStyle Style = SoundsButton->GetStyle();
		Style.Normal.SetResourceObject(SoundButtonNormalTexture);
		Style.Hovered.SetResourceObject(SoundButtonHoveredTexture);
		Style.Hovered.SetImageSize(Style.Hovered.GetImageSize());
		Style.Pressed.SetResourceObject(SoundButtonClickedTexture);
		Style.Pressed.SetImageSize(Style.Pressed.GetImageSize());
		SoundsButton->SetStyle(Style);
		//ChangeButtonPosition(SoundsButton, false);
	}

	FocusButton = GraphicsButton;
}

void UOptionsUMG::OnClickApplyButton()
{
	UGameplayStatics::PlaySound2D(this, MouseClickSound);

	bool bOnChangedValue = false;
	if (bIsChangeControlOptions)
	{
		bIsChangeControlOptions = false;
		ControlsUMG->OnApplyButton();
		bOnChangedValue = true;
	}
	if (bIsChangeSoundOptions)
	{
		bIsChangeSoundOptions = false;
		SoundsUMG->OnApplyButton();
		bOnChangedValue = true;
	}
	if (bIsChangeGraphicOptions)
	{
		bIsChangeGraphicOptions = false;
		GraphicsUMG->OnApplyButton();
		bOnChangedValue = true;
	}

	if (bOnChangedValue)
	{
		TrapperGameInstance->SaveSettings();
	}
}

void UOptionsUMG::OnClickDefaultButton()
{
	UGameplayStatics::PlaySound2D(this, MouseClickSound);

	if (ControlsUMG->GetVisibility() == ESlateVisibility::Visible)
	{
		ControlsUMG->OnDefaultButton();
	}
	else if (SoundsUMG->GetVisibility() == ESlateVisibility::Visible)
	{
		SoundsUMG->OnDefaultButton();
	}
	else if (GraphicsUMG->GetVisibility() == ESlateVisibility::Visible)
	{
		GraphicsUMG->OnDefaultButton();
	}
}

void UOptionsUMG::OnClickBackButton()
{
	UGameplayStatics::PlaySound2D(this, MouseClickSound);

	if (TitleCanvas)
	{
		TitleCanvas->SetVisibility(ESlateVisibility::Collapsed);
	}
	else
	{
		SetVisibility(ESlateVisibility::Collapsed);
	}

	if (TrapperImage)
	{
		TrapperImage->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}

	if (TitleGameExitButton.Get())
	{
		TitleGameExitButton->SetVisibility(ESlateVisibility::Visible);
	}

	if (TitleLogoutButton.Get())
	{
		TitleLogoutButton->SetVisibility(ESlateVisibility::Visible);
	}

	if (TitleCreditButton.Get())
	{
		TitleCreditButton->SetVisibility(ESlateVisibility::Visible);
	}
}

void UOptionsUMG::OnHorverControlsButton()
{
	UGameplayStatics::PlaySound2D(this, MouseHoverSound);
}

void UOptionsUMG::OnHorverSoundsButton()
{
	UGameplayStatics::PlaySound2D(this, MouseHoverSound);
}

void UOptionsUMG::OnHorverGraphicsButton()
{
	UGameplayStatics::PlaySound2D(this, MouseHoverSound);
}

void UOptionsUMG::OnHorverApplyButton()
{
	UGameplayStatics::PlaySound2D(this, MouseHoverSound);
}

void UOptionsUMG::OnHorverDefaultButton()
{
	UGameplayStatics::PlaySound2D(this, MouseHoverSound);
}

void UOptionsUMG::OnHorverBackButton()
{
	UGameplayStatics::PlaySound2D(this, MouseHoverSound);
}

void UOptionsUMG::OnVisibilityChangedFunction(ESlateVisibility InVisibility)
{
	//UE_LOG(LogTrap, Warning, TEXT("UOptionsUMG::OnVisibilityChangedFunction"));

	if (InVisibility == ESlateVisibility::Collapsed)
	{
		if (bIsChangeControlOptions)
		{
			//UE_LOG(LogTrap, Warning, TEXT("bIsChangeControlOptions true"));
			bIsChangeControlOptions = false;
			ControlsUMG->ImportData();
		}
		if (bIsChangeSoundOptions)
		{
			//UE_LOG(LogTrap, Warning, TEXT("bIsChangeSoundOptions true"));
			bIsChangeSoundOptions = false;
			SoundsUMG->ImportData();
		}
		if (bIsChangeGraphicOptions)
		{
			//UE_LOG(LogTrap, Warning, TEXT("bIsChangeGraphicOptions true"));
			bIsChangeGraphicOptions = false;
			GraphicsUMG->ImportData();
		}
	}
}

void UOptionsUMG::SetTitleGameExitButton(UButton* Button)
{
	TitleGameExitButton = Button;
}

void UOptionsUMG::SetTitleLogoutButton(UButton* Button)
{
	TitleLogoutButton = Button;
}

void UOptionsUMG::SetTitleCreditButton(UButton* Button)
{
	TitleCreditButton = Button;
}

//void UOptionsUMG::ChangeButtonPosition(class UButton* Button, bool Up)
//{
//	if (Button)
//	{
//		UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(Button->Slot);
//
//		if (CanvasSlot)
//		{
//			// 현재 버튼의 위치를 가져옵니다.
//			FVector2D CurrentPosition = CanvasSlot->GetPosition();
//			FVector2D NewPosition = CurrentPosition + FVector2D(0.0f, Up ? -10.f : 10.f);
//			CanvasSlot->SetPosition(NewPosition);
//		}
//	}
//}

void UOptionsUMG::ImportData()
{
	ControlsUMG->ImportData();
	SoundsUMG->ImportData();
	GraphicsUMG->ImportData();
}