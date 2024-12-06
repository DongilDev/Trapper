// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Options.h"
#include "GraphicsUMG.generated.h"

/**
 *
 */
UCLASS()
class TRAPPERPROJECT_API UGraphicsUMG : public UUserWidget
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
	void GraphicOptionInitialize();

private:
	UFUNCTION()
	void OnHorverDisplayMode();

	UFUNCTION()
	void OnHorverResolution();

	UFUNCTION()
	void OnHorverTexture();

	UFUNCTION()
	void OnHorverShadow();

	UFUNCTION()
	void OnHorverEffect();

	UFUNCTION()
	void OnHorverPostProcessing();

	UFUNCTION()
	void OnHorverVSyncEnabled();

	UFUNCTION()
	void OnHorverScreenShake();

	UFUNCTION()
	void OnHorverAntialiasing();

	UFUNCTION()
	void OnHorverFrameRateLimit();

	UFUNCTION()
	void OnHorverScreenBrightness();

	UFUNCTION()
	void OnDisplayModeComboBoxSelectionChanged(FString SelectedItem, ESelectInfo::Type SelectionType);

	UFUNCTION()
	void OnResolutionModeComboBoxSelectionChanged(FString SelectedItem, ESelectInfo::Type SelectionType);

	/// Texture
	UFUNCTION()
	void OnClickTextureLowButton();

	UFUNCTION()
	void OnClickTextureMediumButton();

	UFUNCTION()
	void OnClickTextureHighButton();

	UFUNCTION()
	void OnClickTextureEpicButton();

	/// Shadow
	UFUNCTION()
	void OnClickShadowLowButton();

	UFUNCTION()
	void OnClickShadowMediumButton();

	UFUNCTION()
	void OnClickShadowHighButton();

	UFUNCTION()
	void OnClickShadowEpicButton();

	/// Effect
	UFUNCTION()
	void OnClickEffectLowButton();

	UFUNCTION()
	void OnClickEffectMediumButton();

	UFUNCTION()
	void OnClickEffectHighButton();

	UFUNCTION()
	void OnClickEffectEpicButton();

	/// PostProcessing
	UFUNCTION()
	void OnClickPostProcessingLowButton();

	UFUNCTION()
	void OnClickPostProcessingMediumButton();

	UFUNCTION()
	void OnClickPostProcessingHighButton();

	UFUNCTION()
	void OnClickPostProcessingEpicButton();

	/// VSyncEnabled
	UFUNCTION()
	void OnClickVSyncEnabledOnButton();

	UFUNCTION()
	void OnClickVSyncEnabledOffButton();

	/// ScreenShake
	UFUNCTION()
	void OnClickScreenShakeOnButton();

	UFUNCTION()
	void OnClickScreenShakeOffButton();

	/// Antialiasing
	UFUNCTION()
	void OnClickAntialiasingOffButton();

	UFUNCTION()
	void OnClickAntialiasingFXAAButton();

	UFUNCTION()
	void OnClickAntialiasingTAAButton();

	UFUNCTION()
	void OnClickAntialiasingMSAAButton();

	UFUNCTION()
	void OnClickAntialiasingTSRButton();

	/// FrameRateLimit
	UFUNCTION()
	void OnFrameRateLimitSlider(float Value);

	UFUNCTION()
	void OnFrameRateLimitNumericalCommited(const FText& Text, ETextCommit::Type CommitMethod);

	/// ScreenBrightness
	UFUNCTION()
	void OnScreenBrightnessSlider(float Value);

	UFUNCTION()
	void OnScreenBrightnessNumericalCommited(const FText& Text, ETextCommit::Type CommitMethod);

public:
	void SetDefalutValue();

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

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TObjectPtr<class UTexture> LowNormal;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TObjectPtr<class UTexture> LowActive;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TObjectPtr<class UTexture> MediumNormal;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TObjectPtr<class UTexture> MediumActive;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TObjectPtr<class UTexture> HighNormal;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TObjectPtr<class UTexture> HighActive;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TObjectPtr<class UTexture> BestNormal;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TObjectPtr<class UTexture> BestActive;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TObjectPtr<class UTexture> OnNormal;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TObjectPtr<class UTexture> OnActive;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TObjectPtr<class UTexture> OffNormal;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TObjectPtr<class UTexture> OffActive;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TObjectPtr<class UTexture> FXAANormal;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TObjectPtr<class UTexture> FXAAActive;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TObjectPtr<class UTexture> TAANormal;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TObjectPtr<class UTexture> TAAActive;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TObjectPtr<class UTexture> MSAANormal;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TObjectPtr<class UTexture> MSAAActive;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TObjectPtr<class UTexture> TSRNormal;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TObjectPtr<class UTexture> TSRActive;

private:
	/// DisplayMode
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UCanvasPanel> DisplayModePanel;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UCanvasPanel> DisplayModeWarningPanel;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UImage> DisplayModeBackground;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UComboBoxString> DisplayModeComboBox;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> DisplayMode;

	/// Resolution
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UCanvasPanel> ResolutionPanel;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UCanvasPanel> ResolutionWarningPanel;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UImage> ResolutionBackground;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UComboBoxString> ResolutionComboBox;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UComboBoxString> WindowedFullScreenResolutionComboBox;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> Resolution;

	/// Texture
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UCanvasPanel> TexturePanel;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UCanvasPanel> TextureWarningPanel;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UImage> TextureBackground;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> TextureLowButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> TextureMediumButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> TextureHighButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> TextureEpicButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> Texture;

	/// Shadow
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UCanvasPanel> ShadowPanel;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UCanvasPanel> ShadowWarningPanel;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UImage> ShadowBackground;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> ShadowLowButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> ShadowMediumButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> ShadowHighButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> ShadowEpicButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> Shadow;

	/// Effect
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UCanvasPanel> EffectPanel;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UCanvasPanel> EffectWarningPanel;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UImage> EffectBackground;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> EffectLowButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> EffectMediumButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> EffectHighButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> EffectEpicButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> Effect;

	/// PostProcessing
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UCanvasPanel> PostProcessingPanel;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UCanvasPanel> PostProcessingWarningPanel;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UImage> PostProcessingBackground;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> PostProcessingLowButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> PostProcessingMediumButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> PostProcessingHighButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> PostProcessingEpicButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> PostProcessing;

	/// VSyncEnabled
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UCanvasPanel> VSyncEnabledPanel;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UCanvasPanel> VSyncEnabledWarningPanel;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UImage> VSyncEnabledBackground;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> VSyncEnabledOnButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> VSyncEnabledOffButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> VSyncEnabled;

	/// ScreenShake
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UCanvasPanel> ScreenShakePanel;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UCanvasPanel>  ScreenShakeWarningPanel;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UImage>  ScreenShakeBackground;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton>  ScreenShakeOnButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton>  ScreenShakeOffButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton>  ScreenShake;

	/// Antialiasing
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UCanvasPanel> AntialiasingPanel;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UCanvasPanel> AntialiasingWarningPanel;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UImage> AntialiasingBackground;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> AntialiasingOffButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> AntialiasingFXAAButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> AntialiasingTAAButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> AntialiasingMSAAButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> AntialiasingTSRButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> Antialiasing;

	/// FrameRateLimit
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UCanvasPanel> FrameRateLimitPanel;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UCanvasPanel> FrameRateLimitWarningPanel;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UImage> FrameRateLimitBackground;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class USlider> FrameRateLimitSlider;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UEditableText> FrameRateLimitNumericalText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> FrameRateLimit;

	/// ScreenBrightness
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UCanvasPanel> ScreenBrightnessPanel;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UCanvasPanel> ScreenBrightnessWarningPanel;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UImage> ScreenBrightnessBackground;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class USlider> ScreenBrightnessSlider;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UEditableText> ScreenBrightnessNumericalText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> ScreenBrightness;

	/// �ѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤ�

	TObjectPtr<class UButton> TextureFocusButton;
	TObjectPtr<class UButton> ShadowFocusButton;
	TObjectPtr<class UButton> EffectFocusButton;
	TObjectPtr<class UButton> PostProcessingFocusButton;
	TObjectPtr<class UButton> VSyncEnabledFocusButton;
	TObjectPtr<class UButton> ScreenShakeFocusButton;
	TObjectPtr<class UButton> AntialiasingFocusButton;

	FString DefaultDisplayModeString = "";
	FString DefaultResolutionString = "";
	int32 DefaultTextureQuality = 0;
	int32 DefaultShadowQuality = 0;
	int32 DefaultEffectQuality = 0;
	int32 DefaultPostProcessingQuality = 0;
	uint8 DefaultbVSyncEnabled : 1 = false;
	uint8 DefaultbScreenShake : 1 = false;
	int32 DefaultAntialiasingType = 0;

	uint8 bIsUpdatingFromSlider : 1 = false;
	uint8 bIsUpdatingFromText : 1 = false;
	uint8 bIsChangeOptions : 1 = false;
	uint8 bCanPlaySound : 1 = true;

	FGraphicSettings GraphicSettings;
	TWeakObjectPtr<class UTrapperGameInstance> TrapperGameInstance;
	TWeakObjectPtr<class UOptionsUMG> OptionsUMG;

	TObjectPtr<class UButton> FocusButton;
	TObjectPtr<class UImage> FocusImage;

	TObjectPtr<class USoundWave> MouseClickSound;
	TObjectPtr<class USoundWave> MouseHoverSound;
};
