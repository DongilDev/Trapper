// Fill out your copyright notice in the Description page of Project Settings.


#include "Lobby/InvitationFriendListWidgetBase.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "TrapperGameInstance.h"
#include "ServerPacketHandler.h"
#include "Protocol.pb.h"

void UInvitationFriendListWidgetBase::NativeConstruct()
{
	InviteButton = Cast<UButton>(GetWidgetFromName(TEXT("InviteButton")));
	if (InviteButton)
	{
		InviteButton->OnClicked.AddDynamic(this, &UInvitationFriendListWidgetBase::OnInviteButton);
		InviteButton->OnHovered.AddDynamic(this, &UInvitationFriendListWidgetBase::OnButtonHovered);
	}
}

void UInvitationFriendListWidgetBase::OnInviteButton()
{
	UGameplayStatics::PlaySound2D(this, MouseClickSound);

	Protocol::C_SEND_INVITATION pkt;
	pkt.set_friendnickname(std::string(TCHAR_TO_UTF8(*Nickname)));
	pkt.set_roomnum(Cast<UTrapperGameInstance>(GetGameInstance())->GetRoomNumber());

	SendBufferRef SendBuffer = ServerPacketHandler::MakeSendBuffer(pkt);
	Cast<UTrapperGameInstance>(GetGameInstance())->SendPacket(SendBuffer);

	UTexture2D* InvitedButtonTexture = LoadObject<UTexture2D>(nullptr, TEXT("/Script/Engine.Texture2D'/Game/Blueprints/Lobby/Assets/WF_Imge_InviteCompleted.WF_Imge_InviteCompleted'"));
	FButtonStyle  Style = InviteButton->GetStyle();
	Style.Normal.SetResourceObject(InvitedButtonTexture);
	InviteButton->SetStyle(Style);
	InviteButton->SetIsEnabled(false);
}

void UInvitationFriendListWidgetBase::SetInviteButtonVisible(bool Visible)
{
	if(Visible)
	{
		UTexture2D* OnlineButtonTexture = LoadObject<UTexture2D>(nullptr, TEXT("/Script/Engine.Texture2D'/Game/Blueprints/Lobby/Assets/WF_Btn_Invite_Impact.WF_Btn_Invite_Impact'"));
		FButtonStyle  Style = InviteButton->GetStyle();
		Style.Normal.SetResourceObject(OnlineButtonTexture);
		Style.Hovered.SetResourceObject(OnlineButtonTexture);
		Style.Hovered.SetImageSize(Style.Normal.GetImageSize());
		Style.Pressed.SetResourceObject(OnlineButtonTexture);
		Style.Pressed.SetImageSize(Style.Normal.GetImageSize());
		InviteButton->SetStyle(Style);
	}
	else
	{
		UTexture2D* OfflineButtonTexture = LoadObject<UTexture2D>(nullptr, TEXT("/Script/Engine.Texture2D'/Game/Blueprints/Lobby/Assets/WF_Btn_Invite.WF_Btn_Invite'"));
		FButtonStyle  Style = InviteButton->GetStyle();
		Style.Normal.SetResourceObject(OfflineButtonTexture);
		InviteButton->SetStyle(Style);
		InviteButton->SetIsEnabled(false);
	}
}

void UInvitationFriendListWidgetBase::SetNickname(FString SNickname)
{
	Nickname = SNickname;
}

void UInvitationFriendListWidgetBase::OnButtonHovered()
{
	UGameplayStatics::PlaySound2D(this, MouseHoverSound);
}
