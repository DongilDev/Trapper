// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/Options/GraphicsUMG.h"
#include "Widget/Options/OptionsUMG.h"
#include "Components/Button.h"
#include "Components/EditableText.h"
#include "Components/TextBlock.h"
#include "Components/Slider.h"
#include "Components/Image.h"
#include "Components/Border.h"
#include "Components/CanvasPanel.h"
#include "Components/ComboBoxString.h"
#include "TrapperProject.h"
#include "TrapperGameInstance.h"
#include "Misc/DefaultValueHelper.h"
#include "GameFramework/InputSettings.h"
#include "Character/TrapperPlayer.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/GameUserSettings.h"
#include "Options.h"
#include "Engine/Console.h"


void UGraphicsUMG::NativeConstruct()
{
	Super::NativeConstruct();

	DisplayMode->OnHovered.AddDynamic(this, &UGraphicsUMG::OnHorverDisplayMode);
	Resolution->OnHovered.AddDynamic(this, &UGraphicsUMG::OnHorverResolution);
	Texture->OnHovered.AddDynamic(this, &UGraphicsUMG::OnHorverTexture);
	Shadow->OnHovered.AddDynamic(this, &UGraphicsUMG::OnHorverShadow);
	Effect->OnHovered.AddDynamic(this, &UGraphicsUMG::OnHorverEffect);
	PostProcessing->OnHovered.AddDynamic(this, &UGraphicsUMG::OnHorverPostProcessing);
	VSyncEnabled->OnHovered.AddDynamic(this, &UGraphicsUMG::OnHorverVSyncEnabled);
	ScreenShake->OnHovered.AddDynamic(this, &UGraphicsUMG::OnHorverScreenShake);
	Antialiasing->OnHovered.AddDynamic(this, &UGraphicsUMG::OnHorverAntialiasing);
	FrameRateLimit->OnHovered.AddDynamic(this, &UGraphicsUMG::OnHorverFrameRateLimit);
	ScreenBrightness->OnHovered.AddDynamic(this, &UGraphicsUMG::OnHorverScreenBrightness);

	DisplayModeComboBox->OnSelectionChanged.AddDynamic(this, &UGraphicsUMG::OnDisplayModeComboBoxSelectionChanged);
	ResolutionComboBox->OnSelectionChanged.AddDynamic(this, &UGraphicsUMG::OnResolutionModeComboBoxSelectionChanged);

	TextureLowButton->OnClicked.AddDynamic(this, &UGraphicsUMG::OnClickTextureLowButton);
	TextureMediumButton->OnClicked.AddDynamic(this, &UGraphicsUMG::OnClickTextureMediumButton);
	TextureHighButton->OnClicked.AddDynamic(this, &UGraphicsUMG::OnClickTextureHighButton);
	TextureEpicButton->OnClicked.AddDynamic(this, &UGraphicsUMG::OnClickTextureEpicButton);

	ShadowLowButton->OnClicked.AddDynamic(this, &UGraphicsUMG::OnClickShadowLowButton);
	ShadowMediumButton->OnClicked.AddDynamic(this, &UGraphicsUMG::OnClickShadowMediumButton);
	ShadowHighButton->OnClicked.AddDynamic(this, &UGraphicsUMG::OnClickShadowHighButton);
	ShadowEpicButton->OnClicked.AddDynamic(this, &UGraphicsUMG::OnClickShadowEpicButton);

	EffectLowButton->OnClicked.AddDynamic(this, &UGraphicsUMG::OnClickEffectLowButton);
	EffectMediumButton->OnClicked.AddDynamic(this, &UGraphicsUMG::OnClickEffectMediumButton);
	EffectHighButton->OnClicked.AddDynamic(this, &UGraphicsUMG::OnClickEffectHighButton);
	EffectEpicButton->OnClicked.AddDynamic(this, &UGraphicsUMG::OnClickEffectEpicButton);

	PostProcessingLowButton->OnClicked.AddDynamic(this, &UGraphicsUMG::OnClickPostProcessingLowButton);
	PostProcessingMediumButton->OnClicked.AddDynamic(this, &UGraphicsUMG::OnClickPostProcessingMediumButton);
	PostProcessingHighButton->OnClicked.AddDynamic(this, &UGraphicsUMG::OnClickPostProcessingHighButton);
	PostProcessingEpicButton->OnClicked.AddDynamic(this, &UGraphicsUMG::OnClickPostProcessingEpicButton);

	VSyncEnabledOnButton->OnClicked.AddDynamic(this, &UGraphicsUMG::OnClickVSyncEnabledOnButton);
	VSyncEnabledOffButton->OnClicked.AddDynamic(this, &UGraphicsUMG::OnClickVSyncEnabledOffButton);

	ScreenShakeOnButton->OnClicked.AddDynamic(this, &UGraphicsUMG::OnClickScreenShakeOnButton);
	ScreenShakeOffButton->OnClicked.AddDynamic(this, &UGraphicsUMG::OnClickScreenShakeOffButton);

	AntialiasingOffButton->OnClicked.AddDynamic(this, &UGraphicsUMG::OnClickAntialiasingOffButton);
	AntialiasingFXAAButton->OnClicked.AddDynamic(this, &UGraphicsUMG::OnClickAntialiasingFXAAButton);
	AntialiasingTAAButton->OnClicked.AddDynamic(this, &UGraphicsUMG::OnClickAntialiasingTAAButton);
	AntialiasingMSAAButton->OnClicked.AddDynamic(this, &UGraphicsUMG::OnClickAntialiasingMSAAButton);
	AntialiasingTSRButton->OnClicked.AddDynamic(this, &UGraphicsUMG::OnClickAntialiasingTSRButton);

	FrameRateLimitSlider->OnValueChanged.AddDynamic(this, &UGraphicsUMG::OnFrameRateLimitSlider);
	FrameRateLimitNumericalText->OnTextCommitted.AddDynamic(this, &UGraphicsUMG::OnFrameRateLimitNumericalCommited);

	ScreenBrightnessSlider->OnValueChanged.AddDynamic(this, &UGraphicsUMG::OnScreenBrightnessSlider);
	ScreenBrightnessNumericalText->OnTextCommitted.AddDynamic(this, &UGraphicsUMG::OnScreenBrightnessNumericalCommited);

	FocusButton = DisplayMode;
	FocusImage = DisplayModeBackground;
}

FReply UGraphicsUMG::NativeOnPreviewKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	if (InKeyEvent.GetKey() == EKeys::Tab)
	{
		return FReply::Handled();
	}

	return Super::NativeOnPreviewKeyDown(InGeometry, InKeyEvent);
}

void UGraphicsUMG::ImportData(UTrapperGameInstance* GameInstance, UOptionsUMG* InOptionsUMG)
{
	TrapperGameInstance = GameInstance;
	OptionsUMG = InOptionsUMG;
	ImportData();
	ApplyData();
}

