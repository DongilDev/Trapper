// Fill out your copyright notice in the Description page of Project Settings.


#include "Main/JoinWidgetBase.h"
#include "Protocol.pb.h"
#include "Components/CanvasPanel.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Components/EditableTextBox.h"
#include "Kismet/GameplayStatics.h"
#include "TrapperGameInstance.h"
#include "ServerPacketHandler.h"
#include "Main/MainPC.h"

#include "MediaPlayer.h"
#include "MediaSource.h"

void UJoinWidgetBase::NativeConstruct()
{
	Super::NativeConstruct();

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


	if (OkButton)
	{
		OkButton->OnClicked.AddDynamic(this, &UJoinWidgetBase::OnOkButton);
		OkButton->OnHovered.AddDynamic(this, &UJoinWidgetBase::OnButtonHovered);
	}

	if (CancelButton)
	{
		CancelButton->OnClicked.AddDynamic(this, &UJoinWidgetBase::OnCancelButton);
		CancelButton->OnHovered.AddDynamic(this, &UJoinWidgetBase::OnButtonHovered);
	}

	if (ShowPasswordButton)
	{
		ShowPasswordButton->OnClicked.AddDynamic(this, &UJoinWidgetBase::OnShowPasswordButton);
		ShowPasswordButton->OnHovered.AddDynamic(this, &UJoinWidgetBase::OnButtonHovered);
	}

	WarningCanvasPanel->SetVisibility(ESlateVisibility::Collapsed);
}

void UJoinWidgetBase::OnOkButton()
{
	IsJoinable = true;

	FString FId = UserID->GetText().ToString();
	FString FPassword = Password->GetText().ToString();
	FString FNickname = Nickname->GetText().ToString();

	// Id
	if (FId.Len() < 3 || FId.Len() > 16 || ContainsSpecialCharacters(FId))
	{
		FString Temp = FString::Printf(TEXT("3~16자의 한글, 영문, 숫자만 사용가능합니다."));
		CheckId->SetText(FText::FromString(Temp));
		CheckId->SetColorAndOpacity(FSlateColor(FLinearColor::Red));
		IsJoinable = false;
	}
	else
	{
		Protocol::C_CHECK_DUPLICATE_ID Msg;
		Msg.set_playerid(std::string(TCHAR_TO_UTF8(*FId)));

		SendBufferRef SendBuffer = ServerPacketHandler::MakeSendBuffer(Msg);
		Cast<UTrapperGameInstance>(GetGameInstance())->SendPacket(SendBuffer);
	}

	// Pw
	if (FPassword.Len() < 8 || FPassword.Len() > 14 || ContainsSpecialCharacters(FPassword))
	{
		FString Temp = FString::Printf(TEXT("8~14자 이내로 한글, 영문, 숫자만 입력해야합니다."));
		CheckPassword->SetText(FText::FromString(Temp));
		CheckPassword->SetColorAndOpacity(FSlateColor(FLinearColor::Red));
		IsJoinable = false;
	}
	else
	{
		FString Temp = FString::Printf(TEXT("사용가능한 비밀번호입니다."));
		CheckPassword->SetText(FText::FromString(Temp));
		CheckPassword->SetColorAndOpacity(FSlateColor(FLinearColor::Green));
	}
	
	// Nickname
	if (FNickname.Len() < 2 || FNickname.Len() > 8 || ContainsSpecialCharacters(FNickname))
	{
		FString Temp = FString::Printf(TEXT("닉네임은 최소 2글자, 최대 8글자까지 한글, 영문, 숫자만 만들 수 있습니다."));
		CheckNickname->SetText(FText::FromString(Temp));
		CheckNickname->SetColorAndOpacity(FSlateColor(FLinearColor::Red));
		IsJoinable = false;
	}
	else
	{
		FString Temp = FString::Printf(TEXT("사용가능한 닉네임입니다."));
		CheckNickname->SetText(FText::FromString(Temp));
		CheckNickname->SetColorAndOpacity(FSlateColor(FLinearColor::Green));
	}


	if (!IsJoinable)
	{
		JoinFailedUI();
	}
}

void UJoinWidgetBase::OnCancelButton()
{
	UGameplayStatics::PlaySound2D(this, MouseClickSound);

	AMainPC* PC = GetOwningPlayer<AMainPC>();
	PC->ShowMainWidget(true);
}

void UJoinWidgetBase::OnShowPasswordButton()
{
	UGameplayStatics::PlaySound2D(this, MouseClickSound);

	if (Password)
	{
		bool ShowPassword = Password->IsPassword;
		Password->SetIsPassword(!ShowPassword);

		FString ShowPasswordTexturePath;
		UTexture2D* ShowPasswordTexture;
		if (!ShowPassword)
		{
			ShowPasswordTexturePath = "/Script/Engine.Texture2D'/Game/Blueprints/Main/Assets/2p_Btn_Visivle.2p_Btn_Visivle'";
		}
		else
		{
			ShowPasswordTexturePath = "/Script/Engine.Texture2D'/Game/Blueprints/Main/Assets/2p_Btn_NotVisivle.2p_Btn_NotVisivle'";
		}

		ShowPasswordTexture = Cast<UTexture2D>(StaticLoadObject(UTexture2D::StaticClass(), nullptr, *ShowPasswordTexturePath));
		FButtonStyle  Style = ShowPasswordButton->GetStyle();
		Style.Normal.SetResourceObject(ShowPasswordTexture);
		Style.Hovered.SetResourceObject(ShowPasswordTexture);
		Style.Hovered.SetImageSize(Style.Normal.GetImageSize());
		Style.Pressed.SetResourceObject(ShowPasswordTexture);
		Style.Pressed.SetImageSize(Style.Normal.GetImageSize());
		ShowPasswordButton->SetStyle(Style);
	}
}

