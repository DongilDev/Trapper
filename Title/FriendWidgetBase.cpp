// Fill out your copyright notice in the Description page of Project Settings.


#include "Title/FriendWidgetBase.h"
#include "Title/TitleWidgetBase.h"
#include "Components/Button.h"
#include "Components/EditableTextBox.h"
#include "Components/VerticalBox.h"
#include "Components/TextBlock.h"
#include "Components/CanvasPanel.h"
#include "Components/GridPanel.h"
#include "Components/GridSlot.h"
#include "Components/Image.h"
#include "Kismet/GameplayStatics.h"
#include "Title/TitlePC.h"
#include "TrapperGameInstance.h"
#include "Protocol.pb.h"
#include "ServerPacketHandler.h"
#include "FriendEntryWidgetBase.h"
#include "FriendRequestWidgetBase.h"
#include "AddFriendWidgetBase.h"

void UFriendWidgetBase::NativeConstruct()
{
	Super::NativeConstruct();

	// 기본
	if (FriendListButton)
	{
		FriendListButton->OnClicked.AddDynamic(this, &UFriendWidgetBase::OnFriendListButton);
		FriendListButton->OnHovered.AddDynamic(this, &UFriendWidgetBase::OnButtonHovered);
	}

	if (FriendRequestButton)
	{
		FriendRequestButton->OnClicked.AddDynamic(this, &UFriendWidgetBase::OnFriendRequestButton);
		FriendRequestButton->OnHovered.AddDynamic(this, &UFriendWidgetBase::OnButtonHovered);
	}

	if (AddFriendButton)
	{
		AddFriendButton->OnClicked.AddDynamic(this, &UFriendWidgetBase::OnAddFriendButton);
		AddFriendButton->OnHovered.AddDynamic(this, &UFriendWidgetBase::OnButtonHovered);
	}

	if (FriendToTitleButton)
	{
		FriendToTitleButton->OnClicked.AddDynamic(this, &UFriendWidgetBase::OnQuitFriendButton);
		FriendToTitleButton->OnHovered.AddDynamic(this, &UFriendWidgetBase::OnButtonHovered);
	}

	// 친구 목록 UI
	if (FriendListNextPageButton)
	{
		FriendListNextPageButton->OnClicked.AddDynamic(this, &UFriendWidgetBase::OnFriendListNextPageClicked);
		FriendListNextPageButton->OnHovered.AddDynamic(this, &UFriendWidgetBase::OnButtonHovered);
	}
	if (FriendListPreviousPageButton)
	{
		FriendListPreviousPageButton->OnClicked.AddDynamic(this, &UFriendWidgetBase::OnFriendListPreviousPageClicked);
		FriendListPreviousPageButton->OnHovered.AddDynamic(this, &UFriendWidgetBase::OnButtonHovered);
	}

	// 친구 요청 UI
	if (FriendRequestNextPageButton)
	{
		FriendRequestNextPageButton->OnClicked.AddDynamic(this, &UFriendWidgetBase::OnFriendRequestNextPageClicked);
		FriendRequestNextPageButton->OnHovered.AddDynamic(this, &UFriendWidgetBase::OnButtonHovered);
	}
	if (FriendRequestPreviousPageButton)
	{
		FriendRequestPreviousPageButton->OnClicked.AddDynamic(this, &UFriendWidgetBase::OnFriendRequestPreviousPageClicked);
		FriendRequestPreviousPageButton->OnHovered.AddDynamic(this, &UFriendWidgetBase::OnButtonHovered);
	}

	if (SendInputFriendButton)
	{
		SendInputFriendButton->OnClicked.AddDynamic(this, &UFriendWidgetBase::OnSendInputFriendButton);
		SendInputFriendButton->OnHovered.AddDynamic(this, &UFriendWidgetBase::OnButtonHovered);
	}

	FriendRequestCanvasPanel->SetVisibility(ESlateVisibility::Collapsed);
	AddFriendCanvasPanel->SetVisibility(ESlateVisibility::Collapsed);

	// 친구 목록
	ListButtonNormalTexture = Cast<UTexture2D>(StaticLoadObject(UTexture2D::StaticClass(), nullptr, *FString("/Script/Engine.Texture2D'/Game/Blueprints/Title/Assets/F_Btn_CTGR_List.F_Btn_CTGR_List'")));
	ListButtonHoveredTexture = Cast<UTexture2D>(StaticLoadObject(UTexture2D::StaticClass(), nullptr, *FString("/Script/Engine.Texture2D'/Game/Blueprints/Title/Assets/F_Btn_CTGR_List_Impact.F_Btn_CTGR_List_Impact'")));
	ListButtonClickedTexture = Cast<UTexture2D>(StaticLoadObject(UTexture2D::StaticClass(), nullptr, *FString("/Script/Engine.Texture2D'/Game/Blueprints/Title/Assets/F_Btn_CTGR_List_Actv.F_Btn_CTGR_List_Actv'")));
	// 친구 요청
	RequestButtonNormalTexture = Cast<UTexture2D>(StaticLoadObject(UTexture2D::StaticClass(), nullptr, *FString("/Script/Engine.Texture2D'/Game/Blueprints/Title/Assets/F_Btn_CTGR_request.F_Btn_CTGR_request'")));
	RequestButtonHoveredTexture = Cast<UTexture2D>(StaticLoadObject(UTexture2D::StaticClass(), nullptr, *FString("/Script/Engine.Texture2D'/Game/Blueprints/Title/Assets/F_Btn_CTGR_request_Impact.F_Btn_CTGR_request_Impact'")));
	RequestButtonClickedTexture = Cast<UTexture2D>(StaticLoadObject(UTexture2D::StaticClass(), nullptr, *FString("/Script/Engine.Texture2D'/Game/Blueprints/Title/Assets/F_Btn_CTGR_request_Actv.F_Btn_CTGR_request_Actv'")));
	// 친구 추가
	AddButtonNormalTexture = Cast<UTexture2D>(StaticLoadObject(UTexture2D::StaticClass(), nullptr, *FString("/Script/Engine.Texture2D'/Game/Blueprints/Title/Assets/F_Btn_CTGR_add.F_Btn_CTGR_add'")));
	AddButtonHoveredTexture = Cast<UTexture2D>(StaticLoadObject(UTexture2D::StaticClass(), nullptr, *FString("/Script/Engine.Texture2D'/Game/Blueprints/Title/Assets/F_Btn_CTGR_add_Impact.F_Btn_CTGR_add_Impact'")));
	AddButtonClickedTexture = Cast<UTexture2D>(StaticLoadObject(UTexture2D::StaticClass(), nullptr, *FString("/Script/Engine.Texture2D'/Game/Blueprints/Title/Assets/F_Btn_CTGR_add_Actv.F_Btn_CTGR_add_Actv'")));
}