void UGraphicsUMG::ImportData()
{
	bIsChangeOptions = false;

	FGraphicSettings InGraphicSettings = TrapperGameInstance->GetGraphicSettings();

	// DisPlay
	EDisplayMode GraphicDisplayMode = InGraphicSettings.DisplayMode;
	FString DisPlayModeString;
	switch (GraphicDisplayMode)
	{
	case EDisplayMode::FullScreen:
		DisPlayModeString = "Full Screen";
		GraphicSettings.DisplayMode = EDisplayMode::FullScreen;
		break;
	case EDisplayMode::WindowedFullScreen:
		DisPlayModeString = "Windowed Full Screen";
		GraphicSettings.DisplayMode = EDisplayMode::WindowedFullScreen;
		break;
	case EDisplayMode::Window:
		DisPlayModeString = "Window";
		GraphicSettings.DisplayMode = EDisplayMode::Window;
		break;
	}
	DisplayModeComboBox->SetSelectedOption(DisPlayModeString);

	// Resolution
	int32 ScreenWidth = InGraphicSettings.ScreenWidth;
	int32 ScreenHeight = InGraphicSettings.ScreenHeight;
	FString ResolutionString = FString::Printf(TEXT("%d x %d"), ScreenWidth, ScreenHeight);
	ResolutionComboBox->SetSelectedOption(ResolutionString);
	GraphicSettings.ScreenWidth = ScreenWidth;
	GraphicSettings.ScreenHeight = ScreenHeight;

	FIntPoint DesktopResolution = GEngine->GameUserSettings->GetDesktopResolution();
	ResolutionString = FString::Printf(TEXT("%d x %d"), DesktopResolution.X, DesktopResolution.Y);
	WindowedFullScreenResolutionComboBox->AddOption(ResolutionString);
	WindowedFullScreenResolutionComboBox->SetSelectedOption(ResolutionString);

	// Texture
	int32 TextureQuality = InGraphicSettings.TextureQuality;
	if (TextureQuality == 0)
	{
		OnClickTextureLowButton();

		FButtonStyle Style = TextureLowButton->GetStyle();
		Style.Normal.SetResourceObject(LowActive);
		Style.Hovered.SetResourceObject(LowActive);
		Style.Hovered.SetImageSize(Style.Hovered.GetImageSize());
		Style.Pressed.SetResourceObject(LowActive);
		Style.Pressed.SetImageSize(Style.Pressed.GetImageSize());
		TextureLowButton->SetStyle(Style);
		TextureFocusButton = TextureLowButton;
	}
	else if (TextureQuality == 1)
	{
		OnClickTextureMediumButton();

		FButtonStyle Style = TextureMediumButton->GetStyle();
		Style.Normal.SetResourceObject(MediumActive);
		Style.Hovered.SetResourceObject(MediumActive);
		Style.Hovered.SetImageSize(Style.Hovered.GetImageSize());
		Style.Pressed.SetResourceObject(MediumActive);
		Style.Pressed.SetImageSize(Style.Pressed.GetImageSize());
		TextureMediumButton->SetStyle(Style);
		TextureFocusButton = TextureMediumButton;
	}
	else if (TextureQuality == 2)
	{
		OnClickTextureHighButton();

		FButtonStyle Style = TextureHighButton->GetStyle();
		Style.Normal.SetResourceObject(HighActive);
		Style.Hovered.SetResourceObject(HighActive);
		Style.Hovered.SetImageSize(Style.Hovered.GetImageSize());
		Style.Pressed.SetResourceObject(HighActive);
		Style.Pressed.SetImageSize(Style.Pressed.GetImageSize());
		TextureHighButton->SetStyle(Style);
		TextureFocusButton = TextureHighButton;
	}
	else if (TextureQuality == 3)
	{
		OnClickTextureEpicButton();

		FButtonStyle Style = TextureEpicButton->GetStyle();
		Style.Normal.SetResourceObject(BestActive);
		Style.Hovered.SetResourceObject(BestActive);
		Style.Hovered.SetImageSize(Style.Hovered.GetImageSize());
		Style.Pressed.SetResourceObject(BestActive);
		Style.Pressed.SetImageSize(Style.Pressed.GetImageSize());
		TextureEpicButton->SetStyle(Style);
		TextureFocusButton = TextureEpicButton;
	}

	// Shadow
	int32 ShadowQuality = InGraphicSettings.ShadowQuality;
	if (ShadowQuality == 0)
	{
		OnClickShadowLowButton();

		FButtonStyle Style = ShadowLowButton->GetStyle();
		Style.Normal.SetResourceObject(LowActive);
		Style.Hovered.SetResourceObject(LowActive);
		Style.Hovered.SetImageSize(Style.Hovered.GetImageSize());
		Style.Pressed.SetResourceObject(LowActive);
		Style.Pressed.SetImageSize(Style.Pressed.GetImageSize());
		ShadowLowButton->SetStyle(Style);
		ShadowFocusButton = ShadowLowButton;
	}
	else if (ShadowQuality == 1)
	{
		OnClickShadowMediumButton();

		FButtonStyle Style = ShadowMediumButton->GetStyle();
		Style.Normal.SetResourceObject(MediumActive);
		Style.Hovered.SetResourceObject(MediumActive);
		Style.Hovered.SetImageSize(Style.Hovered.GetImageSize());
		Style.Pressed.SetResourceObject(MediumActive);
		Style.Pressed.SetImageSize(Style.Pressed.GetImageSize());
		ShadowMediumButton->SetStyle(Style);
		ShadowFocusButton = ShadowMediumButton;
	}
	else if (ShadowQuality == 2)
	{
		OnClickShadowHighButton();

		FButtonStyle Style = ShadowHighButton->GetStyle();
		Style.Normal.SetResourceObject(HighActive);
		Style.Hovered.SetResourceObject(HighActive);
		Style.Hovered.SetImageSize(Style.Hovered.GetImageSize());
		Style.Pressed.SetResourceObject(HighActive);
		Style.Pressed.SetImageSize(Style.Pressed.GetImageSize());
		ShadowLowButton->SetStyle(Style);
		ShadowFocusButton = ShadowLowButton;
	}
	else if (ShadowQuality == 3)
	{
		OnClickShadowEpicButton();

		FButtonStyle Style = ShadowEpicButton->GetStyle();
		Style.Normal.SetResourceObject(BestActive);
		Style.Hovered.SetResourceObject(BestActive);
		Style.Hovered.SetImageSize(Style.Hovered.GetImageSize());
		Style.Pressed.SetResourceObject(BestActive);
		Style.Pressed.SetImageSize(Style.Pressed.GetImageSize());
		ShadowEpicButton->SetStyle(Style);
		ShadowFocusButton = ShadowEpicButton;
	}

	// Effect
	int32 EffectQuality = InGraphicSettings.EffectQuality;
	if (EffectQuality == 0)
	{
		OnClickEffectLowButton();

		FButtonStyle Style = EffectLowButton->GetStyle();
		Style.Normal.SetResourceObject(LowActive);
		Style.Hovered.SetResourceObject(LowActive);
		Style.Hovered.SetImageSize(Style.Hovered.GetImageSize());
		Style.Pressed.SetResourceObject(LowActive);
		Style.Pressed.SetImageSize(Style.Pressed.GetImageSize());
		EffectLowButton->SetStyle(Style);
		EffectFocusButton = EffectLowButton;
	}
	else if (EffectQuality == 1)
	{
		OnClickEffectMediumButton();

		FButtonStyle Style = EffectLowButton->GetStyle();
		Style.Normal.SetResourceObject(MediumActive);
		Style.Hovered.SetResourceObject(MediumActive);
		Style.Hovered.SetImageSize(Style.Hovered.GetImageSize());
		Style.Pressed.SetResourceObject(MediumActive);
		Style.Pressed.SetImageSize(Style.Pressed.GetImageSize());
		EffectLowButton->SetStyle(Style);
		EffectFocusButton = EffectLowButton;
	}
	else if (EffectQuality == 2)
	{
		OnClickEffectHighButton();

		FButtonStyle Style = EffectLowButton->GetStyle();
		Style.Normal.SetResourceObject(HighActive);
		Style.Hovered.SetResourceObject(HighActive);
		Style.Hovered.SetImageSize(Style.Hovered.GetImageSize());
		Style.Pressed.SetResourceObject(HighActive);
		Style.Pressed.SetImageSize(Style.Pressed.GetImageSize());
		EffectLowButton->SetStyle(Style);
		EffectFocusButton = EffectLowButton;
	}
	else if (EffectQuality == 3)
	{
		OnClickEffectEpicButton();

		FButtonStyle Style = EffectEpicButton->GetStyle();
		Style.Normal.SetResourceObject(BestActive);
		Style.Hovered.SetResourceObject(BestActive);
		Style.Hovered.SetImageSize(Style.Hovered.GetImageSize());
		Style.Pressed.SetResourceObject(BestActive);
		Style.Pressed.SetImageSize(Style.Pressed.GetImageSize());
		EffectEpicButton->SetStyle(Style);
		EffectFocusButton = EffectEpicButton;
	}

	// PostProcessing
	int32 PostProcessingQuality = InGraphicSettings.PostProcessingQuality;
	if (PostProcessingQuality == 0)
	{
		OnClickPostProcessingLowButton();

		FButtonStyle Style = PostProcessingLowButton->GetStyle();
		Style.Normal.SetResourceObject(LowActive);
		Style.Hovered.SetResourceObject(LowActive);
		Style.Hovered.SetImageSize(Style.Hovered.GetImageSize());
		Style.Pressed.SetResourceObject(LowActive);
		Style.Pressed.SetImageSize(Style.Pressed.GetImageSize());
		PostProcessingLowButton->SetStyle(Style);
		PostProcessingFocusButton = PostProcessingLowButton;
	}
	else if (PostProcessingQuality == 1)
	{
		OnClickPostProcessingMediumButton();

		FButtonStyle Style = PostProcessingMediumButton->GetStyle();
		Style.Normal.SetResourceObject(MediumActive);
		Style.Hovered.SetResourceObject(MediumActive);
		Style.Hovered.SetImageSize(Style.Hovered.GetImageSize());
		Style.Pressed.SetResourceObject(MediumActive);
		Style.Pressed.SetImageSize(Style.Pressed.GetImageSize());
		PostProcessingMediumButton->SetStyle(Style);
		PostProcessingFocusButton = PostProcessingMediumButton;
	}
	else if (PostProcessingQuality == 2)
	{
		OnClickPostProcessingHighButton();

		FButtonStyle Style = PostProcessingHighButton->GetStyle();
		Style.Normal.SetResourceObject(HighActive);
		Style.Hovered.SetResourceObject(HighActive);
		Style.Hovered.SetImageSize(Style.Hovered.GetImageSize());
		Style.Pressed.SetResourceObject(HighActive);
		Style.Pressed.SetImageSize(Style.Pressed.GetImageSize());
		PostProcessingHighButton->SetStyle(Style);
		PostProcessingFocusButton = PostProcessingHighButton;
	}
	else if (PostProcessingQuality == 3)
	{
		OnClickPostProcessingEpicButton();

		FButtonStyle Style = PostProcessingEpicButton->GetStyle();
		Style.Normal.SetResourceObject(BestActive);
		Style.Hovered.SetResourceObject(BestActive);
		Style.Hovered.SetImageSize(Style.Hovered.GetImageSize());
		Style.Pressed.SetResourceObject(BestActive);
		Style.Pressed.SetImageSize(Style.Pressed.GetImageSize());
		PostProcessingEpicButton->SetStyle(Style);
		PostProcessingFocusButton = PostProcessingEpicButton;
	}

	// VSyncEnabled
	bool bVSyncEnabled = InGraphicSettings.bVSyncEnabled;
	if (bVSyncEnabled)
	{
		OnClickVSyncEnabledOnButton();

		FButtonStyle Style = VSyncEnabledOnButton->GetStyle();
		Style.Normal.SetResourceObject(OnActive);
		Style.Hovered.SetResourceObject(OnActive);
		Style.Hovered.SetImageSize(Style.Hovered.GetImageSize());
		Style.Pressed.SetResourceObject(OnActive);
		Style.Pressed.SetImageSize(Style.Pressed.GetImageSize());
		VSyncEnabledOnButton->SetStyle(Style);
		VSyncEnabledFocusButton = VSyncEnabledOnButton;
	}
	else
	{
		OnClickVSyncEnabledOffButton();

		FButtonStyle Style = VSyncEnabledOffButton->GetStyle();
		Style.Normal.SetResourceObject(OffActive);
		Style.Hovered.SetResourceObject(OffActive);
		Style.Hovered.SetImageSize(Style.Hovered.GetImageSize());
		Style.Pressed.SetResourceObject(OffActive);
		Style.Pressed.SetImageSize(Style.Pressed.GetImageSize());
		VSyncEnabledOffButton->SetStyle(Style);
		VSyncEnabledFocusButton = VSyncEnabledOffButton;
	}

	// ScreenShake
	bool bScreenShake = InGraphicSettings.bScreenShake;
	if (bScreenShake)
	{
		OnClickScreenShakeOnButton();

		FButtonStyle Style = ScreenShakeOnButton->GetStyle();
		Style.Normal.SetResourceObject(OnActive);
		Style.Hovered.SetResourceObject(OnActive);
		Style.Hovered.SetImageSize(Style.Hovered.GetImageSize());
		Style.Pressed.SetResourceObject(OnActive);
		Style.Pressed.SetImageSize(Style.Pressed.GetImageSize());
		ScreenShakeOnButton->SetStyle(Style);
		ScreenShakeFocusButton = ScreenShakeOnButton;
	}
	else
	{
		OnClickScreenShakeOffButton();

		FButtonStyle Style = ScreenShakeOffButton->GetStyle();
		Style.Normal.SetResourceObject(OffActive);
		Style.Hovered.SetResourceObject(OffActive);
		Style.Hovered.SetImageSize(Style.Hovered.GetImageSize());
		Style.Pressed.SetResourceObject(OffActive);
		Style.Pressed.SetImageSize(Style.Pressed.GetImageSize());
		ScreenShakeOffButton->SetStyle(Style);
		ScreenShakeFocusButton = ScreenShakeOffButton;
	}

	// AntialiasingQuality
	int32 AntialiasingType = InGraphicSettings.AntialiasingType;
	if (AntialiasingType == 0)
	{
		OnClickAntialiasingOffButton();

		FButtonStyle Style = AntialiasingOffButton->GetStyle();
		Style.Normal.SetResourceObject(OffActive);
		Style.Hovered.SetResourceObject(OffActive);
		Style.Hovered.SetImageSize(Style.Hovered.GetImageSize());
		Style.Pressed.SetResourceObject(OffActive);
		Style.Pressed.SetImageSize(Style.Pressed.GetImageSize());
		AntialiasingOffButton->SetStyle(Style);
		AntialiasingFocusButton = AntialiasingOffButton;
	}
	else if (AntialiasingType == 1)
	{
		OnClickAntialiasingFXAAButton();

		FButtonStyle Style = AntialiasingFXAAButton->GetStyle();
		Style.Normal.SetResourceObject(FXAAActive);
		Style.Hovered.SetResourceObject(FXAAActive);
		Style.Hovered.SetImageSize(Style.Hovered.GetImageSize());
		Style.Pressed.SetResourceObject(FXAAActive);
		Style.Pressed.SetImageSize(Style.Pressed.GetImageSize());
		AntialiasingFXAAButton->SetStyle(Style);
		AntialiasingFocusButton = AntialiasingFXAAButton;
	}
	else if (AntialiasingType == 2)
	{
		OnClickAntialiasingTAAButton();

		FButtonStyle Style = AntialiasingTAAButton->GetStyle();
		Style.Normal.SetResourceObject(TAAActive);
		Style.Hovered.SetResourceObject(TAAActive);
		Style.Hovered.SetImageSize(Style.Hovered.GetImageSize());
		Style.Pressed.SetResourceObject(TAAActive);
		Style.Pressed.SetImageSize(Style.Pressed.GetImageSize());
		AntialiasingTAAButton->SetStyle(Style);
		AntialiasingFocusButton = AntialiasingTAAButton;
	}
	else if (AntialiasingType == 3)
	{
		OnClickAntialiasingMSAAButton();

		FButtonStyle Style = AntialiasingMSAAButton->GetStyle();
		Style.Normal.SetResourceObject(MSAAActive);
		Style.Hovered.SetResourceObject(MSAAActive);
		Style.Hovered.SetImageSize(Style.Hovered.GetImageSize());
		Style.Pressed.SetResourceObject(MSAAActive);
		Style.Pressed.SetImageSize(Style.Pressed.GetImageSize());
		AntialiasingMSAAButton->SetStyle(Style);
		AntialiasingFocusButton = AntialiasingMSAAButton;
	}
	else if (AntialiasingType == 4)
	{
		OnClickAntialiasingTSRButton();

		FButtonStyle Style = AntialiasingTSRButton->GetStyle();
		Style.Normal.SetResourceObject(TSRActive);
		Style.Hovered.SetResourceObject(TSRActive);
		Style.Hovered.SetImageSize(Style.Hovered.GetImageSize());
		Style.Pressed.SetResourceObject(TSRActive);
		Style.Pressed.SetImageSize(Style.Pressed.GetImageSize());
		AntialiasingTSRButton->SetStyle(Style);
		AntialiasingFocusButton = AntialiasingTSRButton;
	}

	FrameRateLimitSlider->SetValue(InGraphicSettings.FrameRateLimit);
	ScreenBrightnessSlider->SetValue(InGraphicSettings.ScreenBrightness);

	AllCollapsedWarningText();
}

void UGraphicsUMG::OnDefaultButton()
{
	bCanPlaySound = false;

	DisplayModeComboBox->SetSelectedOption(DefaultDisplayModeString);
	ResolutionComboBox->SetSelectedOption(DefaultResolutionString);

	// Texture
	if (DefaultTextureQuality == 0)
	{
		OnClickTextureLowButton();
	}
	else if (DefaultTextureQuality == 1)
	{
		OnClickTextureMediumButton();
	}
	else if (DefaultTextureQuality == 2)
	{
		OnClickTextureHighButton();
	}
	else if (DefaultTextureQuality == 3)
	{
		OnClickTextureEpicButton();
	}

	// Shadow
	if (DefaultShadowQuality == 0)
	{
		OnClickShadowLowButton();
	}
	else if (DefaultShadowQuality == 1)
	{
		OnClickShadowMediumButton();
	}
	else if (DefaultShadowQuality == 2)
	{
		OnClickShadowHighButton();
	}
	else if (DefaultShadowQuality == 3)
	{
		OnClickShadowEpicButton();
	}

	// Effect
	if (DefaultEffectQuality == 0)
	{
		OnClickEffectLowButton();
	}
	else if (DefaultEffectQuality == 1)
	{
		OnClickEffectMediumButton();
	}
	else if (DefaultEffectQuality == 2)
	{
		OnClickEffectHighButton();
	}
	else if (DefaultEffectQuality == 3)
	{
		OnClickEffectEpicButton();
	}

	// PostProcessing
	if (DefaultPostProcessingQuality == 0)
	{
		OnClickPostProcessingLowButton();
	}
	else if (DefaultPostProcessingQuality == 1)
	{
		OnClickPostProcessingMediumButton();
	}
	else if (DefaultPostProcessingQuality == 2)
	{
		OnClickPostProcessingHighButton();
	}
	else if (DefaultPostProcessingQuality == 3)
	{
		OnClickPostProcessingEpicButton();
	}

	// VSyncEnabled
	if (DefaultbVSyncEnabled)
	{
		OnClickVSyncEnabledOnButton();
	}
	else
	{
		OnClickVSyncEnabledOffButton();
	}

	// ScreenShake
	if (DefaultbScreenShake)
	{
		OnClickScreenShakeOnButton();
	}
	else
	{
		OnClickScreenShakeOffButton();
	}

	// AntialiasingQuality
	if (DefaultAntialiasingType == 0)
	{
		OnClickAntialiasingOffButton();
	}
	else if (DefaultAntialiasingType == 1)
	{
		OnClickAntialiasingFXAAButton();
	}
	else if (DefaultAntialiasingType == 2)
	{
		OnClickAntialiasingTAAButton();
	}
	else if (DefaultAntialiasingType == 3)
	{
		OnClickAntialiasingMSAAButton();
	}
	else if (DefaultAntialiasingType == 4)
	{
		OnClickAntialiasingTSRButton();
	}

	FrameRateLimitSlider->SetValue(120);
	ScreenBrightnessSlider->SetValue(50);

	bCanPlaySound = true;
}

void UGraphicsUMG::OnApplyButton()
{
	bIsChangeOptions = false;
	ApplyData();
	TrapperGameInstance->SaveGraphicsData(GraphicSettings);
}

void UGraphicsUMG::SetSounds(class USoundWave* ClickSound, class USoundWave* HorverSound)
{
	MouseClickSound = ClickSound;
	MouseHoverSound = HorverSound;
}

