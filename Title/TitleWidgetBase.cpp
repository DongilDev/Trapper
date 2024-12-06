// Fill out your copyright notice in the Description page of Project Settings.


#include "Title/TitleWidgetBase.h"
#include "Title/CreditWidgetBase.h"
#include "Components/Button.h"
#include "Components/EditableTextBox.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "Components/TextBlock.h"
#include "Components/CanvasPanel.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Animation/WidgetAnimation.h"
#include "Kismet/GameplayStatics.h"
#include "TitlePC.h"
#include "Protocol.pb.h"
#include "TrapperGameInstance.h"
#include "ServerPacketHandler.h"
#include "InvitationWidgetBase.h"
#include "RoomListWidgetBase.h"
#include "FriendWidgetBase.h"
#include "Widget/Options/OptionsUMG.h"

#include "MediaPlayer.h"
#include "MediaSource.h"


void UTitleWidgetBase::NativeConstruct()
{
	Super::NativeConstruct();

	// 여기서부터
	UMediaPlayer* TitleMediaPlayer = LoadObject<UMediaPlayer>(nullptr, TEXT("/Script/MediaAssets.MediaPlayer'/Game/Blueprints/Title/Assets/MainTitlePlayer.MainTitlePlayer'"));

	if (TitleMediaPlayer)
	{
		// 미디어 소스 로드
		UMediaSource* MediaSource = LoadObject<UMediaSource>(nullptr, TEXT("/Script/MediaAssets.FileMediaSource'/Game/Movies/MainTitle.MainTitle'"));
		if (MediaSource && TitleMediaPlayer->OpenSource(MediaSource))
		{
			UE_LOG(LogTemp, Warning, TEXT("Title Media Player"));
			TitleMediaPlayer->Play(); // 재생 시작
			TitleMediaPlayer->SetLooping(true);
		}
	}
	// 여기까지
	CreateRoomButton = Cast<UButton>(GetWidgetFromName(TEXT("CreateRoomButton")));
	JoinRoomButton = Cast<UButton>(GetWidgetFromName(TEXT("JoinRoomButton")));
	FriendButton = Cast<UButton>(GetWidgetFromName(TEXT("FriendButton")));
	LogoutButton = Cast<UButton>(GetWidgetFromName(TEXT("LogoutButton")));

	if (CreateRoomButton)
	{
		CreateRoomButton->OnClicked.AddDynamic(this, &UTitleWidgetBase::OnCreateRoomButton);
	}

	if (JoinRoomButton)
	{
		JoinRoomButton->OnClicked.AddDynamic(this, &UTitleWidgetBase::OnJoinRoomButton);
	}

	if (FriendButton)
	{
		FriendButton->OnClicked.AddDynamic(this, &UTitleWidgetBase::OnFriendButton);
	}
	
	if (LogoutButton)
	{
		LogoutButton->OnClicked.AddDynamic(this, &UTitleWidgetBase::OnLogoutButton);
	}
	
	if (CreditButton)
	{
		CreditButton->OnClicked.AddDynamic(this, &UTitleWidgetBase::OnCreditButton);
	}

	if (FriendWidget)
	{
		FriendWidget->SetVisibility(ESlateVisibility::Collapsed);
	}

	/*if (BookButton)
	{
		BookButton->OnClicked.AddDynamic(this, &UTitleWidgetBase::OnBookButton);
	}

	if (ChallengeButton)
	{
		ChallengeButton->OnClicked.AddDynamic(this, &UTitleWidgetBase::OnChallengeButton);
	}*/

	// Option
	if (OptionButton)
	{
		OptionButton->OnClicked.AddDynamic(this, &UTitleWidgetBase::OnOptionButton);
	}
	if (NotifaiButton)
	{
		NotifaiButton->OnClicked.AddDynamic(this, &UTitleWidgetBase::OnClickNotifaiButton);
		NotifaiButton->OnHovered.AddDynamic(this, &UTitleWidgetBase::OnHoveredNotifaiButton);
		NotifaiButton->OnUnhovered.AddDynamic(this, &UTitleWidgetBase::OnUnhoveredNotifaiButton);
	}
	OptionsUMG->SetTrapperImage(TrapperImage);
	OptionsUMG->SetTitleGameExitButton(ExitButton);
	OptionsUMG->SetTitleLogoutButton(LogoutButton);
	OptionsUMG->SetTitleCreditButton(CreditButton);
	OptionsUMG->SetTitleCanvas(OptionCanvasPanel);

	OptionCanvasPanel->SetVisibility(ESlateVisibility::Collapsed);

	// InputRoomNumber
	QuitInputRoomNumberButton = Cast<UButton>(GetWidgetFromName(TEXT("QuitInputRoomNumberButton")));
	InputRoomNumber = Cast<UEditableTextBox>(GetWidgetFromName(TEXT("InputRoomNumber")));
	InputRoomNumberCanvas = Cast<UCanvasPanel>(GetWidgetFromName(TEXT("InputRoomNumberCanvas")));

	if (InputRoomNumber)
	{
		InputRoomNumber->OnTextChanged.AddDynamic(this, &UTitleWidgetBase::OnRoomNumberTextChanged);
	}

	if (QuitInputRoomNumberButton)
	{
		QuitInputRoomNumberButton->OnClicked.AddDynamic(this, &UTitleWidgetBase::OnQuitInputRoomNumberButton);
	}

	InputRoomNumberCanvas->SetVisibility(ESlateVisibility::Collapsed);
	InputRoomNumberCanvas->GetVisibility();

	// 버튼과 텍스트 블록 배열에 추가
	ButtonArray.Add(CreateRoomButton);
	ButtonArray.Add(JoinRoomButton);
	ButtonArray.Add(FriendButton);
	/*ButtonArray.Add(BookButton);
	ButtonArray.Add(ChallengeButton);*/
	ButtonArray.Add(OptionButton);

	TextBlockArray.Add(CreateRoomTextBlock);
	TextBlockArray.Add(JoinRoomTextBlock);
	TextBlockArray.Add(FriendTextBlock);
	/*TextBlockArray.Add(BookTextBlock);
	TextBlockArray.Add(ChallengeTextBlock);*/
	TextBlockArray.Add(OptionTextBlock);

	for (UButton* Button : ButtonArray)
	{
		if (Button)
		{
			Button->OnHovered.AddDynamic(this, &UTitleWidgetBase::OnButtonHovered);
			Button->OnUnhovered.AddDynamic(this, &UTitleWidgetBase::OnButtonUnhovered);
		}
	}

	ClickedButton = nullptr;

	UTrapperGameInstance* TrapperGameInstance = Cast<UTrapperGameInstance>(GetGameInstance());
	if (TrapperGameInstance)
	{
		if (TrapperGameInstance->IsFriendExitLobbyOrGame())
		{
			NotifaiCanvas->SetVisibility(ESlateVisibility::Visible);
			AcceptImage->SetVisibility(ESlateVisibility::Collapsed);
			NotifaiCanvas->SetVisibility(ESlateVisibility::Visible);
			TrapperGameInstance->SetFriendExitLobbyOrGame(false);

			FString ExitFriendName = TrapperGameInstance->GetExitFriendName();
			FString TextString = FString::Printf(TEXT("%s 님이 게임을 나가셨습니다."), *ExitFriendName);
			FriendText->SetText(FText::FromString(TextString));
		}
	}
}

