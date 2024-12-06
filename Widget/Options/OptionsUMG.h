// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "OptionsUMG.generated.h"

/**
 *
 */
UCLASS()
class TRAPPERPROJECT_API UOptionsUMG : public UUserWidget
{
	GENERATED_BODY()

private:
	virtual void NativeConstruct() override;

public:
	void ImportData();
	FORCEINLINE void SetIsChangeControlOptions(bool Value) { bIsChangeControlOptions = Value; }
	FORCEINLINE void SetIsChangeSoundOptions(bool Value) { bIsChangeSoundOptions = Value; }
	FORCEINLINE void SetIsChangeGraphicOptions(bool Value) { bIsChangeGraphicOptions = Value; }

private:
	UFUNCTION()
	void OnClickControlsButton();

	UFUNCTION()
	void OnClickSoundsButton();

	UFUNCTION()
	void OnClickGraphicsButton();

	UFUNCTION()
	void OnClickApplyButton();

	UFUNCTION()
	void OnClickDefaultButton();

	UFUNCTION()
	void OnClickBackButton();

	UFUNCTION()
	void OnHorverControlsButton();

	UFUNCTION()
	void OnHorverSoundsButton();

	UFUNCTION()
	void OnHorverGraphicsButton();

	UFUNCTION()
	void OnHorverApplyButton();

	UFUNCTION()
	void OnHorverDefaultButton();

	UFUNCTION()
	void OnHorverBackButton();

	UFUNCTION()
	void OnVisibilityChangedFunction(ESlateVisibility InVisibility);

public:
	FORCEINLINE void SetTrapperImage(class UImage* Image) { TrapperImage = Image; }
	void SetTitleGameExitButton(class UButton* Button);
	void SetTitleLogoutButton(class UButton* Button);
	void SetTitleCreditButton(class UButton* Button);

	FORCEINLINE void SetTitleCanvas(class UCanvasPanel* Canvas) { TitleCanvas = Canvas; }

private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UControlsUMG> ControlsUMG;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class USoundsUMG> SoundsUMG;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UGraphicsUMG> GraphicsUMG;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> ControlsButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> SoundsButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> GraphicsButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> ApplyButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> DefaultButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> BackButton;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<class UTexture> ControlButtonNormalTexture;
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<class UTexture> ControlButtonHoveredTexture;
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<class UTexture> ControlButtonClickedTexture;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<class UTexture> SoundButtonNormalTexture;
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<class UTexture> SoundButtonHoveredTexture;
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<class UTexture> SoundButtonClickedTexture;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<class UTexture> GraphicButtonNormalTexture;
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<class UTexture> GraphicButtonHoveredTexture;
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<class UTexture> GraphicButtonClickedTexture;

	TWeakObjectPtr<class UTrapperGameInstance> TrapperGameInstance;
	
	TObjectPtr<class UButton> FocusButton;
	TObjectPtr<class UImage> TrapperImage;
	TObjectPtr<class UCanvasPanel> TitleCanvas;

	uint8 bIsChangeControlOptions : 1 = false;
	uint8 bIsChangeSoundOptions : 1 = false;
	uint8 bIsChangeGraphicOptions : 1 = false;

	UPROPERTY(EditDefaultsOnly, Category = "Sound")
	TObjectPtr<class USoundWave> MouseClickSound;

	UPROPERTY(EditDefaultsOnly, Category = "Sound")
	TObjectPtr<class USoundWave> MouseHoverSound;

	TWeakObjectPtr<class UButton> TitleGameExitButton;
	TWeakObjectPtr<class UButton> TitleLogoutButton;
	TWeakObjectPtr<class UButton> TitleCreditButton;
};
