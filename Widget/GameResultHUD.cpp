// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/GameResultHUD.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Components/CanvasPanel.h"
#include "Kismet/GameplayStatics.h"
#include "Framework/TrapperPlayerController.h"
#include "Framework/TrapperGameState.h"

void UGameResultHUD::NativeConstruct()
{
	Super::NativeConstruct();

	BackTitleButton->OnClicked.AddDynamic(this, &UGameResultHUD::BackTitle);
	ContinueButton->OnClicked.AddDynamic(this, &UGameResultHUD::Continue);
	BossButton->OnClicked.AddDynamic(this, &UGameResultHUD::GoToBoss);

	if(ClientLeftCanvasPanel)
	{
		ClientLeftCanvasPanel->SetVisibility(ESlateVisibility::Collapsed);
	}

	if (GotoTitleButton)
	{
		GotoTitleButton->OnClicked.AddDynamic(this, &UGameResultHUD::ServerGotoTitle);
	}
}

void UGameResultHUD::BackTitle()
{
	ATrapperPlayerController* PC = GetOwningPlayer<ATrapperPlayerController>();
	PC->EndGameReturnTitle();

	//UE_LOG(LogTemp, Warning, TEXT("Back To Title"));
	//UGameplayStatics::OpenLevel(this, TEXT("Title"));

	//GetWorld()->GetFirstPlayerController()->ClientReturnToMainMenuWithTextReason(FText::FromString(TEXT("접속 종료")));
}

void UGameResultHUD::Continue()
{
	// TODO :: 클라이언트가 타이틀로 돌아갔을 경우 경고창 띄우고 타이틀로 돌아가기

	ATrapperPlayerController* PC = GetOwningPlayer<ATrapperPlayerController>();
	if (PC->GetClientDisconnected())
	{
		UE_LOG(LogTemp, Warning ,TEXT("ClientDisconnected"));
		ClientLeftCanvasPanel->SetVisibility(ESlateVisibility::Visible);
		return;
	}

	UE_LOG(LogTemp, Warning ,TEXT("Continue"));

	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		ATrapperPlayerController* PlayerController = Cast<ATrapperPlayerController>(Iterator->Get());
		PlayerController->MulticastGameRestart();
	}

	GetWorld()->GetGameState<ATrapperGameState>()->OnEventExecute.Broadcast(ETrapperEventCode::E_ContinueGame);
}

void UGameResultHUD::GoToBoss()
{
	// TODO :: 클라이언트가 타이틀로 돌아갔을 경우 경고창 띄우고 타이틀로 돌아가기

	ATrapperPlayerController* PC = GetOwningPlayer<ATrapperPlayerController>();
	if (PC->GetClientDisconnected())
	{
		UE_LOG(LogTemp, Warning, TEXT("ClientDisconnected"));
		ClientLeftCanvasPanel->SetVisibility(ESlateVisibility::Visible);
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("Go To Boss"));

	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		ATrapperPlayerController* PlayerController = Cast<ATrapperPlayerController>(Iterator->Get());
		PlayerController->MulticastGameRestart();
	}

	GetWorld()->GetGameState<ATrapperGameState>()->OnEventExecute.Broadcast(ETrapperEventCode::E_GoToBossStage);
}

void UGameResultHUD::ServerGotoTitle()
{
	UGameplayStatics::OpenLevel(this, TEXT("Title"));
}

void UGameResultHUD::SetGameEndText(bool IsGameClear)
{
	if (IsGameClear)
	{
		FString ClearText(TEXT("Demo Clear"));
		FString ClearDescriptionText(TEXT("플레이 해주셔서 감사합니다.\n\n다음 업데이트를 기대해주세요!"));

		MainText->SetText(FText::FromString(ClearText));
		MainText->SetColorAndOpacity(FSlateColor(FLinearColor(206.f / 255.f, 110.f / 255.f, 0.f / 255.f, 1.f)));
		DescriptionText->SetText(FText::FromString(ClearDescriptionText));
		ToBeContinuedText->SetVisibility(ESlateVisibility::Visible);
		ContinueButton->SetVisibility(ESlateVisibility::Collapsed);
		BossButton->SetVisibility(ESlateVisibility::Collapsed);
	}
	else
	{
		FString GameOverText(TEXT("Game Over"));
		FString GameOverDescriptionText(TEXT("돌아가기 버튼을 누르면 타이틀로 이동합니다."));

		MainText->SetText(FText::FromString(GameOverText));
		MainText->SetColorAndOpacity(FSlateColor(FLinearColor(207.f / 255.f, 0.f / 255.f, 0.f / 255.f, 1.f)));
		DescriptionText->SetText(FText::FromString(GameOverDescriptionText));
		ToBeContinuedText->SetVisibility(ESlateVisibility::Collapsed);

		for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
		{
			ATrapperPlayerController* PlayerController = Cast<ATrapperPlayerController>(Iterator->Get());
			if (PlayerController->HasAuthority() && PlayerController->IsLocalController())
			{
				ContinueButton->SetVisibility(ESlateVisibility::Visible);

				if (GetWorld()->GetGameState<ATrapperGameState>()->MaintenanceCount > 4)
				{
					return;
				}

				BossButton->SetVisibility(ESlateVisibility::Visible);
				return;
			}
		}
	}
}
