// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TitleWidgetBase.generated.h"

/**
 *
 */
UCLASS()
class TRAPPERPROJECT_API UTitleWidgetBase : public UUserWidget
{
	GENERATED_BODY()
public:
	//UMG Widget과 Control 변수 연결
	virtual void NativeConstruct() override;

	void OnBack();

	UPROPERTY(meta = (BindWidget))
	class UButton* CreateRoomButton;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* CreateRoomTextBlock;

	UPROPERTY(meta = (BindWidget))
	class UButton* JoinRoomButton;
	
	UPROPERTY(meta = (BindWidget))
	class UButton* CreditButton;
	
	UPROPERTY(meta = (BindWidget))
	class UButton* LogoutButton;
	
	UPROPERTY(meta = (BindWidget))
	class UButton* ExitButton;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* JoinRoomTextBlock;

	UPROPERTY(meta = (BindWidget))
	class UButton* FriendButton;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* FriendTextBlock;

	/*UPROPERTY(meta = (BindWidget))
	class UButton* BookButton;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* BookTextBlock;

	UPROPERTY(meta = (BindWidget))
	class UButton* ChallengeButton;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* ChallengeTextBlock;*/

	UPROPERTY(meta = (BindWidget))
	class UVerticalBox* InvitationVerticalBox;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UFriendWidgetBase* FriendWidget;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "UI")
	TSubclassOf<class UInvitationWidgetBase> UInvitationWidgetClass;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "UI")
	class UInvitationWidgetBase* UInvitationWidgetObject;

	// InputRoomNumber
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "UI")
	TSubclassOf<class URoomListWidgetBase> URoomListWidgetClass;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "UI")
	class URoomListWidgetBase* URoomListWidgetObject;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Control")
	class UCanvasPanel* InputRoomNumberCanvas;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Control")
	class UButton* QuitInputRoomNumberButton;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Control")
	class UEditableTextBox* InputRoomNumber;

	UPROPERTY(meta = (BindWidget))
	class UVerticalBox* RoomListVerticalBox;

	UPROPERTY(meta = (BindWidget))
	class UImage* RommNotExistImage;

	UPROPERTY(meta = (BindWidget))
	class UCanvasPanel* OptionCanvasPanel;

	// RoomListWidget들을 관리하는 배열 또는 맵
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TMap<int32, URoomListWidgetBase*> RoomListWidgets;

	// 크레딧
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UCreditWidgetBase* CreditWidget;

	

	UFUNCTION()
	void OnButtonHovered();

	UFUNCTION()
	void OnButtonUnhovered();

	UFUNCTION()
	void OnCreateRoomButton();

	UFUNCTION()
	void OnJoinRoomButton();

	UFUNCTION()
	void OnFriendButton();

	UFUNCTION()
	void OnLogoutButton();

	UFUNCTION()
	void OnBookButton();

	UFUNCTION()
	void OnChallengeButton();

	UFUNCTION()
	void OnCreditButton();

	UFUNCTION()
	void CreditFinished();

	UFUNCTION()
	void OnRoomNumberTextChanged(const FText& Text);

	void AddInvitation(int32 RoomNumber, FString Nickname);

	// InputRoomNumber
	UFUNCTION()
	void OnQuitInputRoomNumberButton();

	void ShowRoomError(int32 RoomNum);

	void MenuButtonClicked(UButton* Button);

	void ShowAddFriend(TArray<int32> SuccessArr, TArray<FString> NicknameArr);

	void ShowInputRoomNumberPopUp(bool Show);

	void ShowFriendPopUp(bool Show);

	void AddRoomList(int32 RoomNumber, FString Nickname);

	void ShowRoomNotExist(bool Show);

	// Sound
	UPROPERTY(EditDefaultsOnly, Category = "Sound")
	TObjectPtr<class USoundWave> MouseClickSound;

	UPROPERTY(EditDefaultsOnly, Category = "Sound")
	TObjectPtr<class USoundWave> MouseHoverSound;


#pragma region Option

public:
	bool IsInputRoomNumberCanvasVisible() const;
	FORCEINLINE class UOptionsUMG* GetOptionsUMG() const { return OptionsUMG; }

	void SetClickedButton();
private:
	UFUNCTION()
	void OnOptionButton();


private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UOptionsUMG> OptionsUMG;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> OptionButton;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* OptionTextBlock;

	TArray<UButton*> ButtonArray;
	TArray<UTextBlock*> TextBlockArray;

	UButton* ClickedButton;

#pragma endregion Option


#pragma region SystemMenu

public:
	FORCEINLINE UImage* GetTrapperImage() const { return TrapperImage; }

private:
	UFUNCTION()
	void OnClickNotifaiButton();

	UFUNCTION()
	void OnHoveredNotifaiButton();

	UFUNCTION()
	void OnUnhoveredNotifaiButton();

private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UCanvasPanel> NotifaiCanvas;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> NotifaiButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UImage> ImpactImage;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UImage> RejectImage;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UImage> AcceptImage;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> FriendText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UImage> TrapperImage;

#pragma endregion SystemMenu
};
