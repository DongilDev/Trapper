// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyWidgetBase.h"
#include "Components/EditableTextBox.h"
#include "LobbyPC.h"
#include "Components/TextBlock.h"
#include "Components/ScrollBox.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/VerticalBox.h"
#include "Components/CanvasPanel.h"
#include "Components/Border.h"
#include "Components/GridPanel.h"
#include "Components/GridSlot.h"
#include "Components/Image.h"
#include "Engine/Texture2D.h"
#include "UObject/ConstructorHelpers.h"
#include "TrapperGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "InvitationFriendListWidgetBase.h"
#include "LobbyGS.h"
#include "LobbyGM.h"
#include "LobbyPS.h"
#include "Protocol.pb.h"
#include "ServerPacketHandler.h"
#include "Widget/SystemMenuUMG.h"

#include "MediaPlayer.h"
#include "MediaSource.h"
#include "MediaTexture.h"

void ULobbyWidgetBase::OnButtonHovered()
{
	UGameplayStatics::PlaySound2D(this, MouseHoverSound);
}

void ULobbyWidgetBase::PressStartGameButton()
{
	if (GameStartPressed)
	{
		return;
	}

	GameStartPressed = true;

	UGameplayStatics::PlaySound2D(this, CountDownSound);
	//UGameplayStatics::PlaySound2D(this, MouseClickSound);

	// 게임 시작 버튼 눌렀을 때
	ALobbyPC* PC = GetOwningPlayer<ALobbyPC>();
	if (PC)
	{
		// 시작 전 5초 카운트 다운
		PC->StartAfterTimer();
		PC->DeactiveButton();
		StartGameButton->SetVisibility(ESlateVisibility::HitTestInvisible);
		ExitLobby->SetVisibility(ESlateVisibility::HitTestInvisible);
	}

	ALobbyGM* GM = Cast<ALobbyGM>(UGameplayStatics::GetGameMode(GetWorld()));
	if (GM) //Server Only
	{
		GM->StartGame();
	}
}

void ULobbyWidgetBase::PressReadyButton()
{
	UGameplayStatics::PlaySound2D(this, MouseClickSound);

	// 클라이언트에서만 실행
	// 플레이어의 레디 상태에 따라서 버튼의 문구를 바꾼다.
	ALobbyPS* PS = GetOwningPlayer()->GetPlayerState<ALobbyPS>();
	if (PS && !GetOwningPlayer()->HasAuthority())
	{
		bool NewReadyState = !PS->bReady;
		PS->SetReady(NewReadyState);
		// 클라이언트 준비버튼 문구 바꾸기
		UpdateReadyButton(NewReadyState);
	}

	// 서버도 바꿔주기
	ALobbyPC* PC = GetOwningPlayer<ALobbyPC>();
	if (PC)
	{
		// Server RPC로 호출(클라이언트에서 호출되지만 서버에서 실행되게)
		PC->ServerChangeReadyButton(PS->bReady);
		PC->ChangeLobbyCharacterAnim(PS->bReady);
		PC->ServerShowClientReadyCanvas(!(PS->bReady));
	}

}

void ULobbyWidgetBase::PressExitButton()
{
	UGameplayStatics::PlaySound2D(this, MouseClickSound);

	ALobbyPC* PC = GetOwningPlayer<ALobbyPC>();
	if (PC)
	{
		if (PC->HasAuthority())
		{
			// 서버의 방 나가기 처리
			PC->ServerLeaveRoom();

			// 방지우기
			Protocol::C_DESTROY_ROOM Msg;
			Msg.set_roomnum(Cast<UTrapperGameInstance>(GetGameInstance())->GetRoomNumber());
			SendBufferRef SendBuffer = ServerPacketHandler::MakeSendBuffer(Msg);
			Cast<UTrapperGameInstance>(GetGameInstance())->SendPacket(SendBuffer);
			Cast<UTrapperGameInstance>(GetGameInstance())->SetRoomNumber(0);
			Cast<UTrapperGameInstance>(GetGameInstance())->SetRoomIp("");
		}
		else
		{
			// 클라이언트의 방 나가기 처리
			PC->ClientLeaveRoom();
			PC->ServerChangeReadyButton(false);
			PC->ServerShowClientReadyCanvas(false);
			Protocol::C_LEAVE_ROOM Msg;
			Msg.set_roomnum(Cast<UTrapperGameInstance>(GetGameInstance())->GetRoomNumber());
			SendBufferRef SendBuffer = ServerPacketHandler::MakeSendBuffer(Msg);
			Cast<UTrapperGameInstance>(GetGameInstance())->SendPacket(SendBuffer);
			Cast<UTrapperGameInstance>(GetGameInstance())->SetRoomNumber(0);
			Cast<UTrapperGameInstance>(GetGameInstance())->SetRoomIp("");
		}
	}
}

