// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ScoreHUD.generated.h"

/**
 *
 */
UCLASS()
class TRAPPERPROJECT_API UScoreHUD : public UUserWidget
{
	GENERATED_BODY()

private:
	virtual void NativeConstruct() override;

	UFUNCTION()
	void OnClickEndScreenButton();

public:
	void UpdateScoreHUD(struct FScoreData Scores);

private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> MonsterScore1P;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> MonsterScore2P;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> TrapScore1P;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> TrapScore2P;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> CollaborationScore;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> ContributionPercent1P;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> ContributionPercent2P;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UProgressBar> ContributionBar;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> TotalScore;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> RankScore;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> WaveText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> EndScreenButton;
};
