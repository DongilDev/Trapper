// Fill out your copyright notice in the Description page of Project Settings.


#include "Title/AddFriendWidgetBase.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Kismet/GameplayStatics.h"

#include "TrapperGameInstance.h"
#include "ServerPacketHandler.h"
#include "Protocol.pb.h"

void UAddFriendWidgetBase::NativeConstruct()
{
	if (AddFriendStatusButton)
	{
		AddFriendStatusButton->OnClicked.AddDynamic(this, &UAddFriendWidgetBase::OnAddFriendStatusButton);
		AddFriendStatusButton->OnHovered.AddDynamic(this, &UAddFriendWidgetBase::OnButtonHovered);
	}

	NormalImage = LoadObject<UTexture2D>(nullptr, TEXT("/Game/Textures/NormalImage.NormalImage"));
	HoveredImage = LoadObject<UTexture2D>(nullptr, TEXT("/Game/Textures/HoveredImage.HoveredImage"));

}

void UAddFriendWidgetBase::OnAddFriendStatusButton()
{
	UGameplayStatics::PlaySound2D(this, MouseClickSound);

	FString StatusTexturePath = "/Script/Engine.Texture2D'/Game/Blueprints/Title/Assets/F2_Btn_Wait.F2_Btn_Wait'";
	UTexture2D* StatusTexture = Cast<UTexture2D>(StaticLoadObject(UTexture2D::StaticClass(), nullptr, *StatusTexturePath));

	FButtonStyle  Style = AddFriendStatusButton->GetStyle();
	Style.Normal.SetResourceObject(StatusTexture);
	AddFriendStatusButton->SetStyle(Style);
	AddFriendStatusButton->SetIsEnabled(false);

	// todo Send Request
	Protocol::C_SEND_REQUEST SendRequestpkt;
	SendRequestpkt.set_playerid(std::string(TCHAR_TO_UTF8(*Cast<UTrapperGameInstance>(GetGameInstance())->GetPlayerID())));
	FString FFriend = AddFriendText->GetText().ToString();
	SendRequestpkt.set_friendnickname(std::string(TCHAR_TO_UTF8(*FFriend)));

	SendBufferRef SendBuffer = ServerPacketHandler::MakeSendBuffer(SendRequestpkt);
	Cast<UTrapperGameInstance>(GetGameInstance())->SendPacket(SendBuffer);
}

void UAddFriendWidgetBase::OnButtonHovered()
{
	UGameplayStatics::PlaySound2D(this, MouseHoverSound);
}
