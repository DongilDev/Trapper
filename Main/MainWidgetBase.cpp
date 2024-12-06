// Fill out your copyright notice in the Description page of Project Settings.


#include "MainWidgetBase.h"
#include "Components/Button.h"
#include "Components/EditableTextBox.h"
#include "MainPC.h"
#include "Kismet/GameplayStatics.h"
#include "Main/JoinWidgetBase.h"

#include "MediaPlayer.h"
#include "MediaSource.h"

void UMainWidgetBase::NativeConstruct()
{
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

	if (LoginButton)
	{
		LoginButton->OnClicked.AddDynamic(this, &UMainWidgetBase::OnLoginButton);
		LoginButton->OnHovered.AddDynamic(this, &UMainWidgetBase::OnButtonHovered);
	}

	if (JoinButton)
	{
		JoinButton->OnClicked.AddDynamic(this, &UMainWidgetBase::OnJoinButton);
		JoinButton->OnHovered.AddDynamic(this, &UMainWidgetBase::OnButtonHovered);
	}
}

void UMainWidgetBase::OnJoinButton()
{
	UGameplayStatics::PlaySound2D(this, MouseClickSound);

	AMainPC* PC = GetOwningPlayer<AMainPC>();
	if (PC)
	{
		PC->ShowJoinWidget(true);
	}
}

void UMainWidgetBase::OnLoginButton()
{
	UGameplayStatics::PlaySound2D(this, MouseClickSound);

	// 로그인
	AMainPC* PC = GetOwningPlayer<AMainPC>();
	if (PC)
	{
		PC->ShowLoginWidget(true);
	}
}

void UMainWidgetBase::OnButtonHovered()
{
	UGameplayStatics::PlaySound2D(this, MouseHoverSound);
}