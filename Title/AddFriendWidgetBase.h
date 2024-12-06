// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AddFriendWidgetBase.generated.h"

/**
 * 
 */
UCLASS()
class TRAPPERPROJECT_API UAddFriendWidgetBase : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* AddFriendText;
	
	UPROPERTY(meta = (BindWidget))
	class UButton* AddFriendStatusButton;

	UFUNCTION()
	void OnAddFriendStatusButton();

	// Sound
	UPROPERTY(EditDefaultsOnly, Category = "Sound")
	TObjectPtr<class USoundWave> MouseClickSound;

	UPROPERTY(EditDefaultsOnly, Category = "Sound")
	TObjectPtr<class USoundWave> MouseHoverSound;

	UFUNCTION()
	void OnButtonHovered();


private:
	UTexture2D* HoveredImage;  // 마우스 오버 시 보여줄 이미지
	UTexture2D* NormalImage;   // 기본 이미지
};
