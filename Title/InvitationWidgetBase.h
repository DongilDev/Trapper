// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InvitationWidgetBase.generated.h"

/**
 * 
 */
UCLASS()
class TRAPPERPROJECT_API UInvitationWidgetBase : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;

	UFUNCTION()
	void OnAcceptButton();

	UFUNCTION()
	void OnRejectButton();

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Control")
	class UTextBlock* InvitationText;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Control")
	class UButton* AcceptButton;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Control")
	class UButton* RejectButton;

	void SetRoomNumber(int32 RNumber);
	void SetNickname(FString RNickname);

	void SetInvitationText(FString RNickname);

	// Sound
	UPROPERTY(EditDefaultsOnly, Category = "Sound")
	TObjectPtr<class USoundWave> MouseClickSound;

	UPROPERTY(EditDefaultsOnly, Category = "Sound")
	TObjectPtr<class USoundWave> MouseHoverSound;

	UFUNCTION()
	void OnButtonHovered();

private:
	int32 RoomNumber = 0;
	FString Nickname;
};