void UFriendWidgetBase::OnQuitFriendButton()
{
	UGameplayStatics::PlaySound2D(this, MouseClickSound);

	ATitlePC* PC = GetOwningPlayer<ATitlePC>();
	if (PC)
	{
		PC->ShowFriendPopUp(false);
		PC->SetClickedButton();
	}

	// 버튼 초기화
	if (CurrentMenu == FriendList)
	{
	}
	else if (CurrentMenu == FriendRequest)
	{
		FButtonStyle Style1 = FriendRequestButton->GetStyle();
		Style1.Normal.SetResourceObject(RequestButtonNormalTexture);
		Style1.Hovered.SetResourceObject(RequestButtonHoveredTexture);
		Style1.Hovered.SetImageSize(Style1.Hovered.GetImageSize());
		Style1.Pressed.SetResourceObject(RequestButtonClickedTexture);
		Style1.Pressed.SetImageSize(Style1.Pressed.GetImageSize());
		FriendRequestButton->SetStyle(Style1);
	}
	else if (CurrentMenu == AddFriend)
	{
		FButtonStyle Style2 = AddFriendButton->GetStyle();
		Style2.Normal.SetResourceObject(AddButtonNormalTexture);
		Style2.Hovered.SetResourceObject(AddButtonHoveredTexture);
		Style2.Hovered.SetImageSize(Style2.Hovered.GetImageSize());
		Style2.Pressed.SetResourceObject(AddButtonClickedTexture);
		Style2.Pressed.SetImageSize(Style2.Pressed.GetImageSize());
		AddFriendButton->SetStyle(Style2);
	}
	
	CurrentMenu = 0;
}

