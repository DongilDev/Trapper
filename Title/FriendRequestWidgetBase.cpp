// Fill out your copyright notice in the Description page of Project Settings.


#include "Title/FriendRequestWidgetBase.h"
#include "Title/FriendWidgetBase.h"
#include "TrapperGameInstance.h"
#include "ServerPacketHandler.h"
#include "Protocol.pb.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"

void UFriendRequestWidgetBase::NativeConstruct()
{
	RequestFriendIdText = Cast<UTextBlock>(GetWidgetFromName(TEXT("RequestFriendIdText")));
	AcceptButton = Cast<UButton>(GetWidgetFromName(TEXT("AcceptButton")));
	RejectButton = Cast<UButton>(GetWidgetFromName(TEXT("RejectButton")));

	if (AcceptButton)
	{
		AcceptButton->OnClicked.AddDynamic(this, &UFriendRequestWidgetBase::OnAcceptButton);
		AcceptButton->OnHovered.AddDynamic(this, &UFriendRequestWidgetBase::OnButtonHovered);
	}

	if (RejectButton)
	{
		RejectButton->OnClicked.AddDynamic(this, &UFriendRequestWidgetBase::OnRejectButton);
		RejectButton->OnHovered.AddDynamic(this, &UFriendRequestWidgetBase::OnButtonHovered);
	}
}

void UFriendRequestWidgetBase::OnAcceptButton()
{
	UGameplayStatics::PlaySound2D(this, MouseClickSound);

	TMap<FString, FString>& FriendRequestsMap = Cast<UTrapperGameInstance>(GetGameInstance())->GetFriendRequests();

	const FString* RequestedId = FriendRequestsMap.FindKey(RequestFriendIdText->GetText().ToString());

	if (!RequestedId)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("friendrequest 맵에 없음")));
	}
	else
	{
		// DB에 친구 추가하라고 서버에 요청
		Protocol::C_ADD_FRIEND AddFriendpkt;
		AddFriendpkt.set_myid(std::string(TCHAR_TO_UTF8(*Cast<UTrapperGameInstance>(GetGameInstance())->GetPlayerID())));
		AddFriendpkt.set_friendid(std::string(TCHAR_TO_UTF8(**RequestedId)));
		AddFriendpkt.set_approve(true);

		SendBufferRef SendBuffer1 = ServerPacketHandler::MakeSendBuffer(AddFriendpkt);
		Cast<UTrapperGameInstance>(GetGameInstance())->SendPacket(SendBuffer1);

		FriendRequestsMap.Remove(*RequestedId);
	}

	FriendWidgetObject->PopulateFriendsRequest();
}

void UFriendRequestWidgetBase::OnRejectButton()
{
	UGameplayStatics::PlaySound2D(this, MouseClickSound);

	TMap<FString, FString>& FriendRequestsMap = Cast<UTrapperGameInstance>(GetGameInstance())->GetFriendRequests();

	const FString* RequestedId = FriendRequestsMap.FindKey(RequestFriendIdText->GetText().ToString());

	if (!RequestedId)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("friendrequest 맵에 없음")));
	}
	else
	{
		Protocol::C_ADD_FRIEND pkt;
		pkt.set_myid(std::string(TCHAR_TO_UTF8(*Cast<UTrapperGameInstance>(GetGameInstance())->GetPlayerID())));
		pkt.set_friendid(std::string(TCHAR_TO_UTF8(**RequestedId)));
		pkt.set_approve(false);

		SendBufferRef SendBuffer = ServerPacketHandler::MakeSendBuffer(pkt);
		Cast<UTrapperGameInstance>(GetGameInstance())->SendPacket(SendBuffer);

		FriendRequestsMap.Remove(*RequestedId);
	}

	FriendWidgetObject->PopulateFriendsRequest();
}

void UFriendRequestWidgetBase::OnButtonHovered()
{
	UGameplayStatics::PlaySound2D(this, MouseHoverSound);
}

void UFriendRequestWidgetBase::SetFriendWidget(UUserWidget* Widget)
{
	FriendWidgetObject = Cast<UFriendWidgetBase>(Widget);
}
