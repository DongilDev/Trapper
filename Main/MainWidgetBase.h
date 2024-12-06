// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainWidgetBase.generated.h"

/**
 * 
 */
UCLASS()
class TRAPPERPROJECT_API UMainWidgetBase : public UUserWidget
{
	GENERATED_BODY()
public:
	//UMG Widget과 Control 변수 연결
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	class UButton* JoinButton;

	UPROPERTY(meta = (BindWidget))
	class UButton* LoginButton; 
	
	UFUNCTION()
	void OnJoinButton();

	UFUNCTION()
	void OnLoginButton();

	UFUNCTION()
	void OnButtonHovered();

	// Sound
	UPROPERTY(EditDefaultsOnly, Category = "Sound")
	TObjectPtr<class USoundWave> MouseClickSound;

	UPROPERTY(EditDefaultsOnly, Category = "Sound")
	TObjectPtr<class USoundWave> MouseHoverSound;
};
