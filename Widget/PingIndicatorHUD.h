// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PingIndicatorHUD.generated.h"

/**
 * 
 */
UCLASS()
class TRAPPERPROJECT_API UPingIndicatorHUD : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	UFUNCTION(BlueprintImplementableEvent)
	void SetWidgetImage(EPlayerPingType type);

public:
	UPROPERTY(meta=(BindWidget), VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<class UCanvasPanel> MasterPanel;

	UPROPERTY(meta=(BindWidget), VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<class UCanvasPanel> Panel;

	UPROPERTY(meta=(BindWidget), VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<class UTextBlock> Distance;
	
	UPROPERTY(meta=(BindWidget), VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<class UImage> Image;

	UPROPERTY(meta=(BindWidget), VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<class UImage> Direction;

	UPROPERTY(meta=(BindWidget), VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<class UUserWidget> QuestBackground;

	UPROPERTY()
	TObjectPtr<class UCanvasPanelSlot> PanelSlot;
};
