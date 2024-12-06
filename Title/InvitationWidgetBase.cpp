// Fill out your copyright notice in the Description page of Project Settings.


#include "Title/InvitationWidgetBase.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "TrapperGameInstance.h"
#include "ServerPacketHandler.h"
#include "Protocol.pb.h"

void UInvitationWidgetBase::NativeConstruct()
{
	InvitationText = Cast<UTextBlock>(GetWidgetFromName(TEXT("InvitationText")));
	AcceptButton = Cast<UButton>(GetWidgetFromName(TEXT("AcceptButton")));
	RejectButton = Cast<UButton>(GetWidgetFromName(TEXT("RejectButton")));

	if (AcceptButton)
	{
		AcceptButton->OnClicked.AddDynamic(this, &UInvitationWidgetBase::OnAcceptButton);
		AcceptButton->OnHovered.AddDynamic(this, &UInvitationWidgetBase::OnButtonHovered);
	}

	if (RejectButton)
	{
		RejectButton->OnClicked.AddDynamic(this, &UInvitationWidgetBase::OnRejectButton);
		RejectButton->OnHovered.AddDynamic(this, &UInvitationWidgetBase::OnButtonHovered);
	}
}

void UInvitationWidgetBase::OnAcceptButton()
{
	UGameplayStatics::PlaySound2D(this, MouseClickSound);

	Protocol::C_JOIN_ROOM pkt;
	pkt.set_roomnum(RoomNumber);

	SendBufferRef SendBuffer = ServerPacketHandler::MakeSendBuffer(pkt);
	Cast<UTrapperGameInstance>(GetGameInstance())->SendPacket(SendBuffer);

	this->RemoveFromParent();
}

void UInvitationWidgetBase::OnRejectButton()
{
	UGameplayStatics::PlaySound2D(this, MouseClickSound);

	this->RemoveFromParent();
}

void UInvitationWidgetBase::SetRoomNumber(int32 RNumber)
{
	RoomNumber = RNumber;
}

void UInvitationWidgetBase::SetNickname(FString RNickname)
{
	Nickname = RNickname;
}

void UInvitationWidgetBase::SetInvitationText(FString SInvitationText)
{
	InvitationText->SetText(FText::FromString(SInvitationText));
}

void UInvitationWidgetBase::OnButtonHovered()
{
	UGameplayStatics::PlaySound2D(this, MouseHoverSound);
}