void UGraphicsUMG::GraphicOptionInitialize()
{
	TArray<FIntPoint> SupportedResolutions;
	FScreenResolutionArray ScreenResolutions;

	if (RHIGetAvailableResolutions(ScreenResolutions, true))
	{
		for (const FScreenResolutionRHI& ScreenResolution : ScreenResolutions)
		{
			float AspectRatio = static_cast<float>(ScreenResolution.Width) / static_cast<float>(ScreenResolution.Height);
			FIntPoint DesktopResolution = GEngine->GameUserSettings->GetDesktopResolution();
			uint32 DesktopWidth = DesktopResolution.X;
			uint32 DesktopHeight = DesktopResolution.Y;
			if (ScreenResolution.Width >= 1280 && ScreenResolution.Height >= 720 &&
				ScreenResolution.Width <= DesktopWidth && ScreenResolution.Height <= DesktopHeight &&
				FMath::IsNearlyEqual(AspectRatio, 16.0f / 9.0f, 0.01f))
			{
				SupportedResolutions.Add(FIntPoint(ScreenResolution.Width, ScreenResolution.Height));
			}
		}
	}

	if (ResolutionComboBox)
	{
		for (const FIntPoint& Res : SupportedResolutions)
		{
			FString ResolutionString = FString::Printf(TEXT("%d x %d"), Res.X, Res.Y);
			ResolutionComboBox->AddOption(ResolutionString);
		}

		/*if (SupportedResolutions.Num() > 0)
		{
			UGameUserSettings* GameUserSettings = UGameUserSettings::GetGameUserSettings();
			FIntPoint NewResolution = FIntPoint();
			if (GameUserSettings)
			{
				GameUserSettings->SetScreenResolution(NewResolution);
				NewResolution = GEngine->GameUserSettings->GetDesktopResolution();
			}

			ResolutionComboBox->SetSelectedOption(FString::Printf(TEXT("%d x %d"),
				NewResolution.X, NewResolution.Y));
		}*/
	}
}

void UGraphicsUMG::OnHorverDisplayMode()
{
	UGameplayStatics::PlaySound2D(this, MouseHoverSound);
	FocusHorverChange(DisplayMode, DisplayModeBackground);
}

void UGraphicsUMG::OnHorverResolution()
{
	UGameplayStatics::PlaySound2D(this, MouseHoverSound);
	FocusHorverChange(Resolution, ResolutionBackground);
}

void UGraphicsUMG::OnHorverTexture()
{
	UGameplayStatics::PlaySound2D(this, MouseHoverSound);
	FocusHorverChange(Texture, TextureBackground);
}

void UGraphicsUMG::OnHorverShadow()
{
	UGameplayStatics::PlaySound2D(this, MouseHoverSound);
	FocusHorverChange(Shadow, ShadowBackground);
}

void UGraphicsUMG::OnHorverEffect()
{
	UGameplayStatics::PlaySound2D(this, MouseHoverSound);
	FocusHorverChange(Effect, EffectBackground);
}

void UGraphicsUMG::OnHorverPostProcessing()
{
	UGameplayStatics::PlaySound2D(this, MouseHoverSound);
	FocusHorverChange(PostProcessing, PostProcessingBackground);
}

void UGraphicsUMG::OnHorverVSyncEnabled()
{
	UGameplayStatics::PlaySound2D(this, MouseHoverSound);
	FocusHorverChange(VSyncEnabled, VSyncEnabledBackground);
}

void UGraphicsUMG::OnHorverScreenShake()
{
	UGameplayStatics::PlaySound2D(this, MouseHoverSound);
	FocusHorverChange(ScreenShake, ScreenShakeBackground);
}

void UGraphicsUMG::OnHorverAntialiasing()
{
	UGameplayStatics::PlaySound2D(this, MouseHoverSound);
	FocusHorverChange(Antialiasing, AntialiasingBackground);
}

void UGraphicsUMG::OnHorverFrameRateLimit()
{
	UGameplayStatics::PlaySound2D(this, MouseHoverSound);
	FocusHorverChange(FrameRateLimit, FrameRateLimitBackground);
}

void UGraphicsUMG::OnHorverScreenBrightness()
{
	UGameplayStatics::PlaySound2D(this, MouseHoverSound);
	FocusHorverChange(ScreenBrightness, ScreenBrightnessBackground);
}

void UGraphicsUMG::OnDisplayModeComboBoxSelectionChanged(FString SelectedItem, ESelectInfo::Type SelectionType)
{
	if (bCanPlaySound)
	{
		UGameplayStatics::PlaySound2D(this, MouseClickSound);
	}
	OptionsUMG->SetIsChangeGraphicOptions(true);

	if (SelectedItem == "Full Screen")
	{
		GraphicSettings.DisplayMode = EDisplayMode::FullScreen;

		if (ResolutionComboBox->IsVisible())
		{
			ResolutionComboBox->SetVisibility(ESlateVisibility::Collapsed);
			WindowedFullScreenResolutionComboBox->SetVisibility(ESlateVisibility::Visible);
		}
	}
	else if (SelectedItem == "Windowed Full Screen")
	{
		GraphicSettings.DisplayMode = EDisplayMode::WindowedFullScreen;

		if (ResolutionComboBox->IsVisible())
		{
			ResolutionComboBox->SetVisibility(ESlateVisibility::Collapsed);
			WindowedFullScreenResolutionComboBox->SetVisibility(ESlateVisibility::Visible);
		}
	}
	else if (SelectedItem == "Window")
	{
		GraphicSettings.DisplayMode = EDisplayMode::Window;

		if (WindowedFullScreenResolutionComboBox->IsVisible())
		{
			ResolutionComboBox->SetVisibility(ESlateVisibility::Visible);
			WindowedFullScreenResolutionComboBox->SetVisibility(ESlateVisibility::Collapsed);
		}
	}

	DisplayModeWarningPanel->SetVisibility(ESlateVisibility::Visible);
	DisplayModeBackground->SetBrushResourceObject(BackgroundNotApply);
}

void UGraphicsUMG::OnResolutionModeComboBoxSelectionChanged(FString SelectedItem, ESelectInfo::Type SelectionType)
{
	if (bCanPlaySound)
	{
		UGameplayStatics::PlaySound2D(this, MouseClickSound);
	}
	OptionsUMG->SetIsChangeGraphicOptions(true);

	TArray<FString> ParsedResolution;
	SelectedItem.ParseIntoArray(ParsedResolution, TEXT("x"));

	if (ParsedResolution.Num() == 2)
	{
		int32 Width = FCString::Atoi(*ParsedResolution[0].TrimStartAndEnd());
		int32 Height = FCString::Atoi(*ParsedResolution[1].TrimStartAndEnd());

		GraphicSettings.ScreenWidth = Width;
		GraphicSettings.ScreenHeight = Height;
	}

	ResolutionWarningPanel->SetVisibility(ESlateVisibility::Visible);
	ResolutionBackground->SetBrushResourceObject(BackgroundNotApply);
}

void UGraphicsUMG::OnClickTextureLowButton()
{
	if (bCanPlaySound)
	{
		UGameplayStatics::PlaySound2D(this, MouseClickSound);
	}
	if (GraphicSettings.TextureQuality == 0)
	{
		return;
	}
	OptionsUMG->SetIsChangeGraphicOptions(true);

	GraphicSettings.TextureQuality = 0;

	TextureWarningPanel->SetVisibility(ESlateVisibility::Visible);
	TextureBackground->SetBrushResourceObject(BackgroundNotApply);

	if (TextureFocusButton != TextureLowButton)
	{
		FButtonStyle Style = TextureLowButton->GetStyle();
		Style.Normal.SetResourceObject(LowActive);
		Style.Hovered.SetResourceObject(LowActive);
		Style.Hovered.SetImageSize(Style.Hovered.GetImageSize());
		Style.Pressed.SetResourceObject(LowActive);
		Style.Pressed.SetImageSize(Style.Pressed.GetImageSize());
		TextureLowButton->SetStyle(Style);
	}

	if (TextureFocusButton == TextureMediumButton)
	{
		FButtonStyle Style = TextureMediumButton->GetStyle();
		Style.Normal.SetResourceObject(MediumNormal);
		Style.Hovered.SetResourceObject(MediumActive);
		Style.Hovered.SetImageSize(Style.Hovered.GetImageSize());
		Style.Pressed.SetResourceObject(MediumActive);
		Style.Pressed.SetImageSize(Style.Pressed.GetImageSize());
		TextureMediumButton->SetStyle(Style);
	}
	else if (TextureFocusButton == TextureHighButton)
	{
		FButtonStyle Style = TextureHighButton->GetStyle();
		Style.Normal.SetResourceObject(HighNormal);
		Style.Hovered.SetResourceObject(HighActive);
		Style.Hovered.SetImageSize(Style.Hovered.GetImageSize());
		Style.Pressed.SetResourceObject(HighActive);
		Style.Pressed.SetImageSize(Style.Pressed.GetImageSize());
		TextureHighButton->SetStyle(Style);
	}
	else if (TextureFocusButton == TextureEpicButton)
	{
		FButtonStyle Style = TextureEpicButton->GetStyle();
		Style.Normal.SetResourceObject(BestNormal);
		Style.Hovered.SetResourceObject(BestActive);
		Style.Hovered.SetImageSize(Style.Hovered.GetImageSize());
		Style.Pressed.SetResourceObject(BestActive);
		Style.Pressed.SetImageSize(Style.Pressed.GetImageSize());
		TextureEpicButton->SetStyle(Style);
	}

	TextureFocusButton = TextureLowButton;
}

void UGraphicsUMG::OnClickTextureMediumButton()
{
	if (bCanPlaySound)
	{
		UGameplayStatics::PlaySound2D(this, MouseClickSound);
	}
	if (GraphicSettings.TextureQuality == 1)
	{
		return;
	}
	OptionsUMG->SetIsChangeGraphicOptions(true);

	GraphicSettings.TextureQuality = 1;

	TextureWarningPanel->SetVisibility(ESlateVisibility::Visible);
	TextureBackground->SetBrushResourceObject(BackgroundNotApply);

	if (TextureFocusButton != TextureMediumButton)
	{
		FButtonStyle Style = TextureMediumButton->GetStyle();
		Style.Normal.SetResourceObject(MediumActive);
		Style.Hovered.SetResourceObject(MediumActive);
		Style.Hovered.SetImageSize(Style.Hovered.GetImageSize());
		Style.Pressed.SetResourceObject(MediumActive);
		Style.Pressed.SetImageSize(Style.Pressed.GetImageSize());
		TextureMediumButton->SetStyle(Style);
	}

	if (TextureFocusButton == TextureLowButton)
	{
		FButtonStyle Style = TextureLowButton->GetStyle();
		Style.Normal.SetResourceObject(LowNormal);
		Style.Hovered.SetResourceObject(LowActive);
		Style.Hovered.SetImageSize(Style.Hovered.GetImageSize());
		Style.Pressed.SetResourceObject(LowActive);
		Style.Pressed.SetImageSize(Style.Pressed.GetImageSize());
		TextureLowButton->SetStyle(Style);
	}
	else if (TextureFocusButton == TextureHighButton)
	{
		FButtonStyle Style = TextureHighButton->GetStyle();
		Style.Normal.SetResourceObject(HighNormal);
		Style.Hovered.SetResourceObject(HighActive);
		Style.Hovered.SetImageSize(Style.Hovered.GetImageSize());
		Style.Pressed.SetResourceObject(HighActive);
		Style.Pressed.SetImageSize(Style.Pressed.GetImageSize());
		TextureHighButton->SetStyle(Style);
	}
	else if (TextureFocusButton == TextureEpicButton)
	{
		FButtonStyle Style = TextureEpicButton->GetStyle();
		Style.Normal.SetResourceObject(BestNormal);
		Style.Hovered.SetResourceObject(BestActive);
		Style.Hovered.SetImageSize(Style.Hovered.GetImageSize());
		Style.Pressed.SetResourceObject(BestActive);
		Style.Pressed.SetImageSize(Style.Pressed.GetImageSize());
		TextureEpicButton->SetStyle(Style);
	}

	TextureFocusButton = TextureMediumButton;
}

void UGraphicsUMG::OnClickTextureHighButton()
{
	if (bCanPlaySound)
	{
		UGameplayStatics::PlaySound2D(this, MouseClickSound);
	}
	if (GraphicSettings.TextureQuality == 2)
	{
		return;
	}
	OptionsUMG->SetIsChangeGraphicOptions(true);

	GraphicSettings.TextureQuality = 2;

	TextureWarningPanel->SetVisibility(ESlateVisibility::Visible);
	TextureBackground->SetBrushResourceObject(BackgroundNotApply);

	if (TextureFocusButton != TextureHighButton)
	{
		FButtonStyle Style = TextureHighButton->GetStyle();
		Style.Normal.SetResourceObject(HighActive);
		Style.Hovered.SetResourceObject(HighActive);
		Style.Hovered.SetImageSize(Style.Hovered.GetImageSize());
		Style.Pressed.SetResourceObject(HighActive);
		Style.Pressed.SetImageSize(Style.Pressed.GetImageSize());
		TextureHighButton->SetStyle(Style);
	}

	if (TextureFocusButton == TextureLowButton)
	{
		FButtonStyle Style = TextureLowButton->GetStyle();
		Style.Normal.SetResourceObject(LowNormal);
		Style.Hovered.SetResourceObject(LowActive);
		Style.Hovered.SetImageSize(Style.Hovered.GetImageSize());
		Style.Pressed.SetResourceObject(LowActive);
		Style.Pressed.SetImageSize(Style.Pressed.GetImageSize());
		TextureLowButton->SetStyle(Style);
	}
	else if (TextureFocusButton == TextureMediumButton)
	{
		FButtonStyle Style = TextureMediumButton->GetStyle();
		Style.Normal.SetResourceObject(MediumNormal);
		Style.Hovered.SetResourceObject(MediumActive);
		Style.Hovered.SetImageSize(Style.Hovered.GetImageSize());
		Style.Pressed.SetResourceObject(MediumActive);
		Style.Pressed.SetImageSize(Style.Pressed.GetImageSize());
		TextureMediumButton->SetStyle(Style);
	}
	else if (TextureFocusButton == TextureEpicButton)
	{
		FButtonStyle Style = TextureEpicButton->GetStyle();
		Style.Normal.SetResourceObject(BestNormal);
		Style.Hovered.SetResourceObject(BestActive);
		Style.Hovered.SetImageSize(Style.Hovered.GetImageSize());
		Style.Pressed.SetResourceObject(BestActive);
		Style.Pressed.SetImageSize(Style.Pressed.GetImageSize());
		TextureEpicButton->SetStyle(Style);
	}

	TextureFocusButton = TextureHighButton;
}

