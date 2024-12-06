// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/PlayerHUD.h"
#include "Widget/PlayerHealthBar.h"
#include "Widget/CrosshairHUD.h"
#include "Widget/WaveInfoWidget.h"
#include "Widget/TowerPortraitHUD.h"
#include "Widget/TrapFailInstallHUD.h"

#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"
#include "Components/RadialSlider.h"
#include "Components/Image.h"
#include "Components/CanvasPanel.h"
#include "Components/VerticalBox.h"
#include "Components/Overlay.h"
#include "Components/SizeBox.h"

#include "Kismet/GameplayStatics.h"
#include "Character/TrapperPlayer.h"
#include "TrapperProject.h"


void UPlayerHUD::NativeConstruct()
{
	Super::NativeConstruct();

	BowControl_Magnetic->SetVisibility(ESlateVisibility::Collapsed);
	if (GetOwningPlayer()->HasAuthority())
	{
		if (IsValid(MagneticArrow1P))
			BowControl_Magnetic->SetBrushFromTexture(MagneticArrow1P);
	}
	else
	{
		if (IsValid(MagneticArrow2P))
			BowControl_Magnetic->SetBrushFromTexture(MagneticArrow2P);
	}
}

void UPlayerHUD::FindPlayer()
{
	if (Player && TeamPlayer) return;

	TArray<AActor*> OutActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATrapperPlayer::StaticClass(), OutActors);

	for (const auto& Pawn : OutActors)
	{
		ATrapperPlayer* TrapperPlayer = Cast<ATrapperPlayer>(Pawn);
		if (!TrapperPlayer) continue;

		if (TrapperPlayer->IsLocallyControlled())
		{
			Player = TrapperPlayer;
		}
		else
		{
			TeamPlayer = TrapperPlayer;
		}
	}

	SetPlayerName();
}

void UPlayerHUD::SetPlayerName()
{
	if (Player->HasAuthority())
	{
		HealthBar->NameText->SetText(FText::FromString(TEXT("리아나")));
		HealthBar->TeamNameText->SetText(FText::FromString(TEXT("디아나")));
	}
	else
	{
		HealthBar->NameText->SetText(FText::FromString(TEXT("디아나")));
		HealthBar->TeamNameText->SetText(FText::FromString(TEXT("리아나")));
	}
}

void UPlayerHUD::SetHealthUI()
{
	FindPlayer();

	// ------------------------------------------------------------------------
	// Player

	float Health = Player->CurrentHP;
	float MaxHealth = Player->MaxHP;
	float HealthPercent = Health / MaxHealth;
	HealthBar->HealthBar->SetPercent(HealthPercent);

	// ------------------------------------------------------------------------
	// Team Player

	if (!TeamPlayer)
	{
		HealthBar->TeamInfoCanvas->SetVisibility(ESlateVisibility::Collapsed);
		return;
	}

	HealthBar->TeamInfoCanvas->SetVisibility(ESlateVisibility::HitTestInvisible);

	float TeamHealth = TeamPlayer->CurrentHP;
	float MaxTeamHealth = TeamPlayer->MaxHP;
	float TeamHealthPercent = TeamHealth / MaxTeamHealth;
	HealthBar->TeamHealthBar->SetPercent(TeamHealthPercent);

	//UE_LOG(LogTemp, Warning, TEXT("Health %f/%f, Team Health %f/%f"), Health, MaxHealth, TeamHealth, MaxTeamHealth);
}

void UPlayerHUD::SetSkipGauge()
{
	FindPlayer();

	if (Player->HasAuthority())
	{
		SkipGaugeFirstPlayer->SetValue(Player->SkipGauge);

		if (TeamPlayer)
		{
			SkipGaugeSecondPlayer->SetValue(1 - TeamPlayer->SkipGauge);
		}
		else
		{
			SkipGaugeSecondPlayer->SetValue(1.f);
		}
	}
	else
	{
		SkipGaugeSecondPlayer->SetValue(1 - Player->SkipGauge);
		SkipGaugeFirstPlayer->SetValue(TeamPlayer->SkipGauge);
	}

}

void UPlayerHUD::SetItemCount(int32 Count)
{
	if (Player->IsLocallyControlled())
	{
		ItemText->SetText(FText::FromString(FString::FromInt(Count)));
	}
}

void UPlayerHUD::SetSkipIcon(bool Value)
{
	if (Value)
	{
		SkipIcon->SetVisibility(ESlateVisibility::HitTestInvisible);
	}
	else
	{
		SkipIcon->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UPlayerHUD::SetCurrentMonsterCount(int32 Count)
{
	int32 MonsterCount = FMath::Clamp(Count, 0, 999);
	CurrentMonsterCount->SetText(FText::FromString(FString::FromInt(MonsterCount)));
}

void UPlayerHUD::SetRemainingMonsterCount(int32 Count)
{
	LastMonsterCount->SetText(FText::FromString(FString::FromInt(Count)));
}

void UPlayerHUD::SetMagneticPillarCount(int32 Count, int32 MaxCount)
{
	MagneticPillarCount->SetText(FText::FromString(FString::FromInt(Count)));
	MagneticPillarMaxCount->SetText(FText::FromString(FString::FromInt(MaxCount)));
}

void UPlayerHUD::SetTowerHealth(float TowerHealth, float MaxTowerHealth)
{
	FString TowerHealthString = FString::FromInt(TowerHealth) + TEXT(" / ") + FString::FromInt(MaxTowerHealth);
	TowerHealthText->SetText(FText::FromString(TowerHealthString));

	float HealthPercent = TowerHealth / MaxTowerHealth;
	TowerHealthBar->SetPercent(HealthPercent);
}

void UPlayerHUD::SetTowerUpgrade(int32 TowerLevel)
{
	int i = 1;
	for (i; i <= TowerLevel; i++)
	{
		switch (i)
		{
		case 1:
			Upgrade1->SetBrushFromTexture(UpgradeImg);
			break;

		case 2:
			Upgrade2->SetBrushFromTexture(UpgradeImg);
			break;

		case 3:
			Upgrade3->SetBrushFromTexture(UpgradeImg);
			break;

		case 4:
			Upgrade4->SetBrushFromTexture(UpgradeImg);
			break;

		case 5:
			Upgrade5->SetBrushFromTexture(UpgradeImg);
			break;

		case 6:
			Towerportrait->TowerNormal->SetVisibility(ESlateVisibility::Collapsed);
			Towerportrait->TowerAwaken->SetVisibility(ESlateVisibility::HitTestInvisible);
			break;

		default:
			break;
		}
	}
}

void UPlayerHUD::ShowWaveInfo(bool Value)
{
	if (Value)
	{
		WaveInfo->SetVisibility(ESlateVisibility::HitTestInvisible);
	}
	else
	{
		WaveInfo->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UPlayerHUD::SetWaveInfo(int32 CurrentWave, int32 MaxWave)
{
	FString TowerWaveString = TEXT("Wave ") + FString::FromInt(CurrentWave) + TEXT(" / ") + FString::FromInt(MaxWave);
	WaveInfo->TowerWaveText->SetText(FText::FromString(TowerWaveString));
}

void UPlayerHUD::SetWaveInfoTitle(FString Title)
{
	WaveInfo->TitleText->SetText(FText::FromString(Title));
}

void UPlayerHUD::SetWaveInfoRemainingTime(float LeftTime)
{
	int32 Minutes = FMath::FloorToInt(LeftTime / 60.f);
	int32 Seconds = FMath::FloorToInt(FMath::Fmod(LeftTime, 60));

	FString TimeString = FString::Printf(TEXT("%02d:%02d"), Minutes, Seconds);

	WaveInfo->LeftTimeText->SetText(FText::FromString(TimeString));
}


void UPlayerHUD::ShowQuestLayout(bool Value)
{
	if (Value)
	{
		PlayAnimation(QuestActiveAnimation);
	}
	else
	{
		PlayAnimation(QuestDeActiveAnimation);
	}
}

void UPlayerHUD::SetQuestText(FString InTitle, FString InContents)
{
	TitleText->SetText(FText::FromString(InTitle));
	ContentsText->SetText(FText::FromString(InContents));
}

void UPlayerHUD::SetArrowControlIcon(bool bIsMagnetic)
{
	if (bIsMagnetic)
	{
		BowControl->SetVisibility(ESlateVisibility::Collapsed);
		BowControl_Magnetic->SetVisibility(ESlateVisibility::HitTestInvisible);
	}
	else
	{
		BowControl->SetVisibility(ESlateVisibility::HitTestInvisible);
		BowControl_Magnetic->SetVisibility(ESlateVisibility::Collapsed);
	}
}

/// --------------------------------------------------------------------------------
///                                    Aim
/// --------------------------------------------------------------------------------

void UPlayerHUD::SetShiftFrameVisibility(bool bIsVisible)
{
	if (IsValid(Crosshair))
		Crosshair->SetShiftFrameVisibility(bIsVisible);
}

void UPlayerHUD::SetMagneticFrameVisibility(bool bIsVisible)
{
	if (IsValid(Crosshair))
		Crosshair->SetMagneticFrameVisibility(bIsVisible);
}

void UPlayerHUD::SetDrawFrameVisibility(bool bIsVisible)
{
	if (IsValid(Crosshair))
		Crosshair->SetDrawFrameVisibility(bIsVisible);
}

void UPlayerHUD::SetDrawFrameScale(float DrawTime, float MaxDrawTime)
{
	if (IsValid(Crosshair))
		Crosshair->SetDrawFrameScale(DrawTime, MaxDrawTime);
}

void UPlayerHUD::SetTrapFrameVisibility(bool bIsVisible)
{
	if (IsValid(Crosshair))
		Crosshair->SetTrapFrameVisibility(bIsVisible);
}

void UPlayerHUD::SetTrapDemolishFrameVisibility(bool bIsVisible)
{
	if (IsValid(Crosshair))
		Crosshair->SetTrapDemolishFrameVisibility(bIsVisible);
}

void UPlayerHUD::SetTrapWeckageFrameVisibility(bool bIsVisible)
{
	if (IsValid(Crosshair))
		Crosshair->SetTrapWeckageFrameVisibility(bIsVisible);
}

void UPlayerHUD::SetInteractFrameVisibility(bool bIsVisible)
{
	if (IsValid(Crosshair))
		Crosshair->SetInteractFrameVisibility(bIsVisible);
}


void UPlayerHUD::SetTrapAimFrameVisibility(bool bIsVisible)
{
	if (IsValid(Crosshair))
		Crosshair->SetTrapAimFrameVisibility(bIsVisible);
}

void UPlayerHUD::OnHit()
{
	HitAccumulatedTime = 0.f;
	GetWorld()->GetTimerManager().SetTimer(
		HitTimer, this, &UPlayerHUD::HitTimerFunction, HitTimerTime, true);
	
	HitImage->SetVisibility(ESlateVisibility::HitTestInvisible);
	HitImage->SetRenderOpacity(1.f);
}


void UPlayerHUD::HitTimerFunction()
{
	float LerpTime = 0.5f;

	if(LerpTime < HitAccumulatedTime)
	{
		HitImage->SetVisibility(ESlateVisibility::Collapsed);
		GetWorld()->GetTimerManager().ClearTimer(HitTimer);
		HitAccumulatedTime = 0.f;
		return;
	}

	HitAccumulatedTime += HitTimerTime;
	float HitOpacity = FMath::Lerp(1.f, 0.f, HitAccumulatedTime / LerpTime);
	HitImage->SetRenderOpacity(HitOpacity);
}

void UPlayerHUD::ShowCheatModeText(bool Value)
{
	if (Value)
	{
		CheatModeText->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		CheatModeText->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UPlayerHUD::ShowInvincibilityModeText(bool Value)
{
	if (Value)
	{
		InvincibilityModeText->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		InvincibilityModeText->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UPlayerHUD::SetTrapFailInstallText(FString string)
{
	TrapFailInstallHUD->ShowText(string);
}

