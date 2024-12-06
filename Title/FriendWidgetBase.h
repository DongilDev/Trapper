// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FriendWidgetBase.generated.h"

/**
 * 
 */

enum AddFrienResult
{
	AddSuccess = 1,
	NotExistFriendId,
	AlreadyFriend,
	AlreadyRequest,
	FriendAlreadyRequests,
};

enum FriendMenu
{
	FriendList = 1,
	FriendRequest,
	AddFriend,
};

UCLASS()
class TRAPPERPROJECT_API UFriendWidgetBase : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	// 타이틀에서 친구 탭 눌렀을 때 기본적으로 나오는 것들
	UPROPERTY(meta = (BindWidget))
	class UButton* FriendListButton;
	
	UPROPERTY(meta = (BindWidget))
	class UButton* FriendRequestButton;

	UPROPERTY(meta = (BindWidget))
	class UButton* AddFriendButton;
	
	UPROPERTY(meta = (BindWidget))
	class UButton* FriendToTitleButton;

	// 친구 목록 UI
	UPROPERTY(meta = (BindWidget))
	class UCanvasPanel* FriendListCanvasPanel;

	UPROPERTY(meta = (BindWidget))
	class UGridPanel* FriendListGridPanel;

	UPROPERTY(meta = (BindWidget))
	class UButton* FriendListNextPageButton;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* FriendListPageTextBlock;

	UPROPERTY(meta = (BindWidget))
	class UButton* FriendListPreviousPageButton;

	// 요청 목록 UI
	UPROPERTY(meta = (BindWidget))
	class UCanvasPanel* FriendRequestCanvasPanel;

	UPROPERTY(meta = (BindWidget))
	class UGridPanel* FriendRequestGridPanel;

	UPROPERTY(meta = (BindWidget))
	UButton* FriendRequestNextPageButton;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* FriendRequestPageTextBlock;

	UPROPERTY(meta = (BindWidget))
	UButton* FriendRequestPreviousPageButton;

	// 친구 추가 UI
	UPROPERTY(meta = (BindWidget))
	class UCanvasPanel* AddFriendCanvasPanel;

	UPROPERTY(meta = (BindWidget))
	class UGridPanel* AddFriendGridPanel;

	UPROPERTY(meta = (BindWidget))
	class UEditableTextBox* InputFriendId;

	UPROPERTY(meta = (BindWidget))
	class UButton* SendInputFriendButton;

	// 동적으로 할당할 위젯들 (친구 목록, 요청 목록 등)
	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<class UFriendEntryWidgetBase> FriendEntryWidgetClass;

	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<class UFriendRequestWidgetBase> FriendRequestWidgetClass;
	
	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<class UAddFriendWidgetBase> AddFriendWidgetClass;

	// 함수들
	// 돌아가기 버튼 눌렀을 때
	UFUNCTION()
	void OnQuitFriendButton();

	// 친구 목록 버튼 눌렀을 때
	UFUNCTION()
	void OnFriendListButton();

	// 친구 목록 보여주는 함수
	UFUNCTION()
	void PopulateFriendsList();

	void AddFriendToList(const FString& FriendKey, bool bIsOnline);

	void UpdateFriendList();

	void UpdateFriendPage(TArray<UUserWidget*>& AllFriends);

	// 친구 요청 버튼 눌렀을 때
	UFUNCTION()
	void OnFriendRequestButton();

	void UpdateRequestsPage();

	// 친구 추가 버튼 눌렀을 때
	UFUNCTION()
	void OnAddFriendButton();

	void ShowAddFriend(TArray<int32> SuccessArr, TArray<FString> NicknameArr);

	// 친구 추가창에서 ㄹㅇ 친구추가 버튼 눌렀을 때
	UFUNCTION()
	void OnSendInputFriendButton();

	// 친구 요청 목록 보여주는 함수
	UFUNCTION()
	void PopulateFriendsRequest();

	UFUNCTION()
	void OnFriendListNextPageClicked();

	UFUNCTION()
	void OnFriendListPreviousPageClicked();

	UFUNCTION()
	void OnFriendRequestNextPageClicked();

	UFUNCTION()
	void OnFriendRequestPreviousPageClicked();

	void EnablePageSwipeButton(UButton* Button);
	void AblePageSwipeButton(UButton* Button);

	void InitFriendList();

	// Sound
	UPROPERTY(EditDefaultsOnly, Category = "Sound")
	TObjectPtr<class USoundWave> MouseClickSound;

	UPROPERTY(EditDefaultsOnly, Category = "Sound")
	TObjectPtr<class USoundWave> MouseHoverSound;

	UFUNCTION()
	void OnButtonHovered();

private:
	TArray<UUserWidget*> OnlineFriendWidgets;
	TArray<UUserWidget*> OfflineFriendWidgets;

	int32 CurrentPage = 0;
	const int32 MaxWidgetsPerPage = 15;

	int32 CurrentMenu = 0;

	// 친구 목록, 친구 요청, 친구 추가 버튼 UI 텍스쳐
	// 친구 목록
	UTexture2D* ListButtonNormalTexture;
	UTexture2D* ListButtonHoveredTexture;
	UTexture2D* ListButtonClickedTexture;
	// 친구 요청
	UTexture2D* RequestButtonNormalTexture;
	UTexture2D* RequestButtonHoveredTexture;
	UTexture2D* RequestButtonClickedTexture;
	// 친구 추가
	UTexture2D* AddButtonNormalTexture;
	UTexture2D* AddButtonHoveredTexture;
	UTexture2D* AddButtonClickedTexture;

};