void UGraphicsUMG::OnClickTextureEpicButton()
{
	if (bCanPlaySound)
	{
		UGameplayStatics::PlaySound2D(this, MouseClickSound);
	}
	if (GraphicSettings.TextureQuality == 3)
	{
		return;
	}
	OptionsUMG->SetIsChangeGraphicOptions(true);

	GraphicSettings.TextureQuality = 3;

	TextureWarningPanel->SetVisibility(ESlateVisibility::Visible);
	TextureBackground->SetBrushResourceObject(BackgroundNotApply);

	if (TextureFocusButton != TextureEpicButton)
	{
		FButtonStyle Style = TextureEpicButton->GetStyle();
		Style.Normal.SetResourceObject(BestActive);
		Style.Hovered.SetResourceObject(BestActive);
		Style.Hovered.SetImageSize(Style.Hovered.GetImageSize());
		Style.Pressed.SetResourceObject(BestActive);
		Style.Pressed.SetImageSize(Style.Pressed.GetImageSize());
		TextureEpicButton->SetStyle(Style);
	}

	if (TextureFocusButton == TextureLowButton)
	{
		FButtonStyle Style = TextureLowButton->GetStyle();
		Style.Normal.SetResourceObject(LowNormal);
		Style.Hovered.SetResourceObject(LowActive);
		Style.Hovered.SetImageSize(Style.Hovered.GetImageSize());
		Style.Pressed.SetResourceObject(LowActive);
		Style.Pressed.SetImageSize(Style.Pressed.GetImageSize());
		TextureLowButton->SetStyle(Style);
	}
	else if (TextureFocusButton == TextureMediumButton)
	{
		FButtonStyle Style = TextureMediumButton->GetStyle();
		Style.Normal.SetResourceObject(MediumNormal);
		Style.Hovered.SetResourceObject(MediumActive);
		Style.Hovered.SetImageSize(Style.Hovered.GetImageSize());
		Style.Pressed.SetResourceObject(MediumActive);
		Style.Pressed.SetImageSize(Style.Pressed.GetImageSize());
		TextureMediumButton->SetStyle(Style);
	}
	else if (TextureFocusButton == TextureHighButton)
	{
		FButtonStyle Style = TextureHighButton->GetStyle();
		Style.Normal.SetResourceObject(HighNormal);
		Style.Hovered.SetResourceObject(HighActive);
		Style.Hovered.SetImageSize(Style.Hovered.GetImageSize());
		Style.Pressed.SetResourceObject(HighActive);
		Style.Pressed.SetImageSize(Style.Pressed.GetImageSize());
		TextureHighButton->SetStyle(Style);
	}

	TextureFocusButton = TextureEpicButton;
}

void UGraphicsUMG::OnClickShadowLowButton()
{
	if (bCanPlaySound)
	{
		UGameplayStatics::PlaySound2D(this, MouseClickSound);
	}
	if (GraphicSettings.ShadowQuality == 0)
	{
		return;
	}
	OptionsUMG->SetIsChangeGraphicOptions(true);

	GraphicSettings.ShadowQuality = 0;

	ShadowWarningPanel->SetVisibility(ESlateVisibility::Visible);
	ShadowBackground->SetBrushResourceObject(BackgroundNotApply);

	if (ShadowFocusButton != ShadowLowButton)
	{
		FButtonStyle Style = ShadowLowButton->GetStyle();
		Style.Normal.SetResourceObject(LowActive);
		Style.Hovered.SetResourceObject(LowActive);
		Style.Hovered.SetImageSize(Style.Hovered.GetImageSize());
		Style.Pressed.SetResourceObject(LowActive);
		Style.Pressed.SetImageSize(Style.Pressed.GetImageSize());
		ShadowLowButton->SetStyle(Style);
	}

	if (ShadowFocusButton == ShadowMediumButton)
	{
		FButtonStyle Style = ShadowMediumButton->GetStyle();
		Style.Normal.SetResourceObject(MediumNormal);
		Style.Hovered.SetResourceObject(MediumActive);
		Style.Hovered.SetImageSize(Style.Hovered.GetImageSize());
		Style.Pressed.SetResourceObject(MediumActive);
		Style.Pressed.SetImageSize(Style.Pressed.GetImageSize());
		ShadowMediumButton->SetStyle(Style);
	}
	else if (ShadowFocusButton == ShadowHighButton)
	{
		FButtonStyle Style = ShadowHighButton->GetStyle();
		Style.Normal.SetResourceObject(HighNormal);
		Style.Hovered.SetResourceObject(HighActive);
		Style.Hovered.SetImageSize(Style.Hovered.GetImageSize());
		Style.Pressed.SetResourceObject(HighActive);
		Style.Pressed.SetImageSize(Style.Pressed.GetImageSize());
		ShadowHighButton->SetStyle(Style);
	}
	else if (ShadowFocusButton == ShadowEpicButton)
	{
		FButtonStyle Style = ShadowEpicButton->GetStyle();
		Style.Normal.SetResourceObject(BestNormal);
		Style.Hovered.SetResourceObject(BestActive);
		Style.Hovered.SetImageSize(Style.Hovered.GetImageSize());
		Style.Pressed.SetResourceObject(BestActive);
		Style.Pressed.SetImageSize(Style.Pressed.GetImageSize());
		ShadowEpicButton->SetStyle(Style);
	}

	ShadowFocusButton = ShadowLowButton;
}

void UGraphicsUMG::OnClickShadowMediumButton()
{
	if (bCanPlaySound)
	{
		UGameplayStatics::PlaySound2D(this, MouseClickSound);
	}
	if (GraphicSettings.ShadowQuality == 1)
	{
		return;
	}
	OptionsUMG->SetIsChangeGraphicOptions(true);

	GraphicSettings.ShadowQuality = 1;

	ShadowWarningPanel->SetVisibility(ESlateVisibility::Visible);
	ShadowBackground->SetBrushResourceObject(BackgroundNotApply);

	if (ShadowFocusButton != ShadowMediumButton)
	{
		FButtonStyle Style = ShadowMediumButton->GetStyle();
		Style.Normal.SetResourceObject(MediumActive);
		Style.Hovered.SetResourceObject(MediumActive);
		Style.Hovered.SetImageSize(Style.Hovered.GetImageSize());
		Style.Pressed.SetResourceObject(MediumActive);
		Style.Pressed.SetImageSize(Style.Pressed.GetImageSize());
		ShadowMediumButton->SetStyle(Style);
	}

	if (ShadowFocusButton == ShadowLowButton)
	{
		FButtonStyle Style = ShadowLowButton->GetStyle();
		Style.Normal.SetResourceObject(LowNormal);
		Style.Hovered.SetResourceObject(LowActive);
		Style.Hovered.SetImageSize(Style.Hovered.GetImageSize());
		Style.Pressed.SetResourceObject(LowActive);
		Style.Pressed.SetImageSize(Style.Pressed.GetImageSize());
		ShadowLowButton->SetStyle(Style);
	}
	else if (ShadowFocusButton == ShadowHighButton)
	{
		FButtonStyle Style = ShadowHighButton->GetStyle();
		Style.Normal.SetResourceObject(HighNormal);
		Style.Hovered.SetResourceObject(HighActive);
		Style.Hovered.SetImageSize(Style.Hovered.GetImageSize());
		Style.Pressed.SetResourceObject(HighActive);
		Style.Pressed.SetImageSize(Style.Pressed.GetImageSize());
		ShadowHighButton->SetStyle(Style);
	}
	else if (ShadowFocusButton == ShadowEpicButton)
	{
		FButtonStyle Style = ShadowEpicButton->GetStyle();
		Style.Normal.SetResourceObject(BestNormal);
		Style.Hovered.SetResourceObject(BestActive);
		Style.Hovered.SetImageSize(Style.Hovered.GetImageSize());
		Style.Pressed.SetResourceObject(BestActive);
		Style.Pressed.SetImageSize(Style.Pressed.GetImageSize());
		ShadowEpicButton->SetStyle(Style);
	}

	ShadowFocusButton = ShadowMediumButton;
}

void UGraphicsUMG::OnClickShadowHighButton()
{
	if (bCanPlaySound)
	{
		UGameplayStatics::PlaySound2D(this, MouseClickSound);
	}
	if (GraphicSettings.ShadowQuality == 2)
	{
		return;
	}
	OptionsUMG->SetIsChangeGraphicOptions(true);

	GraphicSettings.ShadowQuality = 2;

	ShadowWarningPanel->SetVisibility(ESlateVisibility::Visible);
	ShadowBackground->SetBrushResourceObject(BackgroundNotApply);

	if (ShadowFocusButton != ShadowHighButton)
	{
		FButtonStyle Style = ShadowHighButton->GetStyle();
		Style.Normal.SetResourceObject(HighActive);
		Style.Hovered.SetResourceObject(HighActive);
		Style.Hovered.SetImageSize(Style.Hovered.GetImageSize());
		Style.Pressed.SetResourceObject(HighActive);
		Style.Pressed.SetImageSize(Style.Pressed.GetImageSize());
		ShadowHighButton->SetStyle(Style);
	}

	if (ShadowFocusButton == ShadowLowButton)
	{
		FButtonStyle Style = ShadowLowButton->GetStyle();
		Style.Normal.SetResourceObject(LowNormal);
		Style.Hovered.SetResourceObject(LowActive);
		Style.Hovered.SetImageSize(Style.Hovered.GetImageSize());
		Style.Pressed.SetResourceObject(LowActive);
		Style.Pressed.SetImageSize(Style.Pressed.GetImageSize());
		ShadowLowButton->SetStyle(Style);
	}
	else if (ShadowFocusButton == ShadowMediumButton)
	{
		FButtonStyle Style = ShadowMediumButton->GetStyle();
		Style.Normal.SetResourceObject(MediumNormal);
		Style.Hovered.SetResourceObject(MediumActive);
		Style.Hovered.SetImageSize(Style.Hovered.GetImageSize());
		Style.Pressed.SetResourceObject(MediumActive);
		Style.Pressed.SetImageSize(Style.Pressed.GetImageSize());
		ShadowMediumButton->SetStyle(Style);
	}
	else if (ShadowFocusButton == ShadowEpicButton)
	{
		FButtonStyle Style = ShadowEpicButton->GetStyle();
		Style.Normal.SetResourceObject(BestNormal);
		Style.Hovered.SetResourceObject(BestActive);
		Style.Hovered.SetImageSize(Style.Hovered.GetImageSize());
		Style.Pressed.SetResourceObject(BestActive);
		Style.Pressed.SetImageSize(Style.Pressed.GetImageSize());
		ShadowEpicButton->SetStyle(Style);
	}

	ShadowFocusButton = ShadowHighButton;
}

void UGraphicsUMG::OnClickShadowEpicButton()
{
	if (bCanPlaySound)
	{
		UGameplayStatics::PlaySound2D(this, MouseClickSound);
	}
	if (GraphicSettings.ShadowQuality == 3)
	{
		return;
	}
	OptionsUMG->SetIsChangeGraphicOptions(true);

	GraphicSettings.ShadowQuality = 3;

	ShadowWarningPanel->SetVisibility(ESlateVisibility::Visible);
	ShadowBackground->SetBrushResourceObject(BackgroundNotApply);

	if (ShadowFocusButton != ShadowEpicButton)
	{
		FButtonStyle Style = ShadowEpicButton->GetStyle();
		Style.Normal.SetResourceObject(BestActive);
		Style.Hovered.SetResourceObject(BestActive);
		Style.Hovered.SetImageSize(Style.Hovered.GetImageSize());
		Style.Pressed.SetResourceObject(BestActive);
		Style.Pressed.SetImageSize(Style.Pressed.GetImageSize());
		ShadowEpicButton->SetStyle(Style);
	}

	if (ShadowFocusButton == ShadowLowButton)
	{
		FButtonStyle Style = ShadowLowButton->GetStyle();
		Style.Normal.SetResourceObject(LowNormal);
		Style.Hovered.SetResourceObject(LowActive);
		Style.Hovered.SetImageSize(Style.Hovered.GetImageSize());
		Style.Pressed.SetResourceObject(LowActive);
		Style.Pressed.SetImageSize(Style.Pressed.GetImageSize());
		ShadowLowButton->SetStyle(Style);
	}
	else if (ShadowFocusButton == ShadowMediumButton)
	{
		FButtonStyle Style = ShadowMediumButton->GetStyle();
		Style.Normal.SetResourceObject(MediumNormal);
		Style.Hovered.SetResourceObject(MediumActive);
		Style.Hovered.SetImageSize(Style.Hovered.GetImageSize());
		Style.Pressed.SetResourceObject(MediumActive);
		Style.Pressed.SetImageSize(Style.Pressed.GetImageSize());
		ShadowMediumButton->SetStyle(Style);
	}
	else if (ShadowFocusButton == ShadowHighButton)
	{
		FButtonStyle Style = ShadowHighButton->GetStyle();
		Style.Normal.SetResourceObject(HighNormal);
		Style.Hovered.SetResourceObject(HighActive);
		Style.Hovered.SetImageSize(Style.Hovered.GetImageSize());
		Style.Pressed.SetResourceObject(HighActive);
		Style.Pressed.SetImageSize(Style.Pressed.GetImageSize());
		ShadowHighButton->SetStyle(Style);
	}

	ShadowFocusButton = ShadowEpicButton;
}

