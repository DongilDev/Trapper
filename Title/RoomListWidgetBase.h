// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RoomListWidgetBase.generated.h"

/**
 * 
 */
UCLASS()
class TRAPPERPROJECT_API URoomListWidgetBase : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* RoomNumText;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* HostNicknameText;
	
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* FullRoomErrorTextBlock;

	UPROPERTY(meta = (BindWidget))
	class UButton* JoinRoomButton;

	UFUNCTION()
	void OnJoinButton();

	void SetRoomNumText(int32 RoomNum);
	void SetHostNicknameText(FString HostNickname);
	void ShowFullRoomErrorText(bool Show);

	int32 GetRoomNumber() { return RoomNumber; }

	// Sound
	UPROPERTY(EditDefaultsOnly, Category = "Sound")
	TObjectPtr<class USoundWave> MouseClickSound;

	UPROPERTY(EditDefaultsOnly, Category = "Sound")
	TObjectPtr<class USoundWave> MouseHoverSound;
	
	UPROPERTY(EditDefaultsOnly, Category = "Sound")
	TObjectPtr<class USoundWave> MouseClickFailedSound;

	UFUNCTION()
	void OnButtonHovered();

private:
	int32 RoomNumber;
};
