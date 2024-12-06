// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LobbyWidgetBase.generated.h"

/**
 *
 */
UCLASS()
class TRAPPERPROJECT_API ULobbyWidgetBase : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	class UEditableTextBox* ChatInput;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	class UScrollBox* ChatBox;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	class UTextBlock* RoomNumber;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	class UButton* ShowInvitationButton;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	class UButton* StartGameButton;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	class UButton* ClientReadyButton;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	class UButton* ExitLobby;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	class UImage* ClientPreparingImage;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	class UCanvasPanel* InvitationCanvasPanel;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	class UCanvasPanel* ClientReadyCanvasPanel;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* HostNickname;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* ClientNickname;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	class UVerticalBox* FriendListVerticalBox;

	UPROPERTY(meta = (BindWidget))
	class UBorder* ChatBoxBorder;

	// 카운트 다운
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	class UCanvasPanel* GameStartCanvasPanel;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	class UTextBlock* PublicMessage;

	// 로딩
	UPROPERTY(meta = (BindWidget))
	class UCanvasPanel* LoadingCanvasPanel;

	UPROPERTY(meta = (BindWidget))
	class UImage* BackgroundImage;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* TipsTextBlock;

	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<class UInvitationFriendListWidgetBase> InvitationFriendListWidgetClass;

	// 애니메이션
	UPROPERTY(meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* InviteFriendOpen;

	UPROPERTY(meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* InviteFriendClose;

	UFUNCTION()
	void OnButtonHovered();

	UFUNCTION()
	void PressStartGameButton();

	UFUNCTION()
	void PressReadyButton();

	UFUNCTION()
	void PressExitButton();

	void UpdateReadyButton(bool bIsReady);

	virtual void NativeConstruct() override;

	UFUNCTION()
	void ProcessTextCommited(const FText& Text, ETextCommit::Type CommitMethod);

	void AddMessage(FText Message, const FLinearColor& Color);

	//virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	void SetPublicMessage(int LeftTime);

	void ShowSecondPlayerNickname(bool Show, const FString& SecondNickname);

	void ShowFirstPlayerNickname(bool Show, const FString& FirstNickname);

	void ActiveStartButton(bool Ready);

	void DuringCountDown();

	UFUNCTION(BlueprintCallable, Category = "UI")
	void SetTimerVisibility(ESlateVisibility NewVisibility);

	UFUNCTION(BlueprintCallable, Category = "UI")
	void ShowCountDown();

	UFUNCTION()
	void ShowInvitationFriendButton(bool Show);

	UFUNCTION()
	void ShowInvitationFriendList();

	void ShowInvitationFriendList(bool Show);

	void AddFriendToList(const FString& FriendKey, bool bIsOnline);

	// Sound
	UPROPERTY(EditDefaultsOnly, Category = "Sound")
	TObjectPtr<class USoundWave> MouseClickSound;

	UPROPERTY(EditDefaultsOnly, Category = "Sound")
	TObjectPtr<class USoundWave> MouseHoverSound;

	UPROPERTY(EditDefaultsOnly, Category = "Sound")
	TObjectPtr<class USoundWave> SlideSound;

	UPROPERTY(EditDefaultsOnly, Category = "Sound")
	TObjectPtr<class USoundWave> CountDownSound;

private:
	bool GameStartPressed = false;

#pragma region SystemMenu

public:
	void OnBack();

	void ShowLoading();

	void ShowClientReadyCanvas(bool Show);

private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class USystemMenuUMG> SystemMenuUMG;

#pragma endregion SystemMenu
};
