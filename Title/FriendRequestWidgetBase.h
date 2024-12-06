// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FriendRequestWidgetBase.generated.h"

/**
 * 
 */
UCLASS()
class TRAPPERPROJECT_API UFriendRequestWidgetBase : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	UFUNCTION()
	void OnAcceptButton();

	UFUNCTION()
	void OnRejectButton();

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Control")
	class UTextBlock* RequestFriendIdText;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Control")
	class UButton* AcceptButton;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Control")
	class UButton* RejectButton;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "UI")
	class UFriendWidgetBase* FriendWidgetObject;

	// Sound
	UPROPERTY(EditDefaultsOnly, Category = "Sound")
	TObjectPtr<class USoundWave> MouseClickSound;

	UPROPERTY(EditDefaultsOnly, Category = "Sound")
	TObjectPtr<class USoundWave> MouseHoverSound;

	UFUNCTION()
	void OnButtonHovered();

	void SetFriendWidget(UUserWidget* Widget);
};