void UFriendWidgetBase::OnFriendListButton()
{
	UGameplayStatics::PlaySound2D(this, MouseClickSound);

	// 버튼 UI 바꾸기
	if(CurrentMenu != FriendList)
	{
		FButtonStyle Style = FriendListButton->GetStyle();
		Style.Normal.SetResourceObject(ListButtonClickedTexture);
		Style.Hovered.SetResourceObject(ListButtonClickedTexture);
		Style.Hovered.SetImageSize(Style.Hovered.GetImageSize());
		Style.Pressed.SetResourceObject(ListButtonClickedTexture);
		Style.Pressed.SetImageSize(Style.Pressed.GetImageSize());
		FriendListButton->SetStyle(Style);
	}

	if(CurrentMenu == FriendRequest)
	{
		FButtonStyle Style1 = FriendRequestButton->GetStyle();
		Style1.Normal.SetResourceObject(RequestButtonNormalTexture);
		Style1.Hovered.SetResourceObject(RequestButtonHoveredTexture);
		Style1.Hovered.SetImageSize(Style1.Hovered.GetImageSize());
		Style1.Pressed.SetResourceObject(RequestButtonClickedTexture);
		Style1.Pressed.SetImageSize(Style1.Pressed.GetImageSize());
		FriendRequestButton->SetStyle(Style1);
	}
	else if(CurrentMenu == AddFriend)
	{
		FButtonStyle Style2 = AddFriendButton->GetStyle();
		Style2.Normal.SetResourceObject(AddButtonNormalTexture);
		Style2.Hovered.SetResourceObject(AddButtonHoveredTexture);
		Style2.Hovered.SetImageSize(Style2.Hovered.GetImageSize());
		Style2.Pressed.SetResourceObject(AddButtonClickedTexture);
		Style2.Pressed.SetImageSize(Style2.Pressed.GetImageSize());
		AddFriendButton->SetStyle(Style2);
	}

	CurrentMenu = FriendList;

	// 친구 목록 UI 등장
	FriendListCanvasPanel->SetVisibility(ESlateVisibility::Visible);
	FriendRequestCanvasPanel->SetVisibility(ESlateVisibility::Collapsed);
	AddFriendCanvasPanel->SetVisibility(ESlateVisibility::Collapsed);

	// 페이지 초기화
	FString Page = FString::FromInt(1);
	FriendListPageTextBlock->SetText(FText::FromString(Page));

	CurrentPage = 0;

	EnablePageSwipeButton(FriendListPreviousPageButton);
	if (Cast<UTrapperGameInstance>(GetGameInstance())->GetFriends().Num() < MaxWidgetsPerPage)
	{
		EnablePageSwipeButton(FriendListNextPageButton);
	}
	else
	{
		AblePageSwipeButton(FriendListNextPageButton);
	}

	// 친구 목록 보여주기
	PopulateFriendsList();
}

void UFriendWidgetBase::OnFriendRequestButton()
{
	UGameplayStatics::PlaySound2D(this, MouseClickSound);

	if (CurrentMenu != FriendRequest)
	{
		FButtonStyle Style = FriendRequestButton->GetStyle();
		Style.Normal.SetResourceObject(RequestButtonClickedTexture);
		Style.Hovered.SetResourceObject(RequestButtonClickedTexture);
		Style.Hovered.SetImageSize(Style.Hovered.GetImageSize());
		Style.Pressed.SetResourceObject(RequestButtonClickedTexture);
		Style.Pressed.SetImageSize(Style.Pressed.GetImageSize());
		FriendRequestButton->SetStyle(Style);
	}

	if(CurrentMenu == FriendList)
	{
		FButtonStyle Style1 = FriendListButton->GetStyle();
		Style1.Normal.SetResourceObject(ListButtonNormalTexture);
		Style1.Hovered.SetResourceObject(ListButtonHoveredTexture);
		Style1.Hovered.SetImageSize(Style1.Hovered.GetImageSize());
		Style1.Pressed.SetResourceObject(ListButtonClickedTexture);
		Style1.Pressed.SetImageSize(Style1.Pressed.GetImageSize());
		FriendListButton->SetStyle(Style1);
	}

	else if(CurrentMenu == AddFriend)
	{
		FButtonStyle Style2 = AddFriendButton->GetStyle();
		Style2.Normal.SetResourceObject(AddButtonNormalTexture);
		Style2.Hovered.SetResourceObject(AddButtonHoveredTexture);
		Style2.Hovered.SetImageSize(Style2.Hovered.GetImageSize());
		Style2.Pressed.SetResourceObject(AddButtonClickedTexture);
		Style2.Pressed.SetImageSize(Style2.Pressed.GetImageSize());
		AddFriendButton->SetStyle(Style2);
	}

	CurrentMenu = FriendRequest;

	// 친구 추가 ui 없애고
	FriendListCanvasPanel->SetVisibility(ESlateVisibility::Collapsed);
	FriendRequestCanvasPanel->SetVisibility(ESlateVisibility::Visible);
	AddFriendCanvasPanel->SetVisibility(ESlateVisibility::Collapsed);

	// 페이지 초기화
	FString Page = FString::FromInt(1);
	FriendRequestPageTextBlock->SetText(FText::FromString(Page));

	CurrentPage = 0;

	EnablePageSwipeButton(FriendRequestPreviousPageButton);
	if (Cast<UTrapperGameInstance>(GetGameInstance())->GetFriendRequests().Num() < MaxWidgetsPerPage)
	{
		EnablePageSwipeButton(FriendRequestNextPageButton);
	}
	else
	{
		AblePageSwipeButton(FriendRequestNextPageButton);
	}

	// 친구 요청 목록 보여주기
	PopulateFriendsRequest();
}