void UTitleWidgetBase::OnBack()
{
	UGameplayStatics::PlaySound2D(this, MouseClickSound);

	if (IsInputRoomNumberCanvasVisible())
	{
		OnQuitInputRoomNumberButton();
	}
	ShowFriendPopUp(false);


	if (OptionCanvasPanel->GetVisibility() == ESlateVisibility::SelfHitTestInvisible)
	{
		OptionCanvasPanel->SetVisibility(ESlateVisibility::Collapsed);
		TrapperImage->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}

	if (CreditWidget->GetVisibility() == ESlateVisibility::Visible)
	{
		CreditWidget->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UTitleWidgetBase::OnButtonHovered()
{
	UGameplayStatics::PlaySound2D(this, MouseHoverSound);

	for (int32 Index = 0; Index < ButtonArray.Num(); Index++)
	{
		UButton* CurrentButton = ButtonArray[Index];

		// 현재 호버된 버튼을 확인
		if (CurrentButton->IsHovered())
		{
			MenuButtonClicked(CurrentButton);
			return;
		}
	}
}

void UTitleWidgetBase::OnButtonUnhovered()
{
	if (ClickedButton != nullptr)
	{
		MenuButtonClicked(ClickedButton);
		return;
	}

	for (int32 Index = 0; Index < ButtonArray.Num(); Index++)
	{
		UButton* CurrentButton = ButtonArray[Index];

		// 텍스트 블록 알파값 설정
		UTextBlock* CorrespondingTextBlock = TextBlockArray[Index];
		if (CorrespondingTextBlock)
		{
			FLinearColor NewColor = FLinearColor(1, 1, 1, 0.5f); // 알파값 적용
			CorrespondingTextBlock->SetColorAndOpacity(NewColor);
		}
	}
}

void UTitleWidgetBase::OnCreateRoomButton()
{
	UGameplayStatics::PlaySound2D(this, MouseClickSound);

	// 방 생성을 위해 IP주소를 서버에 보내준다.
	FString LocalIp = Cast<UTrapperGameInstance>(GetGameInstance())->GetLocalIPAddress();
	Protocol::C_CREATE_ROOM Msg;
	Msg.set_ip(TCHAR_TO_ANSI(*LocalIp));
	SendBufferRef SendBuffer = ServerPacketHandler::MakeSendBuffer(Msg);
	Cast<UTrapperGameInstance>(GetGameInstance())->SendPacket(SendBuffer);

	if (OptionCanvasPanel->GetVisibility() == ESlateVisibility::SelfHitTestInvisible)
	{
		TrapperImage->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		OptionCanvasPanel->SetVisibility(ESlateVisibility::Collapsed);
	}
	if (InputRoomNumberCanvas->GetVisibility() == ESlateVisibility::SelfHitTestInvisible)
	{
		InputRoomNumberCanvas->SetVisibility(ESlateVisibility::Collapsed);
	}
	if (FriendWidget->GetVisibility() == ESlateVisibility::SelfHitTestInvisible)
	{
		FriendWidget->SetVisibility(ESlateVisibility::Collapsed);
		FriendWidget->OnQuitFriendButton();
	}

	// 클릭한 버튼의 텍스트 알파값 바꾸기
	MenuButtonClicked(CreateRoomButton);

	ClickedButton = CreateRoomButton;
}

void UTitleWidgetBase::OnJoinRoomButton()
{
	UGameplayStatics::PlaySound2D(this, MouseClickSound);

	// 방번호 입력 팝업 띄우기
	ShowInputRoomNumberPopUp(true);

	if (OptionCanvasPanel->GetVisibility() == ESlateVisibility::SelfHitTestInvisible)
	{
		TrapperImage->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		OptionCanvasPanel->SetVisibility(ESlateVisibility::Collapsed);
	}
	if (FriendWidget->GetVisibility() == ESlateVisibility::SelfHitTestInvisible)
	{
		FriendWidget->SetVisibility(ESlateVisibility::Collapsed);
		FriendWidget->OnQuitFriendButton();
	}


	TrapperImage->SetVisibility(ESlateVisibility::Collapsed);
	ExitButton->SetVisibility(ESlateVisibility::Collapsed);
	LogoutButton->SetVisibility(ESlateVisibility::Collapsed);
	CreditButton->SetVisibility(ESlateVisibility::Collapsed);

	// 클릭한 버튼의 텍스트 알파값 바꾸기
	MenuButtonClicked(JoinRoomButton);

	ClickedButton = JoinRoomButton;
}

void UTitleWidgetBase::OnFriendButton()
{
	UGameplayStatics::PlaySound2D(this, MouseClickSound);

	// 친구 팝업 띄우기
	/*ATitlePC* PC = GetOwningPlayer<ATitlePC>();
	if (PC)
	{
		PC->ShowFriendPopup(true);
	}*/

	if (OptionCanvasPanel->GetVisibility() == ESlateVisibility::SelfHitTestInvisible)
	{
		TrapperImage->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		OptionCanvasPanel->SetVisibility(ESlateVisibility::Collapsed);
	}
	if (InputRoomNumberCanvas->GetVisibility() == ESlateVisibility::SelfHitTestInvisible)
	{
		InputRoomNumberCanvas->SetVisibility(ESlateVisibility::Collapsed);
	}

	if (FriendWidget->GetVisibility() == ESlateVisibility::SelfHitTestInvisible)
	{
	}
	else
	{
		FriendWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		FriendWidget->OnFriendListButton();
	}


	TrapperImage->SetVisibility(ESlateVisibility::Collapsed);
	ExitButton->SetVisibility(ESlateVisibility::Collapsed);
	LogoutButton->SetVisibility(ESlateVisibility::Collapsed);
	CreditButton->SetVisibility(ESlateVisibility::Collapsed);

	// 클릭한 버튼의 텍스트 알파값 바꾸기
	MenuButtonClicked(FriendButton);

	ClickedButton = FriendButton;
}

void UTitleWidgetBase::OnBookButton()
{
	if (OptionCanvasPanel->GetVisibility() == ESlateVisibility::SelfHitTestInvisible)
	{
		TrapperImage->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		OptionCanvasPanel->SetVisibility(ESlateVisibility::Collapsed);
	}
	if (InputRoomNumberCanvas->GetVisibility() == ESlateVisibility::SelfHitTestInvisible)
	{
		InputRoomNumberCanvas->SetVisibility(ESlateVisibility::Collapsed);
	}
	if (FriendWidget->GetVisibility() == ESlateVisibility::Visible)
	{
		FriendWidget->SetVisibility(ESlateVisibility::Collapsed);
		FriendWidget->OnQuitFriendButton();
	}

	// 클릭한 버튼의 텍스트 알파값 바꾸기
	//MenuButtonClicked(BookButton);

	//ClickedButton = BookButton;
}

void UTitleWidgetBase::OnChallengeButton()
{
	if (OptionCanvasPanel->GetVisibility() == ESlateVisibility::SelfHitTestInvisible)
	{
		TrapperImage->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		OptionCanvasPanel->SetVisibility(ESlateVisibility::Collapsed);
	}
	if (InputRoomNumberCanvas->GetVisibility() == ESlateVisibility::SelfHitTestInvisible)
	{
		InputRoomNumberCanvas->SetVisibility(ESlateVisibility::Collapsed);
	}
	if (FriendWidget->GetVisibility() == ESlateVisibility::Visible)
	{
		FriendWidget->SetVisibility(ESlateVisibility::Collapsed);
		FriendWidget->OnQuitFriendButton();
	}

	// 클릭한 버튼의 텍스트 알파값 바꾸기
	//MenuButtonClicked(ChallengeButton);

	//ClickedButton = ChallengeButton;
}

void UTitleWidgetBase::OnCreditButton()
{
	UGameplayStatics::PlaySound2D(this, MouseClickSound);

	UE_LOG(LogTemp, Log, TEXT("Credit"));

	CreditWidget->SetVisibility(ESlateVisibility::Visible);
}

void UTitleWidgetBase::CreditFinished()
{
	CreditWidget->SetVisibility(ESlateVisibility::Collapsed);
}

void UTitleWidgetBase::OnLogoutButton()
{
	UGameplayStatics::PlaySound2D(this, MouseClickSound);

	UE_LOG(LogTemp, Log, TEXT("LogOut"));
	if (!Cast<UTrapperGameInstance>(GetGameInstance())->GetLoginVer())
	{
		return;
	}

	Protocol::C_LOGOUT Msg;
	FString FId = Cast<UTrapperGameInstance>(GetGameInstance())->GetPlayerID();
	Msg.set_playerid(std::string(TCHAR_TO_UTF8(*FId)));
	SendBufferRef SendBuffer = ServerPacketHandler::MakeSendBuffer(Msg);
	Cast<UTrapperGameInstance>(GetGameInstance())->SendPacket(SendBuffer);
}

void UTitleWidgetBase::OnRoomNumberTextChanged(const FText& Text)
{
	RoomListVerticalBox->ClearChildren();

	// 방번호 입력될 때마다 방목록 갱신시키기
	FString StringRoomNumber = Text.ToString();
	UE_LOG(LogTemp, Log, TEXT("Room number input: %s"), *StringRoomNumber);
	int32 RoomNumber = FCString::Atoi(*(StringRoomNumber));
	Protocol::C_SHOW_ROOM Msg;
	Msg.set_roomnum(RoomNumber);
	SendBufferRef SendBuffer = ServerPacketHandler::MakeSendBuffer(Msg);
	Cast<UTrapperGameInstance>(GetGameInstance())->SendPacket(SendBuffer);
}

void UTitleWidgetBase::AddInvitation(int32 RoomNumber, FString Nickname)
{
	if (!InvitationVerticalBox || !UInvitationWidgetClass)
	{
		return;
	}

	UInvitationWidgetBase* Invitation = CreateWidget<UInvitationWidgetBase>(this, UInvitationWidgetClass);
	Invitation->SetRoomNumber(RoomNumber);
	Invitation->SetNickname(Nickname);
	FString Temp = FString::Printf(TEXT("%s 님이 초대를 보냈습니다."), *Nickname);
	Invitation->SetInvitationText(Temp);

	UVerticalBoxSlot* VerticalBoxSlot = InvitationVerticalBox->AddChildToVerticalBox(Invitation);

	if (VerticalBoxSlot)
	{
		VerticalBoxSlot->SetPadding(FMargin(0.0f, 0.0f, 0.0f, 30.0f));
	}
}

void UTitleWidgetBase::OnQuitInputRoomNumberButton()
{

	// 뒤로 버튼 눌렀을 때 팝업 사라지게 하고 입력창도 초기화한다.
	InputRoomNumber->SetText(FText::FromString(""));
	RoomListVerticalBox->ClearChildren();

	ATitlePC* PC = GetOwningPlayer<ATitlePC>();
	if (PC)
	{
		PC->ShowInputRoomNumberPopup(false);
	}
	ExitButton->SetVisibility(ESlateVisibility::Visible);
	LogoutButton->SetVisibility(ESlateVisibility::Visible);
	CreditButton->SetVisibility(ESlateVisibility::Visible);
	TrapperImage->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	SetClickedButton();
}

void UTitleWidgetBase::ShowInputRoomNumberPopUp(bool Show)
{
	if (Show)
	{
		InputRoomNumberCanvas->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		ExitButton->SetVisibility(ESlateVisibility::Collapsed);
		LogoutButton->SetVisibility(ESlateVisibility::Collapsed);
		CreditButton->SetVisibility(ESlateVisibility::Collapsed);
		TrapperImage->SetVisibility(ESlateVisibility::Collapsed);
	}
	else
	{
		InputRoomNumberCanvas->SetVisibility(ESlateVisibility::Collapsed);
		ExitButton->SetVisibility(ESlateVisibility::Visible);
		LogoutButton->SetVisibility(ESlateVisibility::Visible);
		CreditButton->SetVisibility(ESlateVisibility::Visible);
		TrapperImage->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
}

void UTitleWidgetBase::ShowFriendPopUp(bool Show)
{
	FriendWidget->SetVisibility(Show ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Collapsed);
	ExitButton->SetVisibility(Show ? ESlateVisibility::Collapsed : ESlateVisibility::Visible);
	LogoutButton->SetVisibility(Show ? ESlateVisibility::Collapsed : ESlateVisibility::Visible);
	CreditButton->SetVisibility(Show ? ESlateVisibility::Collapsed : ESlateVisibility::Visible);
	TrapperImage->SetVisibility(Show ? ESlateVisibility::Collapsed : ESlateVisibility::SelfHitTestInvisible);
}

void UTitleWidgetBase::AddRoomList(int32 RoomNumber, FString Nickname)
{
	if (!RoomListVerticalBox || !URoomListWidgetClass)
	{
		return;
	}

	URoomListWidgetBase* RoomList = CreateWidget<URoomListWidgetBase>(this, URoomListWidgetClass);
	RoomList->SetRoomNumText(RoomNumber);
	RoomList->SetHostNicknameText(Nickname);

	UVerticalBoxSlot* VerticalBoxSlot = RoomListVerticalBox->AddChildToVerticalBox(RoomList);

	if (VerticalBoxSlot)
	{
		VerticalBoxSlot->SetPadding(FMargin(0.0f, 0.0f, 0.0f, 30.0f));
	}

	RoomListWidgets.Add(RoomNumber, RoomList);
}

void UTitleWidgetBase::ShowRoomNotExist(bool Show)
{
	if (RommNotExistImage)
	{
		RommNotExistImage->SetVisibility(Show ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Collapsed);
	}
}

bool UTitleWidgetBase::IsInputRoomNumberCanvasVisible() const
{
	return InputRoomNumberCanvas->GetVisibility() == ESlateVisibility::SelfHitTestInvisible;
}

void UTitleWidgetBase::OnOptionButton()
{

	UGameplayStatics::PlaySound2D(this, MouseClickSound);

	if (IsInputRoomNumberCanvasVisible())
	{
		OnQuitInputRoomNumberButton();
	}


	if (FriendWidget->GetVisibility() == ESlateVisibility::SelfHitTestInvisible)
	{
		FriendWidget->SetVisibility(ESlateVisibility::Collapsed);
		FriendWidget->OnQuitFriendButton();
	}

	if (OptionCanvasPanel->GetVisibility() == ESlateVisibility::SelfHitTestInvisible)
	{
		OptionCanvasPanel->SetVisibility(ESlateVisibility::Collapsed);
		TrapperImage->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		OptionCanvasPanel->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		TrapperImage->SetVisibility(ESlateVisibility::Collapsed);
	}

	ExitButton->SetVisibility(ESlateVisibility::Collapsed);
	LogoutButton->SetVisibility(ESlateVisibility::Collapsed);
	CreditButton->SetVisibility(ESlateVisibility::Collapsed);
	// 클릭한 버튼의 텍스트 알파값 바꾸기
	MenuButtonClicked(OptionButton);
}

void UTitleWidgetBase::OnClickNotifaiButton()
{
	UGameplayStatics::PlaySound2D(this, MouseClickSound);

	NotifaiCanvas->SetVisibility(ESlateVisibility::Collapsed);
	RejectImage->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	AcceptImage->SetVisibility(ESlateVisibility::Collapsed);
	ImpactImage->SetVisibility(ESlateVisibility::Collapsed);
}

void UTitleWidgetBase::OnHoveredNotifaiButton()
{
	UGameplayStatics::PlaySound2D(this, MouseHoverSound);

	ImpactImage->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	RejectImage->SetVisibility(ESlateVisibility::Collapsed);
	AcceptImage->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
}

void UTitleWidgetBase::OnUnhoveredNotifaiButton()
{
	ImpactImage->SetVisibility(ESlateVisibility::Collapsed);
	RejectImage->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	AcceptImage->SetVisibility(ESlateVisibility::Collapsed);
}

void UTitleWidgetBase::ShowRoomError(int32 RoomNum)
{
	URoomListWidgetBase** FoundWidget = RoomListWidgets.Find(RoomNum);
	URoomListWidgetBase* RoomListWidget = *FoundWidget;
	RoomListWidget->ShowFullRoomErrorText(true);
}

void UTitleWidgetBase::MenuButtonClicked(UButton* Button)
{
	// 클릭한 버튼의 인덱스 찾기
	int32 ClickedButtonIndex = ButtonArray.Find(Button);

	// 버튼 배열 순회하여 클릭한 버튼 주변의 알파값을 거리별로 조정
	for (int32 Index = 0; Index < ButtonArray.Num(); ++Index)
	{
		UButton* CurrentButton = ButtonArray[Index];

		if (CurrentButton) // Null 체크
		{
			// 거리 계산
			int32 Distance = FMath::Abs(ClickedButtonIndex - Index);

			// 거리별 알파값 설정 (멀수록 알파값 낮아짐)
			float AlphaValue = FMath::Clamp(1.0f - (Distance * 0.4f), 0.1f, 1.0f);

			// 텍스트 블록 알파값 설정
			UTextBlock* CorrespondingTextBlock = TextBlockArray[Index];
			if (CorrespondingTextBlock)
			{
				FLinearColor NewColor = FLinearColor(1, 1, 1, AlphaValue); // 알파값 적용
				CorrespondingTextBlock->SetColorAndOpacity(NewColor);
			}
		}
	}
}

void UTitleWidgetBase::ShowAddFriend(TArray<int32> SuccessArr, TArray<FString> NicknameArr)
{
	FriendWidget->ShowAddFriend(SuccessArr, NicknameArr);
}

void UTitleWidgetBase::SetClickedButton()
{
	ClickedButton = nullptr;
	OnButtonUnhovered();
}