void ULobbyWidgetBase::UpdateReadyButton(bool bIsReady)
{
	// 2p가 준비버튼 눌렀을 때 이미 준비가 되어있는 상태인지 아닌지 판단후
	// widget 바꿔주기
	if (bIsReady)
	{
		// 준비중 상태에서 준비 버튼 눌렀을때
		FString ReadyTexturePath = "/Script/Engine.Texture2D'/Game/Blueprints/Lobby/Assets/W_Btn_2p_Ready.W_Btn_2p_Ready'";
		UTexture2D* ReadyTexture = Cast<UTexture2D>(StaticLoadObject(UTexture2D::StaticClass(), nullptr, *ReadyTexturePath));
		FString ReadyImpactTexturePath = "/Script/Engine.Texture2D'/Game/Blueprints/Lobby/Assets/W_Btn_2p_Ready_Impact.W_Btn_2p_Ready_Impact'";
		UTexture2D* ReadyImpactTexture = Cast<UTexture2D>(StaticLoadObject(UTexture2D::StaticClass(), nullptr, *ReadyImpactTexturePath));

		FButtonStyle  Style = ClientReadyButton->GetStyle();
		Style.Normal.SetResourceObject(ReadyTexture);
		Style.Hovered.SetResourceObject(ReadyImpactTexture);
		Style.Hovered.SetImageSize(Style.Normal.GetImageSize());
		Style.Pressed.SetResourceObject(ReadyImpactTexture);
		Style.Pressed.SetImageSize(Style.Normal.GetImageSize());
		ClientReadyButton->SetStyle(Style);

		UTexture2D* ClientReadyTexture = LoadObject<UTexture2D>(nullptr, TEXT("/Script/Engine.Texture2D'/Game/Blueprints/Lobby/Assets/W_Img_Ready.W_Img_Ready'"));
		ClientPreparingImage->SetBrushFromTexture(ClientReadyTexture);
	}
	else
	{
		// 준비상태에서 준비 버튼 눌렀을 때
		FString EnableReadyTexturePath = "/Script/Engine.Texture2D'/Game/Blueprints/Lobby/Assets/W_Btn_2p_preparing.W_Btn_2p_preparing'";
		UTexture2D* EnableReadyTexture = Cast<UTexture2D>(StaticLoadObject(UTexture2D::StaticClass(), nullptr, *EnableReadyTexturePath));

		FString EnableReadyImpactTexturePath = "/Script/Engine.Texture2D'/Game/Blueprints/Lobby/Assets/W_Btn_2p_preparing_Impact.W_Btn_2p_preparing_Impact'";
		UTexture2D* EnableReadyImpactTexture = Cast<UTexture2D>(StaticLoadObject(UTexture2D::StaticClass(), nullptr, *EnableReadyImpactTexturePath));

		FButtonStyle  Style = ClientReadyButton->GetStyle();
		Style.Normal.SetResourceObject(EnableReadyTexture);
		Style.Hovered.SetResourceObject(EnableReadyImpactTexture);
		Style.Hovered.SetImageSize(Style.Normal.GetImageSize());
		Style.Pressed.SetResourceObject(EnableReadyImpactTexture);
		Style.Pressed.SetImageSize(Style.Normal.GetImageSize());
		ClientReadyButton->SetStyle(Style);

		UTexture2D* PreparingTexture = LoadObject<UTexture2D>(nullptr, TEXT("/Script/Engine.Texture2D'/Game/Blueprints/Lobby/Assets/W_Img_preparing.W_Img_preparing'"));
		ClientPreparingImage->SetBrushFromTexture(PreparingTexture);
	}
}

