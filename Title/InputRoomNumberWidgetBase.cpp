// Fill out your copyright notice in the Description page of Project Settings.


#include "InputRoomNumberWidgetBase.h"
#include "Components/Button.h"
#include "Components/EditableTextBox.h"
#include "Components/CanvasPanel.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "TitlePC.h"
#include "Protocol.pb.h"
#include "TrapperGameInstance.h"
#include "ServerPacketHandler.h"

void UInputRoomNumberWidgetBase::NativeConstruct()
{
	EnterRoomButton = Cast<UButton>(GetWidgetFromName(TEXT("EnterRoomButton")));
	QuitInputRoomNumberButton = Cast<UButton>(GetWidgetFromName(TEXT("QuitInputRoomNumberButton")));
	InputRoomNumber = Cast<UEditableTextBox>(GetWidgetFromName(TEXT("InputRoomNumber")));
	RoomErrorPopUpCanvasPanel = Cast<UCanvasPanel>(GetWidgetFromName(TEXT("RoomErrorPopUpCanvasPanel")));
	OkButton = Cast<UButton>(GetWidgetFromName(TEXT("OkButton")));
	RoomErrorMessage = Cast<UTextBlock>(GetWidgetFromName(TEXT("RoomErrorMessage")));

	if (EnterRoomButton)
	{
		EnterRoomButton->OnClicked.AddDynamic(this, &UInputRoomNumberWidgetBase::OnEnterRoomButton);
	}

	if (QuitInputRoomNumberButton)
	{
		QuitInputRoomNumberButton->OnClicked.AddDynamic(this, &UInputRoomNumberWidgetBase::OnQuitInputRoomNumberButton);
	}

	if (OkButton)
	{
		OkButton->OnClicked.AddDynamic(this, &UInputRoomNumberWidgetBase::HideRoomErrorPop);
	}

	RoomErrorPopUpCanvasPanel->SetVisibility(ESlateVisibility::Collapsed);
}

void UInputRoomNumberWidgetBase::OnEnterRoomButton()
{
	// 방 참가를 위해 입력된 방번호를 서버에 보낸다.
	ATitlePC* PC = GetOwningPlayer<ATitlePC>();
	if (PC)
	{
		int32 RoomNumber = FCString::Atoi(*(InputRoomNumber->GetText().ToString()));
		Protocol::C_JOIN_ROOM Msg;
		Msg.set_roomnum(RoomNumber);
		SendBufferRef SendBuffer = ServerPacketHandler::MakeSendBuffer(Msg);
		Cast<UTrapperGameInstance>(GetGameInstance())->SendPacket(SendBuffer);
	}
}

void UInputRoomNumberWidgetBase::OnQuitInputRoomNumberButton()
{
	// 뒤로 버튼 눌렀을 때 팝업 사라지게 하고 입력창도 초기화한다.
	InputRoomNumber->SetText(FText::FromString(""));

	ATitlePC* PC = GetOwningPlayer<ATitlePC>();
	if (PC)
	{
		PC->ShowInputRoomNumberPopup(false);
	}
}

void UInputRoomNumberWidgetBase::ShowRoomErrorPop(FString Text)
{
	RoomErrorMessage->SetText(FText::FromString(Text));

	RoomErrorPopUpCanvasPanel->SetVisibility(ESlateVisibility::Visible);
}

void UInputRoomNumberWidgetBase::HideRoomErrorPop()
{
	RoomErrorPopUpCanvasPanel->SetVisibility(ESlateVisibility::Collapsed);
}