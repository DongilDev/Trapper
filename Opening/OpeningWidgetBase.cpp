// Fill out your copyright notice in the Description page of Project Settings.


#include "Opening/OpeningWidgetBase.h"
#include "Opening/OpeningPC.h"
#include "Opening/OpeningGM.h"
#include "Components/CanvasPanel.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

#include "Kismet/GameplayStatics.h"

#include "MediaPlayer.h"
#include "MediaTexture.h"
#include "MediaSource.h"
#include "MediaSoundComponent.h"

void UOpeningWidgetBase::NativeConstruct()
{    
	// 로딩
	LoadingCanvasPanel->SetVisibility(ESlateVisibility::Collapsed);
}

void UOpeningWidgetBase::OnMediaFinished()
{
	// 로딩
	ShowLoading();

    // 서버에서만 ServerTravel을 호출
    if (AOpeningPC* PC = Cast<AOpeningPC>(GetOwningPlayer()))
    {
        AOpeningGM* GameMode = Cast<AOpeningGM>(GetWorld()->GetAuthGameMode()); // 서버의 GameMode 가져오기
        if (GameMode)
        {
            GameMode->ServerTravelToTrapper();
        }
    }
}

void UOpeningWidgetBase::PlayOpeningMediaWB()
{
	UMediaPlayer* OpeningMediaPlayer = LoadObject<UMediaPlayer>(nullptr, TEXT("/Script/MediaAssets.MediaPlayer'/Game/Blueprints/Opening/Assets/OpeningMediaplayer.OpeningMediaplayer'"));

	if (OpeningMediaPlayer)
	{
		UMediaSoundComponent* MediaSoundComponent = nullptr;

		if (AOpeningPC* PC = Cast<AOpeningPC>(GetOwningPlayer()))
		{
			MediaSoundComponent = NewObject<UMediaSoundComponent>(PC);
			MediaSoundComponent->RegisterComponent();  // 컴포넌트를 등록하여 활성화
			PC->AddOwnedComponent(MediaSoundComponent);
		}

		if (MediaSoundComponent)
		{
			MediaSoundComponent->SetMediaPlayer(OpeningMediaPlayer);
			MediaSoundComponent->SetVolumeMultiplier(0.5f);
			MediaSoundComponent->SetAutoActivate(true);
		}

		// 미디어 소스 로드
		UMediaSource* MediaSource = LoadObject<UMediaSource>(nullptr, TEXT("/Script/MediaAssets.FileMediaSource'/Game/Movies/OpeningSource.OpeningSource'"));
		//UMediaSource* MediaSource = LoadObject<UMediaSource>(nullptr, TEXT("/Script/MediaAssets.FileMediaSource'/Game/Movies/MainTitle.MainTitle'"));
		if (!MediaSource)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("MediaSource NULL")));
		}

		if (OpeningMediaPlayer->OpenSource(MediaSource))
		{
			OpeningMediaPlayer->OnEndReached.AddDynamic(this, &UOpeningWidgetBase::OnMediaFinished); // 미디어가 끝났을 때 함수 호출
			OpeningMediaPlayer->Play(); // 재생 시작
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("OpenSource Fail")));
		}
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("OpeningMediaPlayer NULL")));
	}
}

void UOpeningWidgetBase::SetSkipCount(int32 Count)
{
	FString Temp = FString::Printf(TEXT("F키를 눌러 스킵 (%d/2)"), Count);
	if (SkipTextBlock)
	{
		SkipTextBlock->SetText(FText::FromString(Temp));
		SkipTextBlock->SetColorAndOpacity(FSlateColor(FLinearColor(1.0f, 1.0f, 1.0f, 1.0f)));
	}
}

void UOpeningWidgetBase::ShowLoading()
{
	UMediaPlayer* OpeningMediaPlayer = LoadObject<UMediaPlayer>(nullptr, TEXT("/Script/MediaAssets.MediaPlayer'/Game/Blueprints/Opening/Assets/OpeningMediaplayer.OpeningMediaplayer'"));
	if (OpeningMediaPlayer->IsPlaying())
	{
		OpeningMediaPlayer->Close();
	}
	
	if (LoadingCanvasPanel->GetVisibility() != ESlateVisibility::HitTestInvisible)
	{
		LoadingCanvasPanel->SetVisibility(ESlateVisibility::HitTestInvisible);
	}

	int32 RandomBackground = FMath::RandRange(0, 17);

	FString AssetPath = FString::Printf(TEXT("/Game/Blueprints/Lobby/Assets/HighresScreenshot%05d.HighresScreenshot%05d"), RandomBackground, RandomBackground);
	UTexture2D* BackgroundTexture = LoadObject<UTexture2D>(nullptr, *AssetPath);

	if (BackgroundTexture)
	{
		BackgroundImage->SetBrushFromTexture(BackgroundTexture);
	}

	int32 RandomTipsText = FMath::RandRange(0, 7);

	FString Temp = "";
	switch (RandomTipsText)
	{
	case 0:
		Temp = FString::Printf(TEXT("협동 함정을 신성화살로 함께 맞춰 사용하면 높은 피해를 줄 수 있습니다."));
		TipsTextBlock->SetText(FText::FromString(Temp));
		break;
	case 1:
		Temp = FString::Printf(TEXT("전선이 밀리지 않도록 최전방에서 전투하면 더욱 쉽게 플레이할 수 있습니다."));
		TipsTextBlock->SetText(FText::FromString(Temp));
		break;
	case 2:
		Temp = FString::Printf(TEXT("사용한 함정 잔해에 E키를 사용해 재설치할 수 있습니다."));
		TipsTextBlock->SetText(FText::FromString(Temp));
		break;
	case 3:
		Temp = FString::Printf(TEXT("Ctrl키를 이용해 상대방에게 정보를 알려줄 수 있습니다."));
		TipsTextBlock->SetText(FText::FromString(Temp));
		break;
	case 4:
		Temp = FString::Printf(TEXT("몬스터는 포탑 주변으로 몰려와 포탑을 파괴합니다."));
		TipsTextBlock->SetText(FText::FromString(Temp));
		break;
	case 5:
		Temp = FString::Printf(TEXT("포탑 근처에 2명의 플레이어가 함께 있을 경우 포탑이 강화됩니다."));
		TipsTextBlock->SetText(FText::FromString(Temp));
		break;
	case 6:
		Temp = FString::Printf(TEXT("2명의 플레이어가 마법석에 신성 화살을 맞출 경우 몬스터를 끌어들입니다."));
		TipsTextBlock->SetText(FText::FromString(Temp));
		break;
	case 7:
		Temp = FString::Printf(TEXT("함정은 포탑과 마법석을 활용할 수 있는 효율적인 위치에 설치하는 것이 좋습니다."));
		TipsTextBlock->SetText(FText::FromString(Temp));
		break;
	default:
		break;
	}
}