void ULobbyWidgetBase::NativeConstruct()
{
	Super::NativeConstruct();

	GameStartPressed = false;
	// 채팅
	ChatInput = Cast<UEditableTextBox>(GetWidgetFromName(TEXT("ChatInput")));

	if (ChatInput)
	{
		ChatInput->OnTextCommitted.AddDynamic(this, &ULobbyWidgetBase::ProcessTextCommited);
	}

	ChatBox = Cast<UScrollBox>(GetWidgetFromName(TEXT("ChatBox")));

	// 텍스트
	PublicMessage = Cast<UTextBlock>(GetWidgetFromName(TEXT("PublicMessage")));
	RoomNumber = Cast<UTextBlock>(GetWidgetFromName(TEXT("RoomNumber")));

	UTrapperGameInstance* GameInstance = Cast<UTrapperGameInstance>(GetGameInstance());

	FString Temp = FString::Printf(TEXT("방 번호 [%d]"), GameInstance->GetRoomNumber());
	RoomNumber->SetText(FText::FromString(Temp));

	// 버튼
	StartGameButton = Cast<UButton>(GetWidgetFromName(TEXT("StartGameButton")));
	if (StartGameButton)
	{
		StartGameButton->SetVisibility(ESlateVisibility::HitTestInvisible);
		StartGameButton->OnClicked.AddDynamic(this, &ULobbyWidgetBase::PressStartGameButton);
		StartGameButton->OnHovered.AddDynamic(this, &ULobbyWidgetBase::OnButtonHovered);
	}

	ClientReadyButton = Cast<UButton>(GetWidgetFromName(TEXT("ClientReadyButton")));
	if (ClientReadyButton)
	{
		ClientReadyButton->OnClicked.AddDynamic(this, &ULobbyWidgetBase::PressReadyButton);
		ClientReadyButton->OnHovered.AddDynamic(this, &ULobbyWidgetBase::OnButtonHovered);
	}

	ExitLobby = Cast<UButton>(GetWidgetFromName(TEXT("ExitLobby")));
	if (ExitLobby)
	{
		ExitLobby->OnClicked.AddDynamic(this, &ULobbyWidgetBase::PressExitButton);
		ExitLobby->OnHovered.AddDynamic(this, &ULobbyWidgetBase::OnButtonHovered);
	}

	ShowInvitationButton = Cast<UButton>(GetWidgetFromName(TEXT("ShowInvitationFriendListButton")));
	if (ShowInvitationButton)
	{
		ShowInvitationButton->OnClicked.AddDynamic(this, &ULobbyWidgetBase::ShowInvitationFriendList);
		ShowInvitationButton->OnHovered.AddDynamic(this, &ULobbyWidgetBase::OnButtonHovered);
	}

	InvitationCanvasPanel = Cast<UCanvasPanel>(GetWidgetFromName(TEXT("InvitationCanvasPanel")));
	InvitationCanvasPanel->SetVisibility(ESlateVisibility::Collapsed);

	ALobbyPC* PC = GetOwningPlayer<ALobbyPC>();
	UTrapperGameInstance* GI = Cast<UTrapperGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	if (PC)
	{
		if (PC->HasAuthority())
		{
			// 서버면 클라이언트 준비버튼 안 보이게
			ClientReadyButton->SetVisibility(ESlateVisibility::Collapsed);

			HostNickname->SetText(FText::FromString(GI->GetPlayerNickname()));

		}
		else
		{
			ClientNickname->SetText(FText::FromString(GI->GetPlayerNickname()));

			// 클라이언트면 게임시작 버튼 안 보이게
			StartGameButton->SetVisibility(ESlateVisibility::Collapsed);
		}
	}

	ClientReadyCanvasPanel->SetVisibility(ESlateVisibility::Collapsed);

	// 카운트다운
	GameStartCanvasPanel->SetVisibility(ESlateVisibility::Collapsed);

	// 로딩
	LoadingCanvasPanel->SetVisibility(ESlateVisibility::Collapsed);

	SystemMenuUMG->SetLobbyProperty(ChatBoxBorder, ChatInput);
}

void ULobbyWidgetBase::ProcessTextCommited(const FText& Text, ETextCommit::Type CommitMethod)
{
	switch (CommitMethod)
	{
	case ETextCommit::OnEnter:
	{
		//Local PC -> Host PC
		ALobbyPC* PC = GetOwningPlayer<ALobbyPC>();
		if (PC)
		{
			UTrapperGameInstance* GI = Cast<UTrapperGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
			if (GI)
			{
				FString Temp = FString::Printf(TEXT("%s : %s"), *GI->GetPlayerNickname(), *Text.ToString());

				PC->C2S_SendMessage(FText::FromString(Temp));
				ChatInput->SetText(FText::FromString(TEXT("")));
			}
		}
	}
	break;
	case ETextCommit::OnCleared:
	{
		ChatInput->SetUserFocus(GetOwningPlayer());
	}
	break;
	}
}