void UGraphicsUMG::OnClickEffectLowButton()
{
	if (bCanPlaySound)
	{
		UGameplayStatics::PlaySound2D(this, MouseClickSound);
	}
	if (GraphicSettings.EffectQuality == 0)
	{
		return;
	}
	OptionsUMG->SetIsChangeGraphicOptions(true);

	GraphicSettings.EffectQuality = 0;

	EffectWarningPanel->SetVisibility(ESlateVisibility::Visible);
	EffectBackground->SetBrushResourceObject(BackgroundNotApply);

	if (EffectFocusButton != EffectLowButton)
	{
		FButtonStyle Style = EffectLowButton->GetStyle();
		Style.Normal.SetResourceObject(LowActive);
		Style.Hovered.SetResourceObject(LowActive);
		Style.Hovered.SetImageSize(Style.Hovered.GetImageSize());
		Style.Pressed.SetResourceObject(LowActive);
		Style.Pressed.SetImageSize(Style.Pressed.GetImageSize());
		EffectLowButton->SetStyle(Style);
	}

	if (EffectFocusButton == EffectMediumButton)
	{
		FButtonStyle Style = EffectMediumButton->GetStyle();
		Style.Normal.SetResourceObject(MediumNormal);
		Style.Hovered.SetResourceObject(MediumActive);
		Style.Hovered.SetImageSize(Style.Hovered.GetImageSize());
		Style.Pressed.SetResourceObject(MediumActive);
		Style.Pressed.SetImageSize(Style.Pressed.GetImageSize());
		EffectMediumButton->SetStyle(Style);
	}
	else if (EffectFocusButton == EffectHighButton)
	{
		FButtonStyle Style = EffectHighButton->GetStyle();
		Style.Normal.SetResourceObject(HighNormal);
		Style.Hovered.SetResourceObject(HighActive);
		Style.Hovered.SetImageSize(Style.Hovered.GetImageSize());
		Style.Pressed.SetResourceObject(HighActive);
		Style.Pressed.SetImageSize(Style.Pressed.GetImageSize());
		EffectHighButton->SetStyle(Style);
	}
	else if (EffectFocusButton == EffectEpicButton)
	{
		FButtonStyle Style = EffectEpicButton->GetStyle();
		Style.Normal.SetResourceObject(BestNormal);
		Style.Hovered.SetResourceObject(BestActive);
		Style.Hovered.SetImageSize(Style.Hovered.GetImageSize());
		Style.Pressed.SetResourceObject(BestActive);
		Style.Pressed.SetImageSize(Style.Pressed.GetImageSize());
		EffectEpicButton->SetStyle(Style);
	}

	EffectFocusButton = EffectLowButton;
}

void UGraphicsUMG::OnClickEffectMediumButton()
{
	if (bCanPlaySound)
	{
		UGameplayStatics::PlaySound2D(this, MouseClickSound);
	}
	if (GraphicSettings.EffectQuality == 1)
	{
		return;
	}
	OptionsUMG->SetIsChangeGraphicOptions(true);

	GraphicSettings.EffectQuality = 1;

	EffectWarningPanel->SetVisibility(ESlateVisibility::Visible);
	EffectBackground->SetBrushResourceObject(BackgroundNotApply);

	if (EffectFocusButton != EffectMediumButton)
	{
		FButtonStyle Style = EffectMediumButton->GetStyle();
		Style.Normal.SetResourceObject(MediumActive);
		Style.Hovered.SetResourceObject(MediumActive);
		Style.Hovered.SetImageSize(Style.Hovered.GetImageSize());
		Style.Pressed.SetResourceObject(MediumActive);
		Style.Pressed.SetImageSize(Style.Pressed.GetImageSize());
		EffectMediumButton->SetStyle(Style);
	}

	if (EffectFocusButton == EffectLowButton)
	{
		FButtonStyle Style = EffectLowButton->GetStyle();
		Style.Normal.SetResourceObject(LowNormal);
		Style.Hovered.SetResourceObject(LowActive);
		Style.Hovered.SetImageSize(Style.Hovered.GetImageSize());
		Style.Pressed.SetResourceObject(LowActive);
		Style.Pressed.SetImageSize(Style.Pressed.GetImageSize());
		EffectLowButton->SetStyle(Style);
	}
	else if (EffectFocusButton == EffectHighButton)
	{
		FButtonStyle Style = EffectHighButton->GetStyle();
		Style.Normal.SetResourceObject(HighNormal);
		Style.Hovered.SetResourceObject(HighActive);
		Style.Hovered.SetImageSize(Style.Hovered.GetImageSize());
		Style.Pressed.SetResourceObject(HighActive);
		Style.Pressed.SetImageSize(Style.Pressed.GetImageSize());
		EffectHighButton->SetStyle(Style);
	}
	else if (EffectFocusButton == EffectEpicButton)
	{
		FButtonStyle Style = EffectEpicButton->GetStyle();
		Style.Normal.SetResourceObject(BestNormal);
		Style.Hovered.SetResourceObject(BestActive);
		Style.Hovered.SetImageSize(Style.Hovered.GetImageSize());
		Style.Pressed.SetResourceObject(BestActive);
		Style.Pressed.SetImageSize(Style.Pressed.GetImageSize());
		EffectEpicButton->SetStyle(Style);
	}

	EffectFocusButton = EffectMediumButton;
}

void UGraphicsUMG::OnClickEffectHighButton()
{
	if (bCanPlaySound)
	{
		UGameplayStatics::PlaySound2D(this, MouseClickSound);
	}
	if (GraphicSettings.EffectQuality == 2)
	{
		return;
	}
	OptionsUMG->SetIsChangeGraphicOptions(true);

	GraphicSettings.EffectQuality = 2;

	EffectWarningPanel->SetVisibility(ESlateVisibility::Visible);
	EffectBackground->SetBrushResourceObject(BackgroundNotApply);

	if (EffectFocusButton != EffectHighButton)
	{
		FButtonStyle Style = EffectHighButton->GetStyle();
		Style.Normal.SetResourceObject(HighActive);
		Style.Hovered.SetResourceObject(HighActive);
		Style.Hovered.SetImageSize(Style.Hovered.GetImageSize());
		Style.Pressed.SetResourceObject(HighActive);
		Style.Pressed.SetImageSize(Style.Pressed.GetImageSize());
		EffectHighButton->SetStyle(Style);
	}

	if (EffectFocusButton == EffectLowButton)
	{
		FButtonStyle Style = EffectLowButton->GetStyle();
		Style.Normal.SetResourceObject(LowNormal);
		Style.Hovered.SetResourceObject(LowActive);
		Style.Hovered.SetImageSize(Style.Hovered.GetImageSize());
		Style.Pressed.SetResourceObject(LowActive);
		Style.Pressed.SetImageSize(Style.Pressed.GetImageSize());
		EffectLowButton->SetStyle(Style);
	}
	else if (EffectFocusButton == EffectMediumButton)
	{
		FButtonStyle Style = EffectMediumButton->GetStyle();
		Style.Normal.SetResourceObject(MediumNormal);
		Style.Hovered.SetResourceObject(MediumActive);
		Style.Hovered.SetImageSize(Style.Hovered.GetImageSize());
		Style.Pressed.SetResourceObject(MediumActive);
		Style.Pressed.SetImageSize(Style.Pressed.GetImageSize());
		EffectMediumButton->SetStyle(Style);
	}
	else if (EffectFocusButton == EffectEpicButton)
	{
		FButtonStyle Style = EffectEpicButton->GetStyle();
		Style.Normal.SetResourceObject(BestNormal);
		Style.Hovered.SetResourceObject(BestActive);
		Style.Hovered.SetImageSize(Style.Hovered.GetImageSize());
		Style.Pressed.SetResourceObject(BestActive);
		Style.Pressed.SetImageSize(Style.Pressed.GetImageSize());
		EffectEpicButton->SetStyle(Style);
	}

	EffectFocusButton = EffectHighButton;
}

void UGraphicsUMG::OnClickEffectEpicButton()
{
	if (bCanPlaySound)
	{

		UGameplayStatics::PlaySound2D(this, MouseClickSound);
	}
	if (GraphicSettings.EffectQuality == 3)
	{
		return;
	}
	OptionsUMG->SetIsChangeGraphicOptions(true);

	GraphicSettings.EffectQuality = 3;

	EffectWarningPanel->SetVisibility(ESlateVisibility::Visible);
	EffectBackground->SetBrushResourceObject(BackgroundNotApply);

	if (EffectFocusButton != EffectEpicButton)
	{
		FButtonStyle Style = EffectEpicButton->GetStyle();
		Style.Normal.SetResourceObject(BestActive);
		Style.Hovered.SetResourceObject(BestActive);
		Style.Hovered.SetImageSize(Style.Hovered.GetImageSize());
		Style.Pressed.SetResourceObject(BestActive);
		Style.Pressed.SetImageSize(Style.Pressed.GetImageSize());
		EffectEpicButton->SetStyle(Style);
	}

	if (EffectFocusButton == EffectLowButton)
	{
		FButtonStyle Style = EffectLowButton->GetStyle();
		Style.Normal.SetResourceObject(LowNormal);
		Style.Hovered.SetResourceObject(LowActive);
		Style.Hovered.SetImageSize(Style.Hovered.GetImageSize());
		Style.Pressed.SetResourceObject(LowActive);
		Style.Pressed.SetImageSize(Style.Pressed.GetImageSize());
		EffectLowButton->SetStyle(Style);
	}
	else if (EffectFocusButton == EffectMediumButton)
	{
		FButtonStyle Style = EffectMediumButton->GetStyle();
		Style.Normal.SetResourceObject(MediumNormal);
		Style.Hovered.SetResourceObject(MediumActive);
		Style.Hovered.SetImageSize(Style.Hovered.GetImageSize());
		Style.Pressed.SetResourceObject(MediumActive);
		Style.Pressed.SetImageSize(Style.Pressed.GetImageSize());
		EffectMediumButton->SetStyle(Style);
	}
	else if (EffectFocusButton == EffectHighButton)
	{
		FButtonStyle Style = EffectHighButton->GetStyle();
		Style.Normal.SetResourceObject(HighNormal);
		Style.Hovered.SetResourceObject(HighActive);
		Style.Hovered.SetImageSize(Style.Hovered.GetImageSize());
		Style.Pressed.SetResourceObject(HighActive);
		Style.Pressed.SetImageSize(Style.Pressed.GetImageSize());
		EffectHighButton->SetStyle(Style);
	}
	
	EffectFocusButton = EffectEpicButton;
}

void UGraphicsUMG::OnClickPostProcessingLowButton()
{
	if (bCanPlaySound)
	{
		UGameplayStatics::PlaySound2D(this, MouseClickSound);
	}
	if (GraphicSettings.PostProcessingQuality == 0)
	{
		return;
	}
	OptionsUMG->SetIsChangeGraphicOptions(true);

	GraphicSettings.PostProcessingQuality = 0;

	PostProcessingWarningPanel->SetVisibility(ESlateVisibility::Visible);
	PostProcessingBackground->SetBrushResourceObject(BackgroundNotApply);

	if (PostProcessingFocusButton != PostProcessingLowButton)
	{
		FButtonStyle Style = EffectLowButton->GetStyle();
		Style.Normal.SetResourceObject(LowActive);
		Style.Hovered.SetResourceObject(LowActive);
		Style.Hovered.SetImageSize(Style.Hovered.GetImageSize());
		Style.Pressed.SetResourceObject(LowActive);
		Style.Pressed.SetImageSize(Style.Pressed.GetImageSize());
		PostProcessingLowButton->SetStyle(Style);
	}

	if (PostProcessingFocusButton == PostProcessingMediumButton)
	{
		FButtonStyle Style = PostProcessingMediumButton->GetStyle();
		Style.Normal.SetResourceObject(MediumNormal);
		Style.Hovered.SetResourceObject(MediumActive);
		Style.Hovered.SetImageSize(Style.Hovered.GetImageSize());
		Style.Pressed.SetResourceObject(MediumActive);
		Style.Pressed.SetImageSize(Style.Pressed.GetImageSize());
		PostProcessingMediumButton->SetStyle(Style);
	}
	else if (PostProcessingFocusButton == PostProcessingHighButton)
	{
		FButtonStyle Style = PostProcessingHighButton->GetStyle();
		Style.Normal.SetResourceObject(HighNormal);
		Style.Hovered.SetResourceObject(HighActive);
		Style.Hovered.SetImageSize(Style.Hovered.GetImageSize());
		Style.Pressed.SetResourceObject(HighActive);
		Style.Pressed.SetImageSize(Style.Pressed.GetImageSize());
		PostProcessingHighButton->SetStyle(Style);
	}
	else if (PostProcessingFocusButton == PostProcessingEpicButton)
	{
		FButtonStyle Style = PostProcessingEpicButton->GetStyle();
		Style.Normal.SetResourceObject(BestNormal);
		Style.Hovered.SetResourceObject(BestActive);
		Style.Hovered.SetImageSize(Style.Hovered.GetImageSize());
		Style.Pressed.SetResourceObject(BestActive);
		Style.Pressed.SetImageSize(Style.Pressed.GetImageSize());
		PostProcessingEpicButton->SetStyle(Style);
	}

	PostProcessingFocusButton = PostProcessingLowButton;
}

void UGraphicsUMG::OnClickPostProcessingMediumButton()
{
	if (bCanPlaySound)
	{
		UGameplayStatics::PlaySound2D(this, MouseClickSound);
	}
	if (GraphicSettings.PostProcessingQuality == 1)
	{
		return;
	}
	OptionsUMG->SetIsChangeGraphicOptions(true);

	GraphicSettings.PostProcessingQuality = 1;

	PostProcessingWarningPanel->SetVisibility(ESlateVisibility::Visible);
	PostProcessingBackground->SetBrushResourceObject(BackgroundNotApply);

	if (PostProcessingFocusButton != PostProcessingMediumButton)
	{
		FButtonStyle Style = EffectLowButton->GetStyle();
		Style.Normal.SetResourceObject(MediumActive);
		Style.Hovered.SetResourceObject(MediumActive);
		Style.Hovered.SetImageSize(Style.Hovered.GetImageSize());
		Style.Pressed.SetResourceObject(MediumActive);
		Style.Pressed.SetImageSize(Style.Pressed.GetImageSize());
		PostProcessingMediumButton->SetStyle(Style);
	}

	if (PostProcessingFocusButton == PostProcessingLowButton)
	{
		FButtonStyle Style = PostProcessingLowButton->GetStyle();
		Style.Normal.SetResourceObject(LowNormal);
		Style.Hovered.SetResourceObject(LowActive);
		Style.Hovered.SetImageSize(Style.Hovered.GetImageSize());
		Style.Pressed.SetResourceObject(LowActive);
		Style.Pressed.SetImageSize(Style.Pressed.GetImageSize());
		PostProcessingLowButton->SetStyle(Style);
	}
	else if (PostProcessingFocusButton == PostProcessingHighButton)
	{
		FButtonStyle Style = PostProcessingHighButton->GetStyle();
		Style.Normal.SetResourceObject(HighNormal);
		Style.Hovered.SetResourceObject(HighActive);
		Style.Hovered.SetImageSize(Style.Hovered.GetImageSize());
		Style.Pressed.SetResourceObject(HighActive);
		Style.Pressed.SetImageSize(Style.Pressed.GetImageSize());
		PostProcessingHighButton->SetStyle(Style);
	}
	else if (PostProcessingFocusButton == PostProcessingEpicButton)
	{
		FButtonStyle Style = PostProcessingEpicButton->GetStyle();
		Style.Normal.SetResourceObject(BestNormal);
		Style.Hovered.SetResourceObject(BestActive);
		Style.Hovered.SetImageSize(Style.Hovered.GetImageSize());
		Style.Pressed.SetResourceObject(BestActive);
		Style.Pressed.SetImageSize(Style.Pressed.GetImageSize());
		PostProcessingEpicButton->SetStyle(Style);
	}

	PostProcessingFocusButton = PostProcessingMediumButton;
}

void UGraphicsUMG::OnClickPostProcessingHighButton()
{
	if (bCanPlaySound)
	{
		UGameplayStatics::PlaySound2D(this, MouseClickSound);
	}
	if (GraphicSettings.PostProcessingQuality == 2)
	{
		return;
	}
	OptionsUMG->SetIsChangeGraphicOptions(true);

	GraphicSettings.PostProcessingQuality = 2;

	PostProcessingWarningPanel->SetVisibility(ESlateVisibility::Visible);
	PostProcessingBackground->SetBrushResourceObject(BackgroundNotApply);

	if (PostProcessingFocusButton != PostProcessingHighButton)
	{
		FButtonStyle Style = EffectLowButton->GetStyle();
		Style.Normal.SetResourceObject(HighActive);
		Style.Hovered.SetResourceObject(HighActive);
		Style.Hovered.SetImageSize(Style.Hovered.GetImageSize());
		Style.Pressed.SetResourceObject(HighActive);
		Style.Pressed.SetImageSize(Style.Pressed.GetImageSize());
		PostProcessingHighButton->SetStyle(Style);
	}

	if (PostProcessingFocusButton == PostProcessingLowButton)
	{
		FButtonStyle Style = PostProcessingLowButton->GetStyle();
		Style.Normal.SetResourceObject(LowNormal);
		Style.Hovered.SetResourceObject(LowActive);
		Style.Hovered.SetImageSize(Style.Hovered.GetImageSize());
		Style.Pressed.SetResourceObject(LowActive);
		Style.Pressed.SetImageSize(Style.Pressed.GetImageSize());
		PostProcessingLowButton->SetStyle(Style);
	}
	else if (PostProcessingFocusButton == PostProcessingMediumButton)
	{
		FButtonStyle Style = PostProcessingMediumButton->GetStyle();
		Style.Normal.SetResourceObject(MediumNormal);
		Style.Hovered.SetResourceObject(MediumActive);
		Style.Hovered.SetImageSize(Style.Hovered.GetImageSize());
		Style.Pressed.SetResourceObject(MediumActive);
		Style.Pressed.SetImageSize(Style.Pressed.GetImageSize());
		PostProcessingMediumButton->SetStyle(Style);
	}
	else if (PostProcessingFocusButton == PostProcessingEpicButton)
	{
		FButtonStyle Style = PostProcessingEpicButton->GetStyle();
		Style.Normal.SetResourceObject(BestNormal);
		Style.Hovered.SetResourceObject(BestActive);
		Style.Hovered.SetImageSize(Style.Hovered.GetImageSize());
		Style.Pressed.SetResourceObject(BestActive);
		Style.Pressed.SetImageSize(Style.Pressed.GetImageSize());
		PostProcessingEpicButton->SetStyle(Style);
	}

	PostProcessingFocusButton = PostProcessingHighButton;
}

void UGraphicsUMG::OnClickPostProcessingEpicButton()
{
	if (bCanPlaySound)
	{
		UGameplayStatics::PlaySound2D(this, MouseClickSound);
	}
	if (GraphicSettings.PostProcessingQuality == 3)
	{
		return;
	}
	OptionsUMG->SetIsChangeGraphicOptions(true);

	GraphicSettings.PostProcessingQuality = 3;

	PostProcessingWarningPanel->SetVisibility(ESlateVisibility::Visible);
	PostProcessingBackground->SetBrushResourceObject(BackgroundNotApply);

	if (PostProcessingFocusButton != PostProcessingEpicButton)
	{
		FButtonStyle Style = EffectLowButton->GetStyle();
		Style.Normal.SetResourceObject(BestActive);
		Style.Hovered.SetResourceObject(BestActive);
		Style.Hovered.SetImageSize(Style.Hovered.GetImageSize());
		Style.Pressed.SetResourceObject(BestActive);
		Style.Pressed.SetImageSize(Style.Pressed.GetImageSize());
		PostProcessingEpicButton->SetStyle(Style);
	}
	if (PostProcessingFocusButton == PostProcessingLowButton)
	{
		FButtonStyle Style = PostProcessingEpicButton->GetStyle();
		Style.Normal.SetResourceObject(LowNormal);
		Style.Hovered.SetResourceObject(LowActive);
		Style.Hovered.SetImageSize(Style.Hovered.GetImageSize());
		Style.Pressed.SetResourceObject(LowActive);
		Style.Pressed.SetImageSize(Style.Pressed.GetImageSize());
		PostProcessingLowButton->SetStyle(Style);
	}
	else if (PostProcessingFocusButton == PostProcessingMediumButton)
	{
		FButtonStyle Style = PostProcessingMediumButton->GetStyle();
		Style.Normal.SetResourceObject(MediumNormal);
		Style.Hovered.SetResourceObject(MediumActive);
		Style.Hovered.SetImageSize(Style.Hovered.GetImageSize());
		Style.Pressed.SetResourceObject(MediumActive);
		Style.Pressed.SetImageSize(Style.Pressed.GetImageSize());
		PostProcessingMediumButton->SetStyle(Style);
	}
	else if (PostProcessingFocusButton == PostProcessingHighButton)
	{
		FButtonStyle Style = PostProcessingHighButton->GetStyle();
		Style.Normal.SetResourceObject(HighNormal);
		Style.Hovered.SetResourceObject(HighActive);
		Style.Hovered.SetImageSize(Style.Hovered.GetImageSize());
		Style.Pressed.SetResourceObject(HighActive);
		Style.Pressed.SetImageSize(Style.Pressed.GetImageSize());
		PostProcessingHighButton->SetStyle(Style);
	}

	PostProcessingFocusButton = PostProcessingEpicButton;
}

void UGraphicsUMG::OnClickVSyncEnabledOnButton()
{
	if (bCanPlaySound)
	{
		UGameplayStatics::PlaySound2D(this, MouseClickSound);
	}
	if (GraphicSettings.bVSyncEnabled)
	{
		return;
	}
	OptionsUMG->SetIsChangeGraphicOptions(true);

	GraphicSettings.bVSyncEnabled = true;

	VSyncEnabledWarningPanel->SetVisibility(ESlateVisibility::Visible);
	VSyncEnabledBackground->SetBrushResourceObject(BackgroundNotApply);

	if (VSyncEnabledFocusButton != VSyncEnabledOnButton)
	{
		FButtonStyle Style = VSyncEnabledOnButton->GetStyle();
		Style.Normal.SetResourceObject(OnActive);
		Style.Hovered.SetResourceObject(OnActive);
		Style.Hovered.SetImageSize(Style.Hovered.GetImageSize());
		Style.Pressed.SetResourceObject(OnActive);
		Style.Pressed.SetImageSize(Style.Pressed.GetImageSize());
		VSyncEnabledOnButton->SetStyle(Style);
	}

	if (VSyncEnabledFocusButton == VSyncEnabledOffButton)
	{
		FButtonStyle Style = VSyncEnabledOffButton->GetStyle();
		Style.Normal.SetResourceObject(OffNormal);
		Style.Hovered.SetResourceObject(OffActive);
		Style.Hovered.SetImageSize(Style.Hovered.GetImageSize());
		Style.Pressed.SetResourceObject(OffActive);
		Style.Pressed.SetImageSize(Style.Pressed.GetImageSize());
		VSyncEnabledOffButton->SetStyle(Style);
	}

	VSyncEnabledFocusButton = VSyncEnabledOnButton;
}

void UGraphicsUMG::OnClickVSyncEnabledOffButton()
{
	if (bCanPlaySound)
	{
		UGameplayStatics::PlaySound2D(this, MouseClickSound);
	}
	if (!GraphicSettings.bVSyncEnabled)
	{
		return;
	}
	OptionsUMG->SetIsChangeGraphicOptions(true);

	GraphicSettings.bVSyncEnabled = false;

	VSyncEnabledWarningPanel->SetVisibility(ESlateVisibility::Visible);
	VSyncEnabledBackground->SetBrushResourceObject(BackgroundNotApply);

	if (VSyncEnabledFocusButton != VSyncEnabledOffButton)
	{
		FButtonStyle Style = VSyncEnabledOffButton->GetStyle();
		Style.Normal.SetResourceObject(OffActive);
		Style.Hovered.SetResourceObject(OffActive);
		Style.Hovered.SetImageSize(Style.Hovered.GetImageSize());
		Style.Pressed.SetResourceObject(OffActive);
		Style.Pressed.SetImageSize(Style.Pressed.GetImageSize());
		VSyncEnabledOffButton->SetStyle(Style);
	}

	if (VSyncEnabledFocusButton == VSyncEnabledOnButton)
	{
		FButtonStyle Style = VSyncEnabledOnButton->GetStyle();
		Style.Normal.SetResourceObject(OnNormal);
		Style.Hovered.SetResourceObject(OnActive);
		Style.Hovered.SetImageSize(Style.Hovered.GetImageSize());
		Style.Pressed.SetResourceObject(OnActive);
		Style.Pressed.SetImageSize(Style.Pressed.GetImageSize());
		VSyncEnabledOnButton->SetStyle(Style);
	}

	VSyncEnabledFocusButton = VSyncEnabledOffButton;
}

void UGraphicsUMG::OnClickScreenShakeOnButton()
{
	if (bCanPlaySound)
	{
		UGameplayStatics::PlaySound2D(this, MouseClickSound);
	}
	if (GraphicSettings.bScreenShake)
	{
		return;
	}
	OptionsUMG->SetIsChangeGraphicOptions(true);

	GraphicSettings.bScreenShake = true;

	ScreenShakeWarningPanel->SetVisibility(ESlateVisibility::Visible);
	ScreenShakeBackground->SetBrushResourceObject(BackgroundNotApply);

	if (ScreenShakeFocusButton != ScreenShakeOnButton)
	{
		FButtonStyle Style = ScreenShakeOnButton->GetStyle();
		Style.Normal.SetResourceObject(OnActive);
		Style.Hovered.SetResourceObject(OnActive);
		Style.Hovered.SetImageSize(Style.Hovered.GetImageSize());
		Style.Pressed.SetResourceObject(OnActive);
		Style.Pressed.SetImageSize(Style.Pressed.GetImageSize());
		ScreenShakeOnButton->SetStyle(Style);
	}

	if (ScreenShakeFocusButton == ScreenShakeOffButton)
	{
		FButtonStyle Style = ScreenShakeOffButton->GetStyle();
		Style.Normal.SetResourceObject(OffNormal);
		Style.Hovered.SetResourceObject(OffActive);
		Style.Hovered.SetImageSize(Style.Hovered.GetImageSize());
		Style.Pressed.SetResourceObject(OffActive);
		Style.Pressed.SetImageSize(Style.Pressed.GetImageSize());
		ScreenShakeOffButton->SetStyle(Style);
	}

	ScreenShakeFocusButton = ScreenShakeOnButton;
}

void UGraphicsUMG::OnClickScreenShakeOffButton()
{
	if (bCanPlaySound)
	{
		UGameplayStatics::PlaySound2D(this, MouseClickSound);
	}
	if (!GraphicSettings.bScreenShake)
	{
		return;
	}
	OptionsUMG->SetIsChangeGraphicOptions(true);

	GraphicSettings.bScreenShake = false;

	ScreenShakeWarningPanel->SetVisibility(ESlateVisibility::Visible);
	ScreenShakeBackground->SetBrushResourceObject(BackgroundNotApply);

	if (ScreenShakeFocusButton != ScreenShakeOffButton)
	{
		FButtonStyle Style = ScreenShakeOffButton->GetStyle();
		Style.Normal.SetResourceObject(OffActive);
		Style.Hovered.SetResourceObject(OffActive);
		Style.Hovered.SetImageSize(Style.Hovered.GetImageSize());
		Style.Pressed.SetResourceObject(OffActive);
		Style.Pressed.SetImageSize(Style.Pressed.GetImageSize());
		ScreenShakeOffButton->SetStyle(Style);
	}

	if (ScreenShakeFocusButton == ScreenShakeOnButton)
	{
		FButtonStyle Style = ScreenShakeOnButton->GetStyle();
		Style.Normal.SetResourceObject(OnNormal);
		Style.Hovered.SetResourceObject(OnActive);
		Style.Hovered.SetImageSize(Style.Hovered.GetImageSize());
		Style.Pressed.SetResourceObject(OnActive);
		Style.Pressed.SetImageSize(Style.Pressed.GetImageSize());
		ScreenShakeOnButton->SetStyle(Style);
	}

	ScreenShakeFocusButton = ScreenShakeOffButton;
}