void UJoinWidgetBase::ShowDuplicateId(bool Duplicated)
{
	if (!Duplicated)
	{
		FString Temp = FString::Printf(TEXT("사용가능한 아이디입니다."));
		CheckId->SetText(FText::FromString(Temp));
		CheckId->SetColorAndOpacity(FSlateColor(FLinearColor::Green));
	}
	else
	{
		FString Temp = FString::Printf(TEXT("중복된 아이디입니다."));
		CheckId->SetText(FText::FromString(Temp));
		CheckId->SetColorAndOpacity(FSlateColor(FLinearColor::Red));
		JoinFailedUI();
		return;
	}
	
	// 전부 다 괜찮다면 회원가입 패킷 보내기
	if (IsJoinable)
	{
		FString FId = UserID->GetText().ToString();
		FString FPassword = Password->GetText().ToString();
		FString FNickname = Nickname->GetText().ToString();

		Protocol::C_CREATE_ACCOUNT Msg;
		Msg.set_playerid(std::string(TCHAR_TO_UTF8(*FId)));
		Msg.set_password(std::string(TCHAR_TO_UTF8(*FPassword)));
		Msg.set_nickname(std::string(TCHAR_TO_UTF8(*FNickname)));
		SendBufferRef SendBuffer = ServerPacketHandler::MakeSendBuffer(Msg);
		Cast<UTrapperGameInstance>(GetGameInstance())->SendPacket(SendBuffer);
		UGameplayStatics::PlaySound2D(this, MouseClickSound);
	}
}

void UJoinWidgetBase::JoinFailedUI()
{
	FString FailedJoinTexturePath = "/Script/Engine.Texture2D'/Game/Blueprints/Main/Assets/2p_Btn_Singup_Warning.2p_Btn_Singup_Warning'";;
	UTexture2D* FailedJoinTexture = Cast<UTexture2D>(StaticLoadObject(UTexture2D::StaticClass(), nullptr, *FailedJoinTexturePath));

	FButtonStyle  Style = OkButton->GetStyle();
	Style.Normal.SetResourceObject(FailedJoinTexture);
	OkButton->SetStyle(Style);

	WarningCanvasPanel->SetVisibility(ESlateVisibility::HitTestInvisible);
	UGameplayStatics::PlaySound2D(this, MouseClickFailedSound);
}

void UJoinWidgetBase::InitalizeUI()
{
	UserID->SetText(FText::FromString(""));
	Password->SetText(FText::FromString(""));
	Nickname->SetText(FText::FromString(""));

	IsJoinable = true;

	FString Temp1 = FString::Printf(TEXT("3~16자의 한글, 영문과 숫자만 사용 가능합니다.."));
	CheckId->SetText(FText::FromString(Temp1));
	CheckId->SetColorAndOpacity(FSlateColor(FLinearColor::White));

	FString Temp2 = FString::Printf(TEXT("8~14자 이내로 입력하셔야합니다."));
	CheckPassword->SetText(FText::FromString(Temp2));
	CheckPassword->SetColorAndOpacity(FSlateColor(FLinearColor::White));
	
	FString Temp3 = FString::Printf(TEXT("닉네임은 최소 2글자, 최대 8글자까지 만들 수 있습니다."));
	CheckNickname->SetText(FText::FromString(Temp3));
	CheckNickname->SetColorAndOpacity(FSlateColor(FLinearColor::White));

	FString JoinTexturePath = "/Script/Engine.Texture2D'/Game/Blueprints/Main/Assets/2p_Btn_Singup.2p_Btn_Singup'";;
	UTexture2D* JoinTexture = Cast<UTexture2D>(StaticLoadObject(UTexture2D::StaticClass(), nullptr, *JoinTexturePath));

	FButtonStyle  Style = OkButton->GetStyle();
	Style.Normal.SetResourceObject(JoinTexture);
	OkButton->SetStyle(Style);

	WarningCanvasPanel->SetVisibility(ESlateVisibility::Collapsed);
}

bool UJoinWidgetBase::ContainsSpecialCharacters(const FString& InputString)
{
	FString SpecialCharacters = TEXT("!@#$%^&*()-_=+[]{};:'\",.<>/?\\|`~");

	for (TCHAR Character : InputString)
	{
		if (SpecialCharacters.Contains(FString(1, &Character)))
		{
			return true;
		}
	}
	return false;
}

void UJoinWidgetBase::OnButtonHovered()
{
	UGameplayStatics::PlaySound2D(this, MouseHoverSound);
}