void ULobbyWidgetBase::AddMessage(FText Message, const FLinearColor& Color)
{
	if (ChatBox)
	{
		UTextBlock* NewTextBlock = NewObject<UTextBlock>(ChatBox);
		if (NewTextBlock)
		{
			NewTextBlock->SetText(Message);

			FSlateFontInfo FontInfo = NewTextBlock->GetFont();
			FontInfo.Size = 18;
			NewTextBlock->SetFont(FontInfo);
			FSlateColor SlateColor(Color);
			NewTextBlock->SetColorAndOpacity(SlateColor);
			//NewTextBlock->Font.Size = 18;
			ChatBox->AddChild(NewTextBlock);
			ChatBox->ScrollToEnd();
		}
	}
}

void ULobbyWidgetBase::SetPublicMessage(int LeftTime)
{
	FString Temp = FString::Printf(TEXT("%d"), LeftTime);
	if (PublicMessage)
	{
		PublicMessage->SetText(FText::FromString(Temp));
	}
}

void ULobbyWidgetBase::ShowSecondPlayerNickname(bool Show, const FString& SecondNickname)
{
	// 2p 이미지 보이게 할지 말지
	if (Show && ClientNickname)
	{
		ClientNickname->SetVisibility(ESlateVisibility::HitTestInvisible);
		ClientNickname->SetText(FText::FromString(SecondNickname));
	}
	else
	{
		ClientNickname->SetVisibility(ESlateVisibility::Collapsed);

		// 우측 상단 준비중, 준비완료
		UTexture2D* PreparingTexture = LoadObject<UTexture2D>(nullptr, TEXT("/Script/Engine.Texture2D'/Game/Blueprints/Lobby/Assets/W_Img_preparing.W_Img_preparing'"));
		ClientPreparingImage->SetBrushFromTexture(PreparingTexture);
	}
}