void UGraphicsUMG::OnClickAntialiasingOffButton()
{
	if (bCanPlaySound)
	{
		UGameplayStatics::PlaySound2D(this, MouseClickSound);
	}
	if (GraphicSettings.AntialiasingType == 0)
	{
		return;
	}
	OptionsUMG->SetIsChangeGraphicOptions(true);

	GraphicSettings.AntialiasingType = 0;

	AntialiasingWarningPanel->SetVisibility(ESlateVisibility::Visible);
	AntialiasingBackground->SetBrushResourceObject(BackgroundNotApply);

	if (AntialiasingFocusButton != AntialiasingOffButton)
	{
		FButtonStyle Style = AntialiasingOffButton->GetStyle();
		Style.Normal.SetResourceObject(OffActive);
		Style.Hovered.SetResourceObject(OffActive);
		Style.Hovered.SetImageSize(Style.Hovered.GetImageSize());
		Style.Pressed.SetResourceObject(OffActive);
		Style.Pressed.SetImageSize(Style.Pressed.GetImageSize());
		AntialiasingOffButton->SetStyle(Style);
	}

	if (AntialiasingFocusButton == AntialiasingFXAAButton)
	{
		FButtonStyle Style = AntialiasingFXAAButton->GetStyle();
		Style.Normal.SetResourceObject(FXAANormal);
		Style.Hovered.SetResourceObject(FXAAActive);
		Style.Hovered.SetImageSize(Style.Hovered.GetImageSize());
		Style.Pressed.SetResourceObject(FXAAActive);
		Style.Pressed.SetImageSize(Style.Pressed.GetImageSize());
		AntialiasingFXAAButton->SetStyle(Style);
	}
	else if (AntialiasingFocusButton == AntialiasingTAAButton)
	{
		FButtonStyle Style = AntialiasingTAAButton->GetStyle();
		Style.Normal.SetResourceObject(TAANormal);
		Style.Hovered.SetResourceObject(TAAActive);
		Style.Hovered.SetImageSize(Style.Hovered.GetImageSize());
		Style.Pressed.SetResourceObject(TAAActive);
		Style.Pressed.SetImageSize(Style.Pressed.GetImageSize());
		AntialiasingTAAButton->SetStyle(Style);
	}
	else if (AntialiasingFocusButton == AntialiasingMSAAButton)
	{
		FButtonStyle Style = AntialiasingMSAAButton->GetStyle();
		Style.Normal.SetResourceObject(MSAANormal);
		Style.Hovered.SetResourceObject(MSAAActive);
		Style.Hovered.SetImageSize(Style.Hovered.GetImageSize());
		Style.Pressed.SetResourceObject(MSAAActive);
		Style.Pressed.SetImageSize(Style.Pressed.GetImageSize());
		AntialiasingMSAAButton->SetStyle(Style);
	}
	else if (AntialiasingFocusButton == AntialiasingTSRButton)
	{
		FButtonStyle Style = AntialiasingTSRButton->GetStyle();
		Style.Normal.SetResourceObject(TSRNormal);
		Style.Hovered.SetResourceObject(TSRActive);
		Style.Hovered.SetImageSize(Style.Hovered.GetImageSize());
		Style.Pressed.SetResourceObject(TSRActive);
		Style.Pressed.SetImageSize(Style.Pressed.GetImageSize());
		AntialiasingTSRButton->SetStyle(Style);
	}

	AntialiasingFocusButton = AntialiasingOffButton;
}

void UGraphicsUMG::OnClickAntialiasingFXAAButton()
{
	if (bCanPlaySound)
	{
		UGameplayStatics::PlaySound2D(this, MouseClickSound);
	}
	if (GraphicSettings.AntialiasingType == 1)
	{
		return;
	}
	OptionsUMG->SetIsChangeGraphicOptions(true);

	GraphicSettings.AntialiasingType = 1;

	AntialiasingWarningPanel->SetVisibility(ESlateVisibility::Visible);
	AntialiasingBackground->SetBrushResourceObject(BackgroundNotApply);

	if (AntialiasingFocusButton != AntialiasingFXAAButton)
	{
		FButtonStyle Style = AntialiasingFXAAButton->GetStyle();
		Style.Normal.SetResourceObject(FXAAActive);
		Style.Hovered.SetResourceObject(FXAAActive);
		Style.Hovered.SetImageSize(Style.Hovered.GetImageSize());
		Style.Pressed.SetResourceObject(FXAAActive);
		Style.Pressed.SetImageSize(Style.Pressed.GetImageSize());
		AntialiasingFXAAButton->SetStyle(Style);
	}

	if (AntialiasingFocusButton == AntialiasingOffButton)
	{
		FButtonStyle Style = AntialiasingOffButton->GetStyle();
		Style.Normal.SetResourceObject(OffNormal);
		Style.Hovered.SetResourceObject(OffActive);
		Style.Hovered.SetImageSize(Style.Hovered.GetImageSize());
		Style.Pressed.SetResourceObject(OffActive);
		Style.Pressed.SetImageSize(Style.Pressed.GetImageSize());
		AntialiasingOffButton->SetStyle(Style);
	}
	else if (AntialiasingFocusButton == AntialiasingTAAButton)
	{
		FButtonStyle Style = AntialiasingTAAButton->GetStyle();
		Style.Normal.SetResourceObject(TAANormal);
		Style.Hovered.SetResourceObject(TAAActive);
		Style.Hovered.SetImageSize(Style.Hovered.GetImageSize());
		Style.Pressed.SetResourceObject(TAAActive);
		Style.Pressed.SetImageSize(Style.Pressed.GetImageSize());
		AntialiasingTAAButton->SetStyle(Style);
	}
	else if (AntialiasingFocusButton == AntialiasingMSAAButton)
	{
		FButtonStyle Style = AntialiasingMSAAButton->GetStyle();
		Style.Normal.SetResourceObject(MSAANormal);
		Style.Hovered.SetResourceObject(MSAAActive);
		Style.Hovered.SetImageSize(Style.Hovered.GetImageSize());
		Style.Pressed.SetResourceObject(MSAAActive);
		Style.Pressed.SetImageSize(Style.Pressed.GetImageSize());
		AntialiasingMSAAButton->SetStyle(Style);
	}
	else if (AntialiasingFocusButton == AntialiasingTSRButton)
	{
		FButtonStyle Style = AntialiasingTSRButton->GetStyle();
		Style.Normal.SetResourceObject(TSRNormal);
		Style.Hovered.SetResourceObject(TSRActive);
		Style.Hovered.SetImageSize(Style.Hovered.GetImageSize());
		Style.Pressed.SetResourceObject(TSRActive);
		Style.Pressed.SetImageSize(Style.Pressed.GetImageSize());
		AntialiasingTSRButton->SetStyle(Style);
	}

	AntialiasingFocusButton = AntialiasingFXAAButton;
}

void UGraphicsUMG::OnClickAntialiasingTAAButton()
{
	if (bCanPlaySound)
	{
		UGameplayStatics::PlaySound2D(this, MouseClickSound);
	}
	if (GraphicSettings.AntialiasingType == 2)
	{
		return;
	}
	OptionsUMG->SetIsChangeGraphicOptions(true);

	GraphicSettings.AntialiasingType = 2;

	AntialiasingWarningPanel->SetVisibility(ESlateVisibility::Visible);
	AntialiasingBackground->SetBrushResourceObject(BackgroundNotApply);

	if (AntialiasingFocusButton != AntialiasingTAAButton)
	{
		FButtonStyle Style = AntialiasingTAAButton->GetStyle();
		Style.Normal.SetResourceObject(TAAActive);
		Style.Hovered.SetResourceObject(TAAActive);
		Style.Hovered.SetImageSize(Style.Hovered.GetImageSize());
		Style.Pressed.SetResourceObject(TAAActive);
		Style.Pressed.SetImageSize(Style.Pressed.GetImageSize());
		AntialiasingTAAButton->SetStyle(Style);
	}

	if (AntialiasingFocusButton == AntialiasingOffButton)
	{
		FButtonStyle Style = AntialiasingOffButton->GetStyle();
		Style.Normal.SetResourceObject(OffNormal);
		Style.Hovered.SetResourceObject(OffActive);
		Style.Hovered.SetImageSize(Style.Hovered.GetImageSize());
		Style.Pressed.SetResourceObject(OffActive);
		Style.Pressed.SetImageSize(Style.Pressed.GetImageSize());
		AntialiasingOffButton->SetStyle(Style);
	}
	else if (AntialiasingFocusButton == AntialiasingFXAAButton)
	{
		FButtonStyle Style = AntialiasingFXAAButton->GetStyle();
		Style.Normal.SetResourceObject(FXAANormal);
		Style.Hovered.SetResourceObject(FXAAActive);
		Style.Hovered.SetImageSize(Style.Hovered.GetImageSize());
		Style.Pressed.SetResourceObject(FXAAActive);
		Style.Pressed.SetImageSize(Style.Pressed.GetImageSize());
		AntialiasingFXAAButton->SetStyle(Style);
	}
	else if (AntialiasingFocusButton == AntialiasingMSAAButton)
	{
		FButtonStyle Style = AntialiasingMSAAButton->GetStyle();
		Style.Normal.SetResourceObject(MSAANormal);
		Style.Hovered.SetResourceObject(MSAAActive);
		Style.Hovered.SetImageSize(Style.Hovered.GetImageSize());
		Style.Pressed.SetResourceObject(MSAAActive);
		Style.Pressed.SetImageSize(Style.Pressed.GetImageSize());
		AntialiasingMSAAButton->SetStyle(Style);
	}
	else if (AntialiasingFocusButton == AntialiasingTSRButton)
	{
		FButtonStyle Style = AntialiasingTSRButton->GetStyle();
		Style.Normal.SetResourceObject(TSRNormal);
		Style.Hovered.SetResourceObject(TSRActive);
		Style.Hovered.SetImageSize(Style.Hovered.GetImageSize());
		Style.Pressed.SetResourceObject(TSRActive);
		Style.Pressed.SetImageSize(Style.Pressed.GetImageSize());
		AntialiasingTSRButton->SetStyle(Style);
	}

	AntialiasingFocusButton = AntialiasingTAAButton;
}

void UGraphicsUMG::OnClickAntialiasingMSAAButton()
{
	if (bCanPlaySound)
	{
		UGameplayStatics::PlaySound2D(this, MouseClickSound);
	}
	if (GraphicSettings.AntialiasingType == 3)
	{
		return;
	}
	OptionsUMG->SetIsChangeGraphicOptions(true);

	GraphicSettings.AntialiasingType = 3;

	AntialiasingWarningPanel->SetVisibility(ESlateVisibility::Visible);
	AntialiasingBackground->SetBrushResourceObject(BackgroundNotApply);

	if (AntialiasingFocusButton != AntialiasingMSAAButton)
	{
		FButtonStyle Style = AntialiasingMSAAButton->GetStyle();
		Style.Normal.SetResourceObject(MSAAActive);
		Style.Hovered.SetResourceObject(MSAAActive);
		Style.Hovered.SetImageSize(Style.Hovered.GetImageSize());
		Style.Pressed.SetResourceObject(MSAAActive);
		Style.Pressed.SetImageSize(Style.Pressed.GetImageSize());
		AntialiasingMSAAButton->SetStyle(Style);
	}
	
	if (AntialiasingFocusButton == AntialiasingOffButton)
	{
		FButtonStyle Style = AntialiasingOffButton->GetStyle();
		Style.Normal.SetResourceObject(OffNormal);
		Style.Hovered.SetResourceObject(OffActive);
		Style.Hovered.SetImageSize(Style.Hovered.GetImageSize());
		Style.Pressed.SetResourceObject(OffActive);
		Style.Pressed.SetImageSize(Style.Pressed.GetImageSize());
		AntialiasingOffButton->SetStyle(Style);
	}
	else if (AntialiasingFocusButton == AntialiasingFXAAButton)
	{
		FButtonStyle Style = AntialiasingFXAAButton->GetStyle();
		Style.Normal.SetResourceObject(FXAANormal);
		Style.Hovered.SetResourceObject(FXAAActive);
		Style.Hovered.SetImageSize(Style.Hovered.GetImageSize());
		Style.Pressed.SetResourceObject(FXAAActive);
		Style.Pressed.SetImageSize(Style.Pressed.GetImageSize());
		AntialiasingFXAAButton->SetStyle(Style);
	}
	else if (AntialiasingFocusButton == AntialiasingTAAButton)
	{
		FButtonStyle Style = AntialiasingTAAButton->GetStyle();
		Style.Normal.SetResourceObject(TAANormal);
		Style.Hovered.SetResourceObject(TAAActive);
		Style.Hovered.SetImageSize(Style.Hovered.GetImageSize());
		Style.Pressed.SetResourceObject(TAAActive);
		Style.Pressed.SetImageSize(Style.Pressed.GetImageSize());
		AntialiasingTAAButton->SetStyle(Style);
	}
	else if (AntialiasingFocusButton == AntialiasingTSRButton)
	{
		FButtonStyle Style = AntialiasingTSRButton->GetStyle();
		Style.Normal.SetResourceObject(TSRNormal);
		Style.Hovered.SetResourceObject(TSRActive);
		Style.Hovered.SetImageSize(Style.Hovered.GetImageSize());
		Style.Pressed.SetResourceObject(TSRActive);
		Style.Pressed.SetImageSize(Style.Pressed.GetImageSize());
		AntialiasingTSRButton->SetStyle(Style);
	}

	AntialiasingFocusButton = AntialiasingMSAAButton;
}

