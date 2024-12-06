// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerHUD.generated.h"

/**
 *
 */
UCLASS()
class TRAPPERPROJECT_API UPlayerHUD : public UUserWidget
{
	GENERATED_BODY()

public:
	// Animation
	UPROPERTY(meta = (BindWidgetAnim), Transient, VisibleAnywhere, BlueprintReadOnly, Category = "Quest")
	class UWidgetAnimation* QuestActiveAnimation;
	
	UPROPERTY(meta = (BindWidgetAnim), Transient, VisibleAnywhere, BlueprintReadOnly, Category = "Quest")
	class UWidgetAnimation* QuestDeActiveAnimation;

public:
	// Item
	UPROPERTY(meta = (BindWidget), VisibleAnywhere, BlueprintReadOnly, Category = "Item")
	TObjectPtr<class UTextBlock> ItemText;

	// Player Health Bar
	UPROPERTY(meta = (BindWidget), VisibleAnywhere, BlueprintReadOnly, Category = "Health")
	TObjectPtr<class UPlayerHealthBar> HealthBar;

	// Wave Info
	UPROPERTY(meta = (BindWidget), VisibleAnywhere, BlueprintReadOnly, Category = "Wave")
	TObjectPtr<class UWaveInfoWidget> WaveInfo;

	// Tower
	UPROPERTY(meta = (BindWidget), VisibleAnywhere, BlueprintReadOnly, Category = "Health")
	TObjectPtr<class UTowerPortraitHUD> Towerportrait;

	UPROPERTY(meta = (BindWidget), VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<class UTextBlock> TowerHealthText;

	UPROPERTY(meta = (BindWidget), VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<class UProgressBar> TowerHealthBar;


	// Monster Count
	UPROPERTY(meta = (BindWidget), VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<class UTextBlock> CurrentMonsterCount;

	UPROPERTY(meta = (BindWidget), VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<class UTextBlock> LastMonsterCount;


	// Magnetic Pillar Count
	UPROPERTY(meta = (BindWidget), VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<class UTextBlock> MagneticPillarCount;

	UPROPERTY(meta = (BindWidget), VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<class UTextBlock> MagneticPillarMaxCount;

	// Quest
	UPROPERTY(meta = (BindWidget), VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<class UOverlay> QuestLayout;

	UPROPERTY(meta = (BindWidget), VisibleAnywhere, BlueprintReadOnly, Category = "Quest")
	TObjectPtr<class UTextBlock> TitleText;

	UPROPERTY(meta = (BindWidget), VisibleAnywhere, BlueprintReadOnly, Category = "Quest")
	TObjectPtr<class UTextBlock> ContentsText;

	UPROPERTY(meta = (BindWidget), VisibleAnywhere, BlueprintReadOnly, Category = "Quest")
	TObjectPtr<class USizeBox> SkipIcon;

	UPROPERTY(meta = (BindWidget), VisibleAnywhere, BlueprintReadOnly, Category = "Quest")
	TObjectPtr<class URadialSlider> SkipGaugeFirstPlayer;

	UPROPERTY(meta = (BindWidget), VisibleAnywhere, BlueprintReadOnly, Category = "Quest")
	TObjectPtr<class URadialSlider> SkipGaugeSecondPlayer;

	// Controll
	UPROPERTY(meta=(BindWidget), VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<class UImage> BowControl_Magnetic;

	UPROPERTY(meta=(BindWidget), VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<class UImage> BowControl;

	// Aim
	UPROPERTY(meta = (BindWidget), VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<class UCrosshairHUD> Crosshair;

	// Minimap
	UPROPERTY(meta = (BindWidget), VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<class UMiniMapHUD> MiniMap;

	// Indicator
	UPROPERTY(meta = (BindWidget), VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<class UCanvasPanel> IndicatorPanel;

	UPROPERTY(meta = (BindWidget), VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<class UCanvasPanel> HitIndicatorPanel;

	// Hit
	UPROPERTY(meta=(BindWidget), VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<class UImage> HitImage;

	// Cheat Mode
	UPROPERTY(meta = (BindWidget), VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<class UTextBlock> CheatModeText;

	UPROPERTY(meta = (BindWidget), VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<class UTextBlock> InvincibilityModeText;

	UPROPERTY(meta = (BindWidget), VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<class UTrapFailInstallHUD> TrapFailInstallHUD;

public:
	UPROPERTY(EditAnywhere, Category = "TextureSource");
	class UTexture2D* MagneticArrow1P; 

	UPROPERTY(EditAnywhere, Category = "TextureSource");
	class UTexture2D* MagneticArrow2P; 

public:
	UPROPERTY(EditAnywhere, Category = "TextureSource");
	class UTexture2D* UpgradeImg;

	UPROPERTY(meta = (BindWidget), VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<class UImage> Upgrade1;

	UPROPERTY(meta = (BindWidget), VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<class UImage> Upgrade2;

	UPROPERTY(meta = (BindWidget), VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<class UImage> Upgrade3;

	UPROPERTY(meta = (BindWidget), VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<class UImage> Upgrade4;

	UPROPERTY(meta = (BindWidget), VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<class UImage> Upgrade5;

public:
	void FindPlayer();

	TObjectPtr<class ATrapperPlayer> Player;
	TObjectPtr<class ATrapperPlayer> TeamPlayer;

public:
	virtual void NativeConstruct() override;

public:
	void SetPlayerName();
	void SetHealthUI();

	void SetItemCount(int32 Count);

	void ShowQuestLayout(bool Value);
	void SetQuestText(FString InTitle, FString InContents);
	void SetSkipIcon(bool Value);
	void SetSkipGauge();

	void SetCurrentMonsterCount(int32 Count);
	void SetRemainingMonsterCount(int32 Count);
	void SetMagneticPillarCount(int32 Count, int32 MaxCount);

	void SetTowerHealth(float TowerHealth, float MaxTowerHealth);
	void SetTowerUpgrade(int32 TowerLevel);

	void ShowWaveInfo(bool Value);
	void SetWaveInfo(int32 CurrentWave, int32 MaxWave);
	void SetWaveInfoTitle(FString Title);
	void SetWaveInfoRemainingTime(float LeftTime);

	void SetArrowControlIcon(bool bIsMagnetic);

	// Aim
	void SetShiftFrameVisibility(bool bIsVisible);
	void SetMagneticFrameVisibility(bool bIsVisible);
	void SetDrawFrameVisibility(bool bIsVisible);
	void SetDrawFrameScale(float DrawTime, float MaxDrawTime);
	void SetTrapFrameVisibility(bool bIsVisible);
	void SetTrapDemolishFrameVisibility(bool bIsVisible);
	void SetTrapWeckageFrameVisibility(bool bIsVisible);
	void SetInteractFrameVisibility(bool bIsVisible);
	void SetTrapAimFrameVisibility(bool bIsVisible);

	// Hit
	void OnHit();
	FTimerHandle HitTimer;
	void HitTimerFunction();
	float HitAccumulatedTime = 0.f;
	float HitTimerTime = 0.01f;

	// Cheat
	void ShowCheatModeText(bool Value);
	void ShowInvincibilityModeText(bool Value);

	// Trap
	void SetTrapFailInstallText(FString string);
};

