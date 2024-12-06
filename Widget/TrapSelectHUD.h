// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Trap/Utillity/TrapEnums.h"
#include "Trap/Utillity/TrapHUDParts.h"
#include "Trap/Utillity/TrapCount.h"
#include "Trap/Utillity/TrapCCParts.h"
#include "Trap/Utillity/TrapActivationMethodParts.h"
#include "Trap/Utillity/TrapInstallLocationParts.h"
#include "TrapSelectHUD.generated.h"

/**
 *
 */
UCLASS()
class TRAPPERPROJECT_API UTrapSelectHUD : public UUserWidget
{
	GENERATED_BODY()
public:
	UTrapSelectHUD(const FObjectInitializer& ObjectInitializer);

public:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	void OnHovered(ETrapType type, ETrapHUDType bEnabled, bool bShowGuide);
	void LerpAngle(float DeltaTime);

	FVector2D GetSize();
	FVector2D GetCenterPos();
	void UpgradeCost(float ratio);
	void UpgradeDamage(float ratio);

	/// 멤버 함수 ---------------------------------------------------------------------------
private:
	void LoadTable();
	void LoadGuide();
	void ConvertToMap();

	/// 멤버 변수 ---------------------------------------------------------------------------
private:
	UPROPERTY() UDataTable* TrapTable = nullptr;
	UPROPERTY() UDataTable* LocationTable = nullptr;
	UPROPERTY() UDataTable* ActiveTable = nullptr;
	UPROPERTY() UDataTable* CCTable = nullptr;

	TMap<ETrapType, FTrapHUDParts> TrapData;
	TMap<ETrapInstallLocation, FTrapInstallLocationParts> LocationData;
	TMap<ETrapActivationMethod, FTrapActivationMethodParts> ActivationData;
	TMap<ETrapCCType, FTrapCCParts> CCData;

	TMap<ETrapType, FString> TrapCosts;
	TMap<ETrapType, FString> TrapDamages;

	/// 가이드 영상 --------------------------------------------------------------------------
	UPROPERTY() TObjectPtr<class UMediaPlayer> TrapMediaPlayer;
	UPROPERTY() TObjectPtr<class UMediaSource> BarrelGuide;
	UPROPERTY() TObjectPtr<class UMediaSource> OilbagGuide;
	UPROPERTY() TObjectPtr<class UMediaSource> SpearGuide;
	UPROPERTY() TObjectPtr<class UMediaSource> PlankGuide;
	UPROPERTY() TObjectPtr<class UMediaSource> BearGuide;
	UPROPERTY() TObjectPtr<class UMediaSource> SpikeGuide;
	UPROPERTY() TObjectPtr<class UMediaSource> ClapGuide;
	UPROPERTY() TObjectPtr<class UMediaSource> GuillotineGuide;

	/// 위젯 오브젝트들 ---------------------------------------------------------------------
public:
	// 중심 ---------------------------------
	UPROPERTY(meta = (BindWidget), EditAnywhere, BlueprintReadWrite, Category = "Description")
	TObjectPtr<class UCanvasPanel> SidePannel;

	UPROPERTY(meta = (BindWidget), EditAnywhere, BlueprintReadWrite, Category = "Center")
	TObjectPtr<class UImage> CenterRing;

	UPROPERTY(meta = (BindWidget), EditAnywhere, BlueprintReadWrite, Category = "Center")
	TObjectPtr<class UImage> SideRing;

	UPROPERTY(meta = (BindWidget), EditAnywhere, BlueprintReadWrite, Category = "Center")
	TObjectPtr<class UTextBlock> InstallDisabledText;


	// 함정들 ---------------------------------
	UPROPERTY(meta = (BindWidget), EditAnywhere, BlueprintReadWrite, Category = "Trap")
	TObjectPtr<class UImage> Spear;

	UPROPERTY(meta = (BindWidget), EditAnywhere, BlueprintReadWrite, Category = "Trap")
	TObjectPtr<class UImage> Clap;

	UPROPERTY(meta = (BindWidget), EditAnywhere, BlueprintReadWrite, Category = "Trap")
	TObjectPtr<class UImage> Bear;

	UPROPERTY(meta = (BindWidget), EditAnywhere, BlueprintReadWrite, Category = "Trap")
	TObjectPtr<class UImage> Spike;

	UPROPERTY(meta = (BindWidget), EditAnywhere, BlueprintReadWrite, Category = "Trap")
	TObjectPtr<class UImage> Barrel;

	UPROPERTY(meta = (BindWidget), EditAnywhere, BlueprintReadWrite, Category = "Trap")
	TObjectPtr<class UImage> OilBag;

	UPROPERTY(meta = (BindWidget), EditAnywhere, BlueprintReadWrite, Category = "Trap")
	TObjectPtr<class UImage> Guillotine;

	UPROPERTY(meta = (BindWidget), EditAnywhere, BlueprintReadWrite, Category = "Trap")
	TObjectPtr<class UImage> Plank;



	// 중앙 --------------------------------
	UPROPERTY(meta = (BindWidget), EditAnywhere, BlueprintReadWrite, Category = "Description")
	TObjectPtr<class UCanvasPanel> CenterPanel;

	UPROPERTY(meta = (BindWidget), EditAnywhere, BlueprintReadWrite, Category = "Center")
	TObjectPtr<class UTextBlock> TrapName;

	UPROPERTY(meta = (BindWidget), EditAnywhere, BlueprintReadWrite, Category = "Center")
	TObjectPtr<class UTextBlock> Cost;

	UPROPERTY(meta = (BindWidget), EditAnywhere, BlueprintReadWrite, Category = "Center")
	TObjectPtr<class UTextBlock> Damage;

	UPROPERTY(meta = (BindWidget), EditAnywhere, BlueprintReadWrite, Category = "Center")
	TObjectPtr<class UImage> InstallPosImage;

	UPROPERTY(meta = (BindWidget), EditAnywhere, BlueprintReadWrite, Category = "Center")
	TObjectPtr<class UImage> ActivateTypeImage;

	UPROPERTY(meta = (BindWidget), EditAnywhere, BlueprintReadWrite, Category = "Center")
	TObjectPtr<class UImage> CCTypeImage;

	UPROPERTY(meta = (BindWidget), EditAnywhere, BlueprintReadWrite, Category = "Center")
	TObjectPtr<class UTextBlock> InstallPosText;

	UPROPERTY(meta = (BindWidget), EditAnywhere, BlueprintReadWrite, Category = "Center")
	TObjectPtr<class UTextBlock> ActivateTypeText;

	UPROPERTY(meta = (BindWidget), EditAnywhere, BlueprintReadWrite, Category = "Center")
	TObjectPtr<class UTextBlock> CCTypeText;


	// 설치 제한 텍스트 --------------------------------
	UPROPERTY(meta = (BindWidget), EditAnywhere, BlueprintReadWrite, Category = "Center")
	TObjectPtr<class UTextBlock> SpikeText;

	UPROPERTY(meta = (BindWidget), EditAnywhere, BlueprintReadWrite, Category = "Center")
	TObjectPtr<class UTextBlock> ClapText;
	
	UPROPERTY(meta = (BindWidget), EditAnywhere, BlueprintReadWrite, Category = "Center")
	TObjectPtr<class UTextBlock> BarrelRemainingCountText;

	UPROPERTY(meta = (BindWidget), EditAnywhere, BlueprintReadWrite, Category = "Center")
	TObjectPtr<class UTextBlock> BarrelMaxCountText;

	UPROPERTY(meta = (BindWidget), EditAnywhere, BlueprintReadWrite, Category = "Center")
	TObjectPtr<class UTextBlock> BarrelText;

	UPROPERTY(meta = (BindWidget), EditAnywhere, BlueprintReadWrite, Category = "Center")
	TObjectPtr<class UCanvasPanel> SpikePanel;

	UPROPERTY(meta = (BindWidget), EditAnywhere, BlueprintReadWrite, Category = "Center")
	TObjectPtr<class UCanvasPanel> ClapPanel;

	UPROPERTY(meta = (BindWidget), EditAnywhere, BlueprintReadWrite, Category = "Center")
	TObjectPtr<class UCanvasPanel> BarrelPanel;

	// 가이드 영상 ----------------------------------------
	UPROPERTY(meta = (BindWidget), EditAnywhere, BlueprintReadWrite, Category = "Guide")
	TObjectPtr<class UImage> GuideVideo;

	UPROPERTY(meta = (BindWidget), EditAnywhere, BlueprintReadWrite, Category = "Guide")
	TObjectPtr<class UImage> GuideOutline;

private:
	float TargetAngle = 0.f;
	ETrapType CurrentType = ETrapType::None;

	UPROPERTY(EditDefaultsOnly, Category = "Data")
	TObjectPtr<class UDataTable> TrapCountTable;

	TArray<struct FTrapCount> TrapCountArray;

};