void UGraphicsUMG::OnClickAntialiasingTSRButton()
{
	if (bCanPlaySound)
	{
		UGameplayStatics::PlaySound2D(this, MouseClickSound);
	}
	if (GraphicSettings.AntialiasingType == 4)
	{
		return;
	}
	OptionsUMG->SetIsChangeGraphicOptions(true);

	GraphicSettings.AntialiasingType = 4;

	AntialiasingWarningPanel->SetVisibility(ESlateVisibility::Visible);
	AntialiasingBackground->SetBrushResourceObject(BackgroundNotApply);

	if (AntialiasingFocusButton != AntialiasingTSRButton)
	{
		FButtonStyle Style = AntialiasingTSRButton->GetStyle();
		Style.Normal.SetResourceObject(TSRActive);
		Style.Hovered.SetResourceObject(TSRActive);
		Style.Hovered.SetImageSize(Style.Hovered.GetImageSize());
		Style.Pressed.SetResourceObject(TSRActive);
		Style.Pressed.SetImageSize(Style.Pressed.GetImageSize());
		AntialiasingTSRButton->SetStyle(Style);
	}

	if (AntialiasingFocusButton == AntialiasingOffButton)
	{
		FButtonStyle Style = AntialiasingOffButton->GetStyle();
		Style.Normal.SetResourceObject(OffNormal);
		Style.Hovered.SetResourceObject(OffActive);
		Style.Hovered.SetImageSize(Style.Hovered.GetImageSize());
		Style.Pressed.SetResourceObject(OffActive);
		Style.Pressed.SetImageSize(Style.Pressed.GetImageSize());
		AntialiasingOffButton->SetStyle(Style);
	}
	else if (AntialiasingFocusButton == AntialiasingFXAAButton)
	{
		FButtonStyle Style = AntialiasingFXAAButton->GetStyle();
		Style.Normal.SetResourceObject(FXAANormal);
		Style.Hovered.SetResourceObject(FXAAActive);
		Style.Hovered.SetImageSize(Style.Hovered.GetImageSize());
		Style.Pressed.SetResourceObject(FXAAActive);
		Style.Pressed.SetImageSize(Style.Pressed.GetImageSize());
		AntialiasingFXAAButton->SetStyle(Style);
	}
	else if (AntialiasingFocusButton == AntialiasingTAAButton)
	{
		FButtonStyle Style = AntialiasingTAAButton->GetStyle();
		Style.Normal.SetResourceObject(TAANormal);
		Style.Hovered.SetResourceObject(TAAActive);
		Style.Hovered.SetImageSize(Style.Hovered.GetImageSize());
		Style.Pressed.SetResourceObject(TAAActive);
		Style.Pressed.SetImageSize(Style.Pressed.GetImageSize());
		AntialiasingTAAButton->SetStyle(Style);
	}
	else if (AntialiasingFocusButton == AntialiasingMSAAButton)
	{
		FButtonStyle Style = AntialiasingMSAAButton->GetStyle();
		Style.Normal.SetResourceObject(MSAANormal);
		Style.Hovered.SetResourceObject(MSAAActive);
		Style.Hovered.SetImageSize(Style.Hovered.GetImageSize());
		Style.Pressed.SetResourceObject(MSAAActive);
		Style.Pressed.SetImageSize(Style.Pressed.GetImageSize());
		AntialiasingMSAAButton->SetStyle(Style);
	}

	AntialiasingFocusButton = AntialiasingTSRButton;
}

void UGraphicsUMG::OnFrameRateLimitSlider(float Value)
{
	GraphicSettings.FrameRateLimit = Value;

	if (bIsUpdatingFromSlider || bIsUpdatingFromText)
	{
		return;
	}

	if (!bIsChangeOptions)
	{
		bIsChangeOptions = true;
		OptionsUMG->SetIsChangeGraphicOptions(true);
	}

	FrameRateLimitWarningPanel->SetVisibility(ESlateVisibility::Visible);
	FrameRateLimitBackground->SetBrushResourceObject(BackgroundNotApply);

	bIsUpdatingFromSlider = true;
	FrameRateLimitNumericalText->SetText(FText::FromString(FString::FromInt(Value)));
	bIsUpdatingFromSlider = false;
}

void UGraphicsUMG::OnFrameRateLimitNumericalCommited(const FText& Text, ETextCommit::Type CommitMethod)
{
	if (bIsUpdatingFromSlider || bIsUpdatingFromText)
	{
		return;
	}

	if (!bIsChangeOptions)
	{
		bIsChangeOptions = true;
		OptionsUMG->SetIsChangeGraphicOptions(true);
	}

	FrameRateLimitWarningPanel->SetVisibility(ESlateVisibility::Visible);
	FrameRateLimitBackground->SetBrushResourceObject(BackgroundNotApply);

	float ParsedValue = 0;
	FDefaultValueHelper::ParseFloat(Text.ToString(), ParsedValue);

	bool bShouldBeFixedValue = false;
	if (ParsedValue < 30)
	{
		bShouldBeFixedValue = true;
		ParsedValue = 30;
	}
	else if (ParsedValue > 200)
	{
		bShouldBeFixedValue = true;
		ParsedValue = 200;
	}

	bIsUpdatingFromText = true;
	if (bShouldBeFixedValue)
	{
		FrameRateLimitNumericalText->SetText(FText::FromString(FString::FromInt(ParsedValue)));
	}
	FrameRateLimitSlider->SetValue(ParsedValue);
	bIsUpdatingFromText = false;
}

void UGraphicsUMG::OnScreenBrightnessSlider(float Value)
{
	GraphicSettings.ScreenBrightness = Value;

	if (bIsUpdatingFromSlider || bIsUpdatingFromText)
	{
		return;
	}

	if (!bIsChangeOptions)
	{
		bIsChangeOptions = true;
		OptionsUMG->SetIsChangeGraphicOptions(true);
	}

	ScreenBrightnessWarningPanel->SetVisibility(ESlateVisibility::Visible);
	ScreenBrightnessBackground->SetBrushResourceObject(BackgroundNotApply);

	bIsUpdatingFromSlider = true;
	ScreenBrightnessNumericalText->SetText(FText::FromString(FString::FromInt(Value)));
	bIsUpdatingFromSlider = false;
}

void UGraphicsUMG::OnScreenBrightnessNumericalCommited(const FText& Text, ETextCommit::Type CommitMethod)
{
	if (bIsUpdatingFromSlider || bIsUpdatingFromText)
	{
		return;
	}

	if (!bIsChangeOptions)
	{
		bIsChangeOptions = true;
		OptionsUMG->SetIsChangeGraphicOptions(true);
	}

	ScreenBrightnessWarningPanel->SetVisibility(ESlateVisibility::Visible);
	ScreenBrightnessBackground->SetBrushResourceObject(BackgroundNotApply);

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
		ScreenBrightnessNumericalText->SetText(FText::FromString(FString::FromInt(ParsedValue)));
	}
	ScreenBrightnessSlider->SetValue(ParsedValue);
	bIsUpdatingFromText = false;
}

void UGraphicsUMG::SetDefalutValue()
{
	if (TrapperGameInstance.Get())
	{
		DefaultDisplayModeString = TrapperGameInstance->GetDefaultDisplayModeString();
		DefaultResolutionString = TrapperGameInstance->GetDefaultDefaultResolutionString();
		DefaultTextureQuality = TrapperGameInstance->GetDefaultTexture();
		DefaultShadowQuality = TrapperGameInstance->GetDefaultShadow();
		DefaultEffectQuality = TrapperGameInstance->GetDefaultEffect();
		DefaultPostProcessingQuality = TrapperGameInstance->GetDefaultPostProcessing();
		DefaultbVSyncEnabled = TrapperGameInstance->GetDefaultbVSyncEnabled();
		DefaultbScreenShake = TrapperGameInstance->GetDefaultScreenShake();
		DefaultAntialiasingType = TrapperGameInstance->GetDefaultAntialiasing();
	}
}

void UGraphicsUMG::ApplyData()
{
	AllCollapsedWarningText();

	UGameUserSettings* GameUserSettings = UGameUserSettings::GetGameUserSettings();
	if (GameUserSettings)
	{
		/// 디스플레이 모드
		EDisplayMode GraphicDisplayMode = GraphicSettings.DisplayMode;
		EWindowMode::Type WindowMode = EWindowMode::Fullscreen;
		switch (GraphicDisplayMode)
		{
		case EDisplayMode::FullScreen:
			WindowMode = EWindowMode::Fullscreen;
			break;
		case EDisplayMode::WindowedFullScreen:
			WindowMode = EWindowMode::WindowedFullscreen;
			break;
		case EDisplayMode::Window:
			WindowMode = EWindowMode::Windowed;
			break;
		default:
			return;
		}

		/// 해상도
		FIntPoint NewResolution = FIntPoint();
		if (WindowMode == EWindowMode::Fullscreen || WindowMode == EWindowMode::WindowedFullscreen)
		{
			NewResolution = GEngine->GameUserSettings->GetDesktopResolution();
		}
		else if (WindowMode == EWindowMode::Windowed)
		{
			int32 ScreenWidth = GraphicSettings.ScreenWidth;
			int32 ScreenHeight = GraphicSettings.ScreenHeight;
			NewResolution = FIntPoint(ScreenWidth, ScreenHeight);
		}

		GameUserSettings->SetFullscreenMode(WindowMode);
		GameUserSettings->SetScreenResolution(NewResolution);

		/// Texture
		uint8 TextureQuality = GraphicSettings.TextureQuality;
		GameUserSettings->SetTextureQuality(TextureQuality);

		/// Shadow
		uint8 ShadowQuality = GraphicSettings.ShadowQuality;
		GameUserSettings->SetShadowQuality(ShadowQuality);

		/// Effect
		uint8 EffectQuality = GraphicSettings.EffectQuality;
		GameUserSettings->SetVisualEffectQuality(EffectQuality);

		/// PostProcessing
		uint8 PostProcessingQuality = GraphicSettings.PostProcessingQuality;
		GameUserSettings->SetPostProcessingQuality(PostProcessingQuality);

		/// VSyncEnabled
		bool bVSyncEnabled = GraphicSettings.bVSyncEnabled;
		GameUserSettings->SetVSyncEnabled(bVSyncEnabled);

		/// ScreenShake
		bool bScreenShake = GraphicSettings.bScreenShake;
		GameUserSettings->SetVSyncEnabled(bScreenShake);
		ATrapperPlayer* Player = Cast<ATrapperPlayer>(GetOwningPlayerPawn());
		if (Player)
		{
			Player->SetScreenShake(bScreenShake);
		}

		/// AntiAliasing
		uint8 AntialiasingType = GraphicSettings.AntialiasingType;
		IConsoleVariable* AntiAliasingMethod = IConsoleManager::Get().FindConsoleVariable(TEXT("r.AntiAliasingMethod"));
		if (AntiAliasingMethod)
		{
			AntiAliasingMethod->Set(AntialiasingType);
		}

		/// FrameRateLimit
		GameUserSettings->SetFrameRateLimit(GraphicSettings.FrameRateLimit);

		/// ScreenBrightness
		float GammaValue = 0.f;
		if (GraphicSettings.ScreenBrightness == 50)
		{
			GammaValue = 2.2f;
		}
		else
		{
			GammaValue = FMath::Lerp(1.1f, 3.3f, (GraphicSettings.ScreenBrightness - 1.0f) / 99.0f);
		}
		GEngine->DisplayGamma = GammaValue;

		GameUserSettings->ApplySettings(false);
	}
}

void UGraphicsUMG::AllCollapsedWarningText()
{
	FocusButton->SetVisibility(ESlateVisibility::Visible);

	if (DisplayModeWarningPanel->IsVisible())
	{
		DisplayModeWarningPanel->SetVisibility(ESlateVisibility::Collapsed);
		DisplayModeBackground->SetBrushResourceObject(BackgroundNormal);
	}
	if (ResolutionWarningPanel->IsVisible())
	{
		ResolutionWarningPanel->SetVisibility(ESlateVisibility::Collapsed);
		ResolutionBackground->SetBrushResourceObject(BackgroundNormal);
	}
	if (TextureWarningPanel->IsVisible())
	{
		TextureWarningPanel->SetVisibility(ESlateVisibility::Collapsed);
		TextureBackground->SetBrushResourceObject(BackgroundNormal);
	}
	if (ResolutionWarningPanel->IsVisible())
	{
		ResolutionWarningPanel->SetVisibility(ESlateVisibility::Collapsed);
		ResolutionBackground->SetBrushResourceObject(BackgroundNormal);
	}
	if (ShadowWarningPanel->IsVisible())
	{
		ShadowWarningPanel->SetVisibility(ESlateVisibility::Collapsed);
		ShadowBackground->SetBrushResourceObject(BackgroundNormal);
	}
	if (EffectWarningPanel->IsVisible())
	{
		EffectWarningPanel->SetVisibility(ESlateVisibility::Collapsed);
		EffectBackground->SetBrushResourceObject(BackgroundNormal);
	}
	if (PostProcessingWarningPanel->IsVisible())
	{
		PostProcessingWarningPanel->SetVisibility(ESlateVisibility::Collapsed);
		PostProcessingBackground->SetBrushResourceObject(BackgroundNormal);
	}
	if (VSyncEnabledWarningPanel->IsVisible())
	{
		VSyncEnabledWarningPanel->SetVisibility(ESlateVisibility::Collapsed);
		VSyncEnabledBackground->SetBrushResourceObject(BackgroundNormal);
	}
	if (ScreenShakeWarningPanel->IsVisible())
	{
		ScreenShakeWarningPanel->SetVisibility(ESlateVisibility::Collapsed);
		ScreenShakeBackground->SetBrushResourceObject(BackgroundNormal);
	}
	if (AntialiasingWarningPanel->IsVisible())
	{
		AntialiasingWarningPanel->SetVisibility(ESlateVisibility::Collapsed);
		AntialiasingBackground->SetBrushResourceObject(BackgroundNormal);
	}
	if (FrameRateLimitWarningPanel->IsVisible())
	{
		FrameRateLimitWarningPanel->SetVisibility(ESlateVisibility::Collapsed);
		FrameRateLimitBackground->SetBrushResourceObject(BackgroundNormal);
	}
	if (ScreenBrightnessWarningPanel->IsVisible())
	{
		ScreenBrightnessWarningPanel->SetVisibility(ESlateVisibility::Collapsed);
		ScreenBrightnessBackground->SetBrushResourceObject(BackgroundNormal);
	}
}

void UGraphicsUMG::FocusHorverChange(class UButton* HorverButton, class UImage* HorverBackground)
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
