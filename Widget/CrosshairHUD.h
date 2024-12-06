// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CrosshairHUD.generated.h"

/**
 *
 */
UCLASS()
class TRAPPERPROJECT_API UCrosshairHUD : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

public:
	void SetShiftFrameVisibility(bool bIsVisible);
	void SetMagneticFrameVisibility(bool bIsVisible);
	void SetDrawFrameVisibility(bool bIsVisible);
	void SetDrawFrameScale(float DrawTime, float MaxDrawTime);
	void SetTrapFrameVisibility(bool bIsVisible);
	void SetTrapDemolishFrameVisibility(bool bIsVisible);
	void SetTrapWeckageFrameVisibility(bool bIsVisible);
	void SetInteractFrameVisibility(bool bIsVisible);
	void SetTrapAimFrameVisibility(bool bIsVisible);

	UPROPERTY(meta = (BindWidget), VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<class UImage> DrawFrame;

	UPROPERTY(meta = (BindWidget), VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<class UImage> BasicFrame;

	UPROPERTY(meta = (BindWidget), VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<class UImage> CenterAim;

	UPROPERTY(meta = (BindWidget), VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<class UImage> MagneticArrowFrame;

	UPROPERTY(meta = (BindWidget), VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<class UImage> ShiftFrame;

	UPROPERTY(meta = (BindWidget), VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<class UImage> TrapInstallFrame;

	UPROPERTY(meta = (BindWidget), VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<class UImage> TrapDemolishFrame;

	UPROPERTY(meta = (BindWidget), VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<class UImage> TrapWeckageFrame;

	UPROPERTY(meta = (BindWidget), VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<class UImage> InteractFrame;

	UPROPERTY(meta = (BindWidget), VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<class UImage> TrapRotationImage;

	UPROPERTY(meta = (BindWidget), VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<class UImage> TrapAimFrame;
};
