// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/ScoreHUD.h"
#include "Framework/TrapperGameState.h"
#include "TrapperProject.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"
#include "Components/Button.h"
#include "Framework/TrapperPlayerController.h"


void UScoreHUD::NativeConstruct()
{
	Super::NativeConstruct();

	EndScreenButton->OnClicked.AddDynamic(this, &UScoreHUD::OnClickEndScreenButton);
}

void UScoreHUD::OnClickEndScreenButton()
{
	ATrapperPlayerController* PlayerController = Cast<ATrapperPlayerController>(GetOwningPlayer());
	if (PlayerController)
	{
		PlayerController->ScoreAfterGameEndScreenSetting();
	}
}

void UScoreHUD::UpdateScoreHUD(FScoreData Scores)
{
	//UE_LOG(LogTrap, Warning, TEXT("UScoreHUD::UpdateScoreHUD"));

	MonsterScore1P->SetText(FText::FromString(FString::FromInt(Scores.MonsterScore1P)));
	MonsterScore2P->SetText(FText::FromString(FString::FromInt(Scores.MonsterScore2P)));
	TrapScore1P->SetText(FText::FromString(FString::FromInt(Scores.TrapScore1P)));
	TrapScore2P->SetText(FText::FromString(FString::FromInt(Scores.TrapScore2P)));
	CollaborationScore->SetText(FText::FromString(FString::FromInt(Scores.CollaborationScore)));
	MonsterScore1P->SetText(FText::FromString(FString::FromInt(Scores.MonsterScore1P)));

	uint32 Total1P2P = Scores.MonsterScore1P + Scores.MonsterScore2P + Scores.TrapScore1P + Scores.TrapScore2P;
	uint32 Total1P = Scores.MonsterScore1P + Scores.TrapScore1P;
	uint32 Total2P = Scores.MonsterScore2P + Scores.TrapScore2P;

	float Contribution1PRate = 0.f;
	if (Total1P2P == 0)
	{
		Contribution1PRate = 0.5f;
	}
	else
	{
		Contribution1PRate = static_cast<float>(Total1P) / Total1P2P;
	}

	uint8 Contribution1P = static_cast<uint8>(std::round(Contribution1PRate * 100));
	uint8 Contribution2P = 100 - Contribution1P;

	//UE_LOG(LogTrap, Warning, TEXT("Total1P2P : %d"), Total1P2P);
	//UE_LOG(LogTrap, Warning, TEXT("Total1P : %d"), Total1P);
	//UE_LOG(LogTrap, Warning, TEXT("Contribution1P : %d"), Contribution1P);
	//UE_LOG(LogTrap, Warning, TEXT("Contribution2P : %d"), Contribution2P);
	//UE_LOG(LogTrap, Warning, TEXT("Contribution1PRate : %f"), Contribution1PRate);

	ContributionPercent1P->SetText(FText::FromString(FString::FromInt(Contribution1P)));
	ContributionPercent2P->SetText(FText::FromString(FString::FromInt(Contribution2P)));
	ContributionBar->SetPercent(Contribution1PRate);

	uint32 Total = Total1P2P + Scores.CollaborationScore;

	TotalScore->SetText(FText::FromString(FString::FromInt(Total)));

	FText Rank;
	if (Total >= 90000)
	{
		Rank = FText::FromString("S");
	}
	else if (Total >= 75000)
	{
		Rank = FText::FromString("A");
	}
	else if (Total >= 55000)
	{
		Rank = FText::FromString("B");
	}
	else if (Total >= 35000)
	{
		Rank = FText::FromString("C");
	}
	else
	{
		Rank = FText::FromString("F");
	}

	RankScore->SetText(Rank);

	if (Scores.WaveLevel <= 20)
	{
		WaveText->SetText(FText::FromString(FString::FromInt(Scores.WaveLevel)));
	}
	else if (Scores.WaveLevel == 21) // Boss
	{
		WaveText->SetText(FText::FromString("BOSS"));
	}
	else if (Scores.WaveLevel == 22) // BONUS
	{
		WaveText->SetText(FText::FromString("BONUS"));
	}
	else if (Scores.WaveLevel == 23) // ALL
	{
		WaveText->SetText(FText::FromString("ALL"));
	}
	else
	{
		UE_LOG(LogTrap, Error, TEXT("UScoreHUD::UpdateScoreHUD WaveLevel Error"));
	}
}
