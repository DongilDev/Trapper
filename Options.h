// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Options.generated.h"

/**
 *
 */
USTRUCT()
struct FControlSettings
{
	GENERATED_BODY()

	UPROPERTY()
	float GeneralSensitivitySliderValue = 0.5f;

	UPROPERTY()
	float AimSensitivitySliderValue = 0.5f;
};

USTRUCT()
struct FSoundSettings
{
	GENERATED_BODY()

	UPROPERTY()
	float MasterSoundsSliderValue = 1.f;

	UPROPERTY()
	float EffectSoundsSliderValue = 1.f;

	UPROPERTY()
	float BackgroundSoundsSliderValue = 1.f;

	UPROPERTY()
	float VoiceDialogSoundsSliderValue = 1.f;
};

UENUM()
enum class EDisplayMode : uint8
{
	FullScreen,
	WindowedFullScreen,
	Window
};

USTRUCT()
struct FGraphicSettings
{
	GENERATED_BODY()

	UPROPERTY()
	EDisplayMode DisplayMode = EDisplayMode::Window;

	UPROPERTY()
	int32 ScreenWidth = 1920;

	UPROPERTY()
	int32 ScreenHeight = 1080;

	UPROPERTY()
	int32 TextureQuality = 0;

	UPROPERTY()
	int32 ShadowQuality = 0;

	UPROPERTY()
	int32 EffectQuality = 0;

	UPROPERTY()
	int32 PostProcessingQuality = 0;

	UPROPERTY()
	uint8 bVSyncEnabled : 1 = false;

	UPROPERTY()
	uint8 bScreenShake : 1 = true;

	UPROPERTY()
	int32 AntialiasingType = 0;

	UPROPERTY()
	int32 FrameRateLimit = 120;

	UPROPERTY()
	int32 ScreenBrightness = 50;
};