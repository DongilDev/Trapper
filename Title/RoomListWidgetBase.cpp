 // Fill out your copyright notice in the Description page of Project Settings.


#include "Title/RoomListWidgetBase.h"
#include "TrapperGameInstance.h"
#include "ServerPacketHandler.h"
#include "Protocol.pb.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "TitlePC.h"

void URoomListWidgetBase::NativeConstruct()
{
	Super::NativeConstruct();

	if (JoinRoomButton)
	{
		JoinRoomButton->OnClicked.AddDynamic(this, &URoomListWidgetBase::OnJoinButton);
		JoinRoomButton->OnHovered.AddDynamic(this, &URoomListWidgetBase::OnButtonHovered);
	}

	FullRoomErrorTextBlock->SetVisibility(ESlateVisibility::Collapsed);
}

void URoomListWidgetBase::OnJoinButton()
{
	UGameplayStatics::PlaySound2D(this, MouseClickSound);

	// 방 참가를 위해 입력된 방번호를 서버에 보낸다.
	ATitlePC* PC = GetOwningPlayer<ATitlePC>();
	if (PC)
	{
		Protocol::C_JOIN_ROOM Msg;
		Msg.set_roomnum(RoomNumber);
		SendBufferRef SendBuffer = ServerPacketHandler::MakeSendBuffer(Msg);
		Cast<UTrapperGameInstance>(GetGameInstance())->SendPacket(SendBuffer);
	}
}

void URoomListWidgetBase::SetRoomNumText(int32 RoomNum)
{
	RoomNumber = RoomNum;
	RoomNumText->SetText(FText::FromString(FString::FromInt(RoomNum)));
}

void URoomListWidgetBase::SetHostNicknameText(FString HostNickname)
{
	HostNicknameText->SetText(FText::FromString(HostNickname));
}

void URoomListWidgetBase::ShowFullRoomErrorText(bool Show)
{
	if(FullRoomErrorTextBlock)
		FullRoomErrorTextBlock->SetVisibility(Show ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);

	if (Show)
	{
		UGameplayStatics::PlaySound2D(this, MouseClickFailedSound);
	}
}

void URoomListWidgetBase::OnButtonHovered()
{
	UGameplayStatics::PlaySound2D(this, MouseHoverSound);
}