void ULobbyWidgetBase::ShowFirstPlayerNickname(bool Show, const FString& FirstNickname)
{
	// 2p 이미지 보이게 할지 말지
	if (Show && HostNickname)
	{
		HostNickname->SetVisibility(ESlateVisibility::HitTestInvisible);
		HostNickname->SetText(FText::FromString(FirstNickname));
	}
	else
	{
		HostNickname->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void ULobbyWidgetBase::ActiveStartButton(bool Ready)
{
	// Ready버튼 누르면 게임시작 버튼 활성화/비활성화
	if (StartGameButton)
	{
		FString StartGameTexturePath;
		UTexture2D* StartGameTexture;

		if (Ready)
		{
			StartGameButton->SetVisibility(ESlateVisibility::Visible);
			StartGameTexturePath = "/Script/Engine.Texture2D'/Game/Blueprints/Lobby/Assets/W_Btn_1P_prepare.W_Btn_1P_prepare'";
			StartGameTexture = Cast<UTexture2D>(StaticLoadObject(UTexture2D::StaticClass(), nullptr, *StartGameTexturePath));
			FString StartImpactGameTexturePath = "/Script/Engine.Texture2D'/Game/Blueprints/Lobby/Assets/W_Btn_1P_prepare_Impact.W_Btn_1P_prepare_Impact'";
			UTexture2D* StartImpactGameTexture = Cast<UTexture2D>(StaticLoadObject(UTexture2D::StaticClass(), nullptr, *StartImpactGameTexturePath));

			FButtonStyle  Style = StartGameButton->GetStyle();
			Style.Normal.SetResourceObject(StartGameTexture);
			Style.Hovered.SetResourceObject(StartImpactGameTexture);
			Style.Hovered.SetImageSize(Style.Normal.GetImageSize());
			Style.Pressed.SetResourceObject(StartImpactGameTexture);
			Style.Pressed.SetImageSize(Style.Normal.GetImageSize());
			StartGameButton->SetStyle(Style);

			// 우측 상단 준비중, 준비 완료 이미지 교체
			UTexture2D* ReadyTexture = LoadObject<UTexture2D>(nullptr, TEXT("/Script/Engine.Texture2D'/Game/Blueprints/Lobby/Assets/W_Img_Ready.W_Img_Ready'"));
			ClientPreparingImage->SetBrushFromTexture(ReadyTexture);
		}
		else
		{
			StartGameButton->SetVisibility(ESlateVisibility::HitTestInvisible);
			StartGameTexturePath = "/Script/Engine.Texture2D'/Game/Blueprints/Lobby/Assets/W_Btn_1P_Waiting.W_Btn_1P_Waiting'";
			StartGameTexture = Cast<UTexture2D>(StaticLoadObject(UTexture2D::StaticClass(), nullptr, *StartGameTexturePath));

			FButtonStyle  Style = StartGameButton->GetStyle();
			Style.Normal.SetResourceObject(StartGameTexture);
			Style.Hovered.SetResourceObject(StartGameTexture);
			Style.Hovered.SetImageSize(Style.Normal.GetImageSize());
			Style.Pressed.SetResourceObject(StartGameTexture);
			Style.Pressed.SetImageSize(Style.Normal.GetImageSize());
			StartGameButton->SetStyle(Style);

			UTexture2D* PreparingTexture = LoadObject<UTexture2D>(nullptr, TEXT("/Script/Engine.Texture2D'/Game/Blueprints/Lobby/Assets/W_Img_preparing.W_Img_preparing'"));
			ClientPreparingImage->SetBrushFromTexture(PreparingTexture);
		}
	}
}

void ULobbyWidgetBase::DuringCountDown()
{
	UGameplayStatics::PlaySound2D(this, CountDownSound);
	// 카운트다운 시작할 때는 아무 버튼도 안 눌리게 하기
	if (ClientReadyButton)
	{
		ClientReadyButton->SetVisibility(ESlateVisibility::HitTestInvisible);
		ClientReadyButton->SetIsEnabled(false);
	}
	if (ExitLobby)
	{
		ExitLobby->SetVisibility(ESlateVisibility::HitTestInvisible);
		ExitLobby->SetIsEnabled(false);
	}
}

void ULobbyWidgetBase::SetTimerVisibility(ESlateVisibility NewVisibility)
{
	if (PublicMessage)
	{
		GameStartCanvasPanel->SetVisibility(NewVisibility);
	}
}

void ULobbyWidgetBase::ShowCountDown()
{
	SetTimerVisibility(ESlateVisibility::Visible);
}

void ULobbyWidgetBase::ShowInvitationFriendButton(bool Show)
{
	ShowInvitationButton->SetVisibility(Show ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
}

void ULobbyWidgetBase::ShowInvitationFriendList(bool Show)
{
	InvitationCanvasPanel->SetVisibility(Show ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
}

void ULobbyWidgetBase::ShowInvitationFriendList()
{
	UGameplayStatics::PlaySound2D(this, SlideSound);

	if (InvitationCanvasPanel->GetVisibility() == ESlateVisibility::Visible)
	{
		PlayAnimation(InviteFriendClose);
		InvitationCanvasPanel->SetVisibility(ESlateVisibility::HitTestInvisible);
		return;
	}

	PlayAnimation(InviteFriendOpen);
	// InvitationCanvasPanel 표시
	InvitationCanvasPanel->SetVisibility(ESlateVisibility::Visible);

	// 기존 친구 리스트 초기화
	FriendListVerticalBox->ClearChildren();

	TMap<FString, bool> FriendsMap = Cast<UTrapperGameInstance>(GetGameInstance())->GetFriends();

	if (!FriendListVerticalBox || !InvitationFriendListWidgetClass)
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


void ULobbyWidgetBase::AddFriendToList(const FString& FriendKey, bool bIsOnline)
{
	UInvitationFriendListWidgetBase* InvitationFriend = CreateWidget<UInvitationFriendListWidgetBase>(this, InvitationFriendListWidgetClass);
	if (InvitationFriend)
	{
		UImage* InviteActiveFriendImage = Cast<UImage>(InvitationFriend->GetWidgetFromName(TEXT("InviteActiveFriendImage")));
		UTextBlock* FriendNameTextBlock = Cast<UTextBlock>(InvitationFriend->GetWidgetFromName(TEXT("FriendNameTextBlock")));
		UTextBlock* FriendStatusTextBlock = Cast<UTextBlock>(InvitationFriend->GetWidgetFromName(TEXT("FriendStatusTextBlock")));

		UTexture2D* OnlineImageTexture = LoadObject<UTexture2D>(nullptr, TEXT("/Script/Engine.Texture2D'/Game/Blueprints/Lobby/Assets/WF_Btn_FriendTag.WF_Btn_FriendTag'"));

		if (FriendNameTextBlock && FriendStatusTextBlock)
		{
			FriendNameTextBlock->SetText(FText::FromString(FriendKey));
			FriendStatusTextBlock->SetText(FText::FromString(bIsOnline ? TEXT("온라인") : TEXT("오프라인")));
			InvitationFriend->SetNickname(FriendKey);
			if (bIsOnline)
			{
				InviteActiveFriendImage->SetBrushFromTexture(OnlineImageTexture);
				InvitationFriend->SetInviteButtonVisible(true);
			}
			else
			{
				InvitationFriend->SetInviteButtonVisible(false);
			}
		}

		FriendListVerticalBox->AddChildToVerticalBox(InvitationFriend);
	}
}

void ULobbyWidgetBase::OnBack()
{
	if (SystemMenuUMG->IsVisible())
	{
		/*ChatBox->SetVisibility(ESlateVisibility::Visible);
		ChatInput->SetVisibility(ESlateVisibility::Visible);*/

		SystemMenuUMG->OnBack();
	}
	else
	{
		ChatBoxBorder->SetVisibility(ESlateVisibility::Collapsed);
		ChatInput->SetVisibility(ESlateVisibility::Collapsed);

		SystemMenuUMG->OnMouseClickSound();
		SystemMenuUMG->SetVisibility(ESlateVisibility::Visible);
	}
}

void ULobbyWidgetBase::ShowLoading()
{
	if (LoadingCanvasPanel)
	{
		LoadingCanvasPanel->SetVisibility(ESlateVisibility::Visible);
	}

	int32 RandomBackground = FMath::RandRange(0, 17);

	FString AssetPath = FString::Printf(TEXT("/Game/Blueprints/Lobby/Assets/HighresScreenshot%05d.HighresScreenshot%05d"), RandomBackground, RandomBackground);
	UTexture2D* BackgroundTexture = LoadObject<UTexture2D>(nullptr, *AssetPath);

	if(BackgroundTexture)
	{
		BackgroundImage->SetBrushFromTexture(BackgroundTexture);
	}

	int32 RandomTipsText = FMath::RandRange(0, 7);

	FString Temp = "";
	switch (RandomTipsText)
	{
	case 0:
		Temp = FString::Printf(TEXT("협동 함정을 신성화살로 함께 맞춰 사용하면 높은 피해를 줄 수 있습니다."));
		TipsTextBlock->SetText(FText::FromString(Temp));
		break;
	case 1:
		Temp = FString::Printf(TEXT("전선이 밀리지 않도록 최전방에서 전투하면 더욱 쉽게 플레이할 수 있습니다."));
		TipsTextBlock->SetText(FText::FromString(Temp));
		break;
	case 2:
		Temp = FString::Printf(TEXT("사용한 함정 잔해에 E키를 사용해 재설치할 수 있습니다."));
		TipsTextBlock->SetText(FText::FromString(Temp));
		break;
	case 3:
		Temp = FString::Printf(TEXT("Ctrl키를 이용해 상대방에게 정보를 알려줄 수 있습니다."));
		TipsTextBlock->SetText(FText::FromString(Temp));
		break;
	case 4:
		Temp = FString::Printf(TEXT("몬스터는 포탑 주변으로 몰려와 포탑을 파괴합니다."));
		TipsTextBlock->SetText(FText::FromString(Temp));
		break;
	case 5:
		Temp = FString::Printf(TEXT("포탑 근처에 2명의 플레이어가 함께 있을 경우 포탑이 강화됩니다."));
		TipsTextBlock->SetText(FText::FromString(Temp));
		break;
	case 6:
		Temp = FString::Printf(TEXT("2명의 플레이어가 마법석에 신성 화살을 맞출 경우 몬스터를 끌어들입니다."));
		TipsTextBlock->SetText(FText::FromString(Temp));
		break;
	case 7:
		Temp = FString::Printf(TEXT("함정은 포탑과 마법석을 활용할 수 있는 효율적인 위치에 설치하는 것이 좋습니다."));
		TipsTextBlock->SetText(FText::FromString(Temp));
		break;
	default:
		break;
	}

}

void ULobbyWidgetBase::ShowClientReadyCanvas(bool Show)
{
	if (ClientReadyCanvasPanel)
	{
		ClientReadyCanvasPanel->SetVisibility(Show ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	}
}