void UFriendWidgetBase::OnAddFriendButton()
{
	UGameplayStatics::PlaySound2D(this, MouseClickSound);

	if (CurrentMenu != AddFriend)
	{
		FButtonStyle Style = AddFriendButton->GetStyle();
		Style.Normal.SetResourceObject(AddButtonClickedTexture);
		Style.Hovered.SetResourceObject(AddButtonClickedTexture);
		Style.Hovered.SetImageSize(Style.Hovered.GetImageSize());
		Style.Pressed.SetResourceObject(AddButtonClickedTexture);
		Style.Pressed.SetImageSize(Style.Pressed.GetImageSize());
		AddFriendButton->SetStyle(Style);
	}

	if(CurrentMenu == FriendList)
	{
		FButtonStyle Style1 = FriendListButton->GetStyle();
		Style1.Normal.SetResourceObject(ListButtonNormalTexture);
		Style1.Hovered.SetResourceObject(ListButtonHoveredTexture);
		Style1.Hovered.SetImageSize(Style1.Hovered.GetImageSize());
		Style1.Pressed.SetResourceObject(ListButtonClickedTexture);
		Style1.Pressed.SetImageSize(Style1.Pressed.GetImageSize());
		FriendListButton->SetStyle(Style1);
	}
	else if(CurrentMenu == FriendRequest)
	{
		FButtonStyle Style2 = FriendRequestButton->GetStyle();
		Style2.Normal.SetResourceObject(RequestButtonNormalTexture);
		Style2.Hovered.SetResourceObject(RequestButtonHoveredTexture);
		Style2.Hovered.SetImageSize(Style2.Hovered.GetImageSize());
		Style2.Pressed.SetResourceObject(RequestButtonClickedTexture);
		Style2.Pressed.SetImageSize(Style2.Pressed.GetImageSize());
		FriendRequestButton->SetStyle(Style2);
	}

	CurrentMenu = AddFriend;

	FriendListCanvasPanel->SetVisibility(ESlateVisibility::Collapsed);
	FriendRequestCanvasPanel->SetVisibility(ESlateVisibility::Collapsed);
	AddFriendCanvasPanel->SetVisibility(ESlateVisibility::Visible);

	AddFriendGridPanel->ClearChildren();
}

void UFriendWidgetBase::ShowAddFriend(TArray<int32> SuccessArr, TArray<FString> NicknameArr)
{
	AddFriendGridPanel->ClearChildren();

	for (int32 i = 0; i < SuccessArr.Num(); i++)
	{
		UAddFriendWidgetBase* AddFriend = CreateWidget<UAddFriendWidgetBase>(this, AddFriendWidgetClass);

		UTextBlock* AddFriendText = Cast<UTextBlock>(AddFriend->GetWidgetFromName(TEXT("AddFriendText")));
		UButton* AddFriendStatusButton = Cast<UButton>(AddFriend->GetWidgetFromName(TEXT("AddFriendStatusButton")));

		AddFriendText->SetText(FText::FromString(NicknameArr[i]));

		if (SuccessArr[i] == AddSuccess)
		{

		}
		else if (SuccessArr[i] == AlreadyRequest)
		{
			FString StatusTexturePath = "/Script/Engine.Texture2D'/Game/Blueprints/Title/Assets/F2_Btn_Wait.F2_Btn_Wait'";
			UTexture2D* StatusTexture = Cast<UTexture2D>(StaticLoadObject(UTexture2D::StaticClass(), nullptr, *StatusTexturePath));

			FButtonStyle  Style = AddFriendStatusButton->GetStyle();
			Style.Normal.SetResourceObject(StatusTexture);
			AddFriendStatusButton->SetStyle(Style);
			AddFriendStatusButton->SetIsEnabled(false);
		}

		int32 MaxRow = 3;
		int32 NumChildren = AddFriendGridPanel->GetChildrenCount();
		int32 Row = NumChildren / MaxRow;
		int32 Column = NumChildren % MaxRow;

		UGridSlot* GridSlot = AddFriendGridPanel->AddChildToGrid(AddFriend);
		GridSlot->SetRow(Row);
		GridSlot->SetColumn(Column);
	}
}

void UFriendWidgetBase::OnSendInputFriendButton()
{
	UGameplayStatics::PlaySound2D(this, MouseClickSound);

	FString FNickname = InputFriendId->GetText().ToString();

	if (FNickname == "")
	{
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("Nickname: %s"), *FNickname);

	Protocol::C_CHECK_FRIEND ckFriend;
	ckFriend.set_mynickname(std::string(TCHAR_TO_UTF8(*Cast<UTrapperGameInstance>(GetGameInstance())->GetPlayerNickname())));
	ckFriend.set_requestnickname(std::string(TCHAR_TO_UTF8(*FNickname)));

	SendBufferRef SendBuffer = ServerPacketHandler::MakeSendBuffer(ckFriend);
	Cast<UTrapperGameInstance>(GetGameInstance())->SendPacket(SendBuffer);
}

void UFriendWidgetBase::PopulateFriendsList()
{
	// 기존 친구 리스트 초기화
	FriendListGridPanel->ClearChildren();
	UpdateFriendList();
	InitFriendList();
}

void UFriendWidgetBase::PopulateFriendsRequest()
{
	FriendRequestGridPanel->ClearChildren();
	UpdateRequestsPage();
	
}

void UFriendWidgetBase::AddFriendToList(const FString& FriendKey, bool bIsOnline)
{
	UFriendEntryWidgetBase* FriendEntry = CreateWidget<UFriendEntryWidgetBase>(this, FriendEntryWidgetClass);
	if (FriendEntry)
	{
		UTextBlock* FriendNameTextBlock = Cast<UTextBlock>(FriendEntry->GetWidgetFromName(TEXT("FriendNameTextBlock")));
		UImage* FriendStatusImage = Cast<UImage>(FriendEntry->GetWidgetFromName(TEXT("FriendStatusImage")));
		UImage* ActiveFriendImage = Cast<UImage>(FriendEntry->GetWidgetFromName(TEXT("ActiveFriendImage")));
		
		UTexture2D* OnlineBoxTexture = LoadObject<UTexture2D>(nullptr, TEXT("/Script/Engine.Texture2D'/Game/Blueprints/Title/Assets/F_Btn_Nametag_Impact.F_Btn_Nametag_Impact'"));
		UTexture2D* OfflineBoxTexture = LoadObject<UTexture2D>(nullptr, TEXT("/Script/Engine.Texture2D'/Game/Blueprints/Title/Assets/F_Btn_Nametag.F_Btn_Nametag'"));

		UTexture2D* OnlineStatusTexture = LoadObject<UTexture2D>(nullptr, TEXT("/Script/Engine.Texture2D'/Game/Blueprints/Title/Assets/F_Img_Off.F_Img_Off'"));
		UTexture2D* OfflineStatusTexture = LoadObject<UTexture2D>(nullptr, TEXT("/Script/Engine.Texture2D'/Game/Blueprints/Title/Assets/F_Img_On.F_Img_On'"));

		if(!OfflineBoxTexture)
			UE_LOG(LogTemp, Warning, TEXT("OfflineBoxTexture Null"));
		if (!OfflineStatusTexture)
			UE_LOG(LogTemp, Warning, TEXT("OfflineStatusTexture Null"));

		if (FriendNameTextBlock && FriendStatusImage)
		{
			FriendNameTextBlock->SetText(FText::FromString(FriendKey));

			if (bIsOnline)
			{
				ActiveFriendImage->SetBrushFromTexture(OnlineBoxTexture);
				FriendStatusImage->SetBrushFromTexture(OnlineStatusTexture);
				OnlineFriendWidgets.Add(FriendEntry);
			}
			else
			{
				ActiveFriendImage->SetBrushFromTexture(OfflineBoxTexture);
				FriendStatusImage->SetBrushFromTexture(OfflineStatusTexture);
				OfflineFriendWidgets.Add(FriendEntry);
				UE_LOG(LogTemp, Warning, TEXT("OfflineTexture Changed"));
			}
		}
	}
}

void UFriendWidgetBase::InitFriendList()
{
	TArray<UUserWidget*> AllFriends;
	AllFriends.Append(OnlineFriendWidgets);
	AllFriends.Append(OfflineFriendWidgets);

	UpdateFriendPage(AllFriends);
}

void UFriendWidgetBase::OnButtonHovered()
{
	UGameplayStatics::PlaySound2D(this, MouseHoverSound);
}

void UFriendWidgetBase::UpdateFriendList()
{
	// 친구목록 UI 갱신
	TMap<FString, bool>& FriendsMap = Cast<UTrapperGameInstance>(GetGameInstance())->GetFriends();

	if (!FriendListGridPanel || !FriendEntryWidgetClass)
	{
		return;
	}

	// 친구 목록 정렬 및 분류
	TArray<FString> SortedKeys;
	FriendsMap.GetKeys(SortedKeys);
	SortedKeys.Sort();  // 문자열로 정렬

	TArray<FString> OnlineFriends;
	TArray<FString> OfflineFriends;

	for (const FString& FriendKey : SortedKeys)
	{
		if (FriendsMap[FriendKey])
		{
			OnlineFriends.Add(FriendKey);
		}
		else
		{
			OfflineFriends.Add(FriendKey);
		}
	}

	OnlineFriendWidgets.Empty();
	OfflineFriendWidgets.Empty();

	// 온라인 친구 먼저 추가
	for (const FString& FriendKey : OnlineFriends)
	{
		AddFriendToList(FriendKey, true);
	}

	// 오프라인 친구 추가
	for (const FString& FriendKey : OfflineFriends)
	{
		AddFriendToList(FriendKey, false);
	}
}

void UFriendWidgetBase::UpdateFriendPage(TArray<UUserWidget*>& AllFriends)
{
	// 기존 친구 리스트 초기화
	FriendListGridPanel->ClearChildren();

	int32 StartIndex = CurrentPage * MaxWidgetsPerPage;
	int32 EndIndex = FMath::Min(StartIndex + MaxWidgetsPerPage, AllFriends.Num());

	for (int32 i = StartIndex; i < EndIndex; i++)
	{
		int32 MaxRow = 3;
		int32 NumChildren = FriendListGridPanel->GetChildrenCount();
		int32 Row = NumChildren / MaxRow;
		int32 Column = NumChildren % MaxRow;

		UGridSlot* GridSlot = FriendListGridPanel->AddChildToGrid(AllFriends[i]);
		GridSlot->SetRow(Row);
		GridSlot->SetColumn(Column);
	}

	// 나머지는 빈칸으로 해주기
	if (EndIndex - StartIndex + 1 != MaxWidgetsPerPage)
	{
		for (int32 i = EndIndex - StartIndex; i < MaxWidgetsPerPage; i++)
		{
			UFriendEntryWidgetBase* FriendEntry = CreateWidget<UFriendEntryWidgetBase>(this, FriendEntryWidgetClass);
			if (FriendEntry)
			{
				UTextBlock* FriendNameTextBlock = Cast<UTextBlock>(FriendEntry->GetWidgetFromName(TEXT("FriendNameTextBlock")));
				UImage* FriendStatusImage = Cast<UImage>(FriendEntry->GetWidgetFromName(TEXT("FriendStatusImage")));
				UImage* ActiveFriendImage = Cast<UImage>(FriendEntry->GetWidgetFromName(TEXT("ActiveFriendImage")));

				UTexture2D* NonTexture = LoadObject<UTexture2D>(nullptr, TEXT("/Script/Engine.Texture2D'/Game/Blueprints/Title/Assets/F_Btn_Nametag_null.F_Btn_Nametag_null'"));
				
				if (FriendNameTextBlock && FriendStatusImage)
				{
					FriendNameTextBlock->SetText(FText::FromString(""));
					FriendStatusImage->SetVisibility(ESlateVisibility::Collapsed);
					ActiveFriendImage->SetBrushFromTexture(NonTexture);
				}
			}

			int32 MaxRow = 3;
			int32 NumChildren = FriendListGridPanel->GetChildrenCount();
			int32 Row = NumChildren / MaxRow;
			int32 Column = NumChildren % MaxRow;

			UGridSlot* GridSlot = FriendListGridPanel->AddChildToGrid(FriendEntry);
			GridSlot->SetRow(Row);
			GridSlot->SetColumn(Column);
			/*FMargin GridPadding(0.0f, 10.0f, 0.0f, 10.0f);
			GridSlot->SetPadding(GridPadding);*/
		}
	}
}

void UFriendWidgetBase::UpdateRequestsPage()
{
	TMap<FString, FString>& FriendRequestsMap = Cast<UTrapperGameInstance>(GetGameInstance())->GetFriendRequests();

	TArray<TPair<FString, FString>> FriendRequestsArray;
	for (const TPair<FString, FString>& Entry : FriendRequestsMap)
	{
		FriendRequestsArray.Add(Entry);
	}

	if (!FriendRequestGridPanel || !FriendRequestWidgetClass)
	{
		return;
	}
	int32 RequestsNum = Cast<UTrapperGameInstance>(GetGameInstance())->GetFriendRequests().Num();

	int32 StartIndex = CurrentPage * MaxWidgetsPerPage;
	int32 EndIndex = FMath::Min(StartIndex + MaxWidgetsPerPage, RequestsNum);

	for (int32 i = StartIndex; i < EndIndex; i++)
	{
		int32 MaxRow = 3;
		int32 NumChildren = FriendRequestGridPanel->GetChildrenCount();
		int32 Row = NumChildren / MaxRow;
		int32 Column = NumChildren % MaxRow;

		UFriendRequestWidgetBase* FriendRequest = CreateWidget<UFriendRequestWidgetBase>(this, FriendRequestWidgetClass);
		FriendRequest->RequestFriendIdText->SetText(FText::FromString(FriendRequestsArray[i].Value));
		UGridSlot* GridSlot = FriendRequestGridPanel->AddChildToGrid(FriendRequest);
		GridSlot->SetRow(Row);
		GridSlot->SetColumn(Column);
		FriendRequest->SetFriendWidget(this);
	}

	// 나머지는 빈칸으로 해주기
	if (EndIndex - StartIndex + 1 != MaxWidgetsPerPage)
	{
		for (int32 i = EndIndex - StartIndex; i < MaxWidgetsPerPage; i++)
		{
			UFriendRequestWidgetBase* FriendRequest = CreateWidget<UFriendRequestWidgetBase>(this, FriendRequestWidgetClass);
			
			if (FriendRequest)
			{
				UTextBlock* RequestFriendIdText = Cast<UTextBlock>(FriendRequest->GetWidgetFromName(TEXT("RequestFriendIdText")));
				UImage* RequestFriendImage = Cast<UImage>(FriendRequest->GetWidgetFromName(TEXT("RequestFriendImage")));
				UButton* AcceptButton = Cast<UButton>(FriendRequest->GetWidgetFromName(TEXT("AcceptButton")));
				UButton* RejectButton = Cast<UButton>(FriendRequest->GetWidgetFromName(TEXT("RejectButton")));

				UTexture2D* NonTexture = LoadObject<UTexture2D>(nullptr, TEXT("/Script/Engine.Texture2D'/Game/Blueprints/Title/Assets/F_Btn_Nametag_null.F_Btn_Nametag_null'"));

				if (RequestFriendIdText && RequestFriendImage)
				{
					RequestFriendIdText->SetText(FText::FromString(""));
					AcceptButton->SetVisibility(ESlateVisibility::Collapsed);
					RejectButton->SetVisibility(ESlateVisibility::Collapsed);
					RequestFriendImage->SetBrushFromTexture(NonTexture);
				}
			}

			int32 MaxRow = 3;
			int32 NumChildren = FriendRequestGridPanel->GetChildrenCount();
			int32 Row = NumChildren / MaxRow;
			int32 Column = NumChildren % MaxRow;

			UGridSlot* GridSlot = FriendRequestGridPanel->AddChildToGrid(FriendRequest);
			GridSlot->SetRow(Row);
			GridSlot->SetColumn(Column);
		}
	}

}

void UFriendWidgetBase::OnFriendListNextPageClicked()
{
	UGameplayStatics::PlaySound2D(this, MouseClickSound);

	UpdateFriendList();

	TArray<UUserWidget*> AllFriends;
	AllFriends.Append(OnlineFriendWidgets);
	AllFriends.Append(OfflineFriendWidgets);

	int32 TotalFriends = OnlineFriendWidgets.Num() + OfflineFriendWidgets.Num();
	if ((CurrentPage + 1) * MaxWidgetsPerPage < TotalFriends)
	{
		CurrentPage++; 
		UpdateFriendPage(AllFriends);

		if (!FriendListPreviousPageButton->GetIsEnabled())
		{
			// 이전 페이지 버튼 상태가 비활성화라면 활성화로 변경
			AblePageSwipeButton(FriendListPreviousPageButton);
		}
	}

	if((CurrentPage + 1) * MaxWidgetsPerPage > TotalFriends)
	{
		// 마지막 페이지일 때 다음 페이지 버튼 비활성화
		EnablePageSwipeButton(FriendListNextPageButton);
	}

	FString Page = FString::FromInt(CurrentPage + 1);
	FriendListPageTextBlock->SetText(FText::FromString(Page));
}

