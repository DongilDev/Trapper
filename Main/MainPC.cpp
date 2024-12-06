// Fill out your copyright notice in the Description page of Project Settings.


#include "MainPC.h"
#include "Blueprint/UserWidget.h"
#include "MainWidgetBase.h"
#include "Main/JoinWidgetBase.h"
#include "Main/LoginWidgetBase.h"
#include "Main/AfterJoinWidgetBase.h"
#include "Kismet/GameplayStatics.h"
#include "Protocol.pb.h"
#include "TrapperGameInstance.h"
#include "ServerPacketHandler.h"

void AMainPC::BeginPlay()
{
	Super::BeginPlay();

	if (IsLocalController() && MainWidgetClass)
	{
		MainWidgetObject = CreateWidget<UMainWidgetBase>(this, MainWidgetClass);
		MainWidgetObject->AddToViewport();

		bShowMouseCursor = true;
		FInputModeUIOnly InputMode;
		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::LockAlways);
		SetInputMode(InputMode);
	}

	if (JoinWidgetClass)
	{
		JoinWidgetObject = CreateWidget<UJoinWidgetBase>(this, JoinWidgetClass);
		JoinWidgetObject->AddToViewport();
		JoinWidgetObject->SetVisibility(ESlateVisibility::Collapsed);
	}

	if (LoginWidgetClass)
	{
		LoginWidgetObject = CreateWidget<ULoginWidgetBase>(this, LoginWidgetClass);
		LoginWidgetObject->AddToViewport();
		LoginWidgetObject->SetVisibility(ESlateVisibility::Collapsed);
	}

	if (AfterJoinWidgetClass)
	{
		AfterJoinWidgetObject = CreateWidget<UAfterJoinWidgetBase>(this, AfterJoinWidgetClass);
		AfterJoinWidgetObject->AddToViewport();
		AfterJoinWidgetObject->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void AMainPC::SendLoginPacket(FString ID, FString Password)
{
	Protocol::C_LOGIN Msg;
	Msg.set_playerid(std::string(TCHAR_TO_UTF8(*ID)));
	Msg.set_password(std::string(TCHAR_TO_UTF8(*Password)));
	SendBufferRef SendBuffer = ServerPacketHandler::MakeSendBuffer(Msg);
	Cast<UTrapperGameInstance>(GetGameInstance())->SendPacket(SendBuffer);
}

void AMainPC::SendAutoLoginPacket()
{
	Protocol::C_AUTO_LOGIN Msg;
	SendBufferRef SendBuffer = ServerPacketHandler::MakeSendBuffer(Msg);
	Cast<UTrapperGameInstance>(GetGameInstance())->SendPacket(SendBuffer);
}

void AMainPC::RecvLoginPacket(bool Success)
{
	if (Success)
	{

	}
	else
	{
		LoginWidgetObject->ChangeLoginFailedUI();
	}
}

void AMainPC::ShowAfterJoinWidget(bool Show)
{
	MainWidgetObject->SetVisibility(ESlateVisibility::Collapsed);
	JoinWidgetObject->SetVisibility(ESlateVisibility::Collapsed);
	LoginWidgetObject->SetVisibility(ESlateVisibility::Collapsed);
	AfterJoinWidgetObject->SetVisibility(Show ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
}

void AMainPC::ShowJoinWidget(bool Show)
{
	AfterJoinWidgetObject->SetVisibility(ESlateVisibility::Collapsed);
	MainWidgetObject->SetVisibility(ESlateVisibility::Collapsed);
	LoginWidgetObject->SetVisibility(ESlateVisibility::Collapsed);
	JoinWidgetObject->SetVisibility(Show ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	JoinWidgetObject->InitalizeUI();
}

void AMainPC::ShowLoginWidget(bool Show)
{
	AfterJoinWidgetObject->SetVisibility(ESlateVisibility::Collapsed);
	MainWidgetObject->SetVisibility(ESlateVisibility::Collapsed);
	JoinWidgetObject->SetVisibility(ESlateVisibility::Collapsed);
	LoginWidgetObject->SetVisibility(Show ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	LoginWidgetObject->InitializeUI();
}

void AMainPC::ShowMainWidget(bool Show)
{
	AfterJoinWidgetObject->SetVisibility(ESlateVisibility::Collapsed);
	LoginWidgetObject->SetVisibility(ESlateVisibility::Collapsed);
	JoinWidgetObject->SetVisibility(ESlateVisibility::Collapsed);
	MainWidgetObject->SetVisibility(Show ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
}

void AMainPC::ShowDuplicateIdWidget(bool Duplicated)
{
	JoinWidgetObject->ShowDuplicateId(Duplicated);
}