void UFriendWidgetBase::OnFriendListPreviousPageClicked()
{
	UGameplayStatics::PlaySound2D(this, MouseClickSound);

	UpdateFriendList();

	TArray<UUserWidget*> AllFriends;
	AllFriends.Append(OnlineFriendWidgets);
	AllFriends.Append(OfflineFriendWidgets);

	if (CurrentPage > 0)
	{
		CurrentPage--;
		UpdateFriendPage(AllFriends);

		FString NormalTexturePath = "/Script/Engine.Texture2D'/Game/Blueprints/Title/Assets/Btn_Swipe_Can.Btn_Swipe_Can'";
		UTexture2D* NormalTexture = Cast<UTexture2D>(StaticLoadObject(UTexture2D::StaticClass(), nullptr, *NormalTexturePath));

		FString HoveredTexturePath = "/Script/Engine.Texture2D'/Game/Blueprints/Title/Assets/Btn_Swipe_Impact.Btn_Swipe_Impact'";
		UTexture2D* HoveredTexture = Cast<UTexture2D>(StaticLoadObject(UTexture2D::StaticClass(), nullptr, *NormalTexturePath));

		if (!FriendListNextPageButton->GetIsEnabled())
		{
			// 마지막 페이지에서 이전 페이지 눌렀을 때 다음 페이지 버튼 활성화
			AblePageSwipeButton(FriendListNextPageButton);
		}
	}
	if(CurrentPage <= 0)
	{
		// 첫 페이지일 때 이전 페이지 버튼 비활성화
		EnablePageSwipeButton(FriendListPreviousPageButton);
	}

	FString Page = FString::FromInt(CurrentPage + 1);
	FriendListPageTextBlock->SetText(FText::FromString(Page));
}

void UFriendWidgetBase::OnFriendRequestNextPageClicked()
{
	UGameplayStatics::PlaySound2D(this, MouseClickSound);

	int32 TotalRequests = Cast<UTrapperGameInstance>(GetGameInstance())->GetFriendRequests().Num();
	if ((CurrentPage + 1) * MaxWidgetsPerPage < TotalRequests)
	{
		CurrentPage++;
		UpdateRequestsPage();
	}
	FString Page = FString::FromInt(CurrentPage + 1);
	FriendRequestPageTextBlock->SetText(FText::FromString(Page));
}

void UFriendWidgetBase::OnFriendRequestPreviousPageClicked()
{
	UGameplayStatics::PlaySound2D(this, MouseClickSound);

	if (CurrentPage > 0)
	{
		CurrentPage--;
		UpdateRequestsPage();
	}

	FString Page = FString::FromInt(CurrentPage + 1);
	FriendRequestPageTextBlock->SetText(FText::FromString(Page));
}

void UFriendWidgetBase::EnablePageSwipeButton(UButton* Button)
{
	FString NormalTexturePath = "/Script/Engine.Texture2D'/Game/Blueprints/Title/Assets/Btn_Swipe_CanNot.Btn_Swipe_CanNot'";
	UTexture2D* NormalTexture = Cast<UTexture2D>(StaticLoadObject(UTexture2D::StaticClass(), nullptr, *NormalTexturePath));

	// 버튼 비활성화
	FButtonStyle  Style = Button->GetStyle();
	Style.Normal.SetResourceObject(NormalTexture);
	Style.Hovered.SetResourceObject(NormalTexture);
	Style.Hovered.SetImageSize(Style.Hovered.GetImageSize());
	Style.Pressed.SetResourceObject(NormalTexture);
	Style.Pressed.SetImageSize(Style.Pressed.GetImageSize());
	Button->SetStyle(Style);
	Button->SetIsEnabled(false);
}

void UFriendWidgetBase::AblePageSwipeButton(UButton* Button)
{
	FString NormalTexturePath = "/Script/Engine.Texture2D'/Game/Blueprints/Title/Assets/Btn_Swipe_Can.Btn_Swipe_Can'";
	UTexture2D* NormalTexture = Cast<UTexture2D>(StaticLoadObject(UTexture2D::StaticClass(), nullptr, *NormalTexturePath));

	FString HoveredTexturePath = "/Script/Engine.Texture2D'/Game/Blueprints/Title/Assets/Btn_Swipe_Impact.Btn_Swipe_Impact'";
	UTexture2D* HoveredTexture = Cast<UTexture2D>(StaticLoadObject(UTexture2D::StaticClass(), nullptr, *HoveredTexturePath));

	// 버튼 활성화
	FButtonStyle  Style = Button->GetStyle();
	Style.Normal.SetResourceObject(NormalTexture);
	Style.Hovered.SetResourceObject(HoveredTexture);
	Style.Hovered.SetImageSize(Style.Hovered.GetImageSize());
	Style.Pressed.SetResourceObject(HoveredTexture);
	Style.Pressed.SetImageSize(Style.Pressed.GetImageSize());
	Button->SetStyle(Style);
	Button->SetIsEnabled(true);
}

