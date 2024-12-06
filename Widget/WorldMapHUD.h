// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WorldMapHUD.generated.h"

/**
 *
 */
UCLASS()
class TRAPPERPROJECT_API UWorldMapHUD : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

public:
	UPROPERTY(meta = (BindWidget), EditAnywhere, BlueprintReadWrite)
	TObjectPtr<class UPingSelectHUD> PingSelectHUD;

	UPROPERTY(meta = (BindWidget), EditAnywhere, BlueprintReadWrite)
	TObjectPtr<class UCanvasPanel> MapPanel;

	UPROPERTY(meta = (BindWidget), VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<class UImage> Background;

	UPROPERTY(meta = (BindWidget), VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<class UImage> Worldmap;

	UPROPERTY(meta = (BindWidget), VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<class UImage> MonsterPath;  

	UPROPERTY(meta = (BindWidget), VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<class UImage> LocalPlayerImg;  // 1

	UPROPERTY(meta = (BindWidget), VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<class UImage> LocalPlayerSight;  // 1

	UPROPERTY(meta = (BindWidget), VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<class UImage> RemotePlayerImg;  // 2 


	// 마커 추가를 위한 Pannel ( Z-order 설정을 위함 )
	UPROPERTY(meta = (BindWidget), EditAnywhere, BlueprintReadWrite)
	TObjectPtr<class UCanvasPanel> PingPanel; // 3

	UPROPERTY(meta = (BindWidget), EditAnywhere, BlueprintReadWrite)
	TObjectPtr<class UCanvasPanel> MonsterPanel; // 4

	UPROPERTY(meta = (BindWidget), EditAnywhere, BlueprintReadWrite)
	TObjectPtr<class UCanvasPanel> PillarPanel; // 5

	UPROPERTY(meta = (BindWidget), EditAnywhere, BlueprintReadWrite)
	TObjectPtr<class UCanvasPanel> TemplePanel; // 6

	UPROPERTY(meta = (BindWidget), EditAnywhere, BlueprintReadWrite)
	TObjectPtr<class UCanvasPanel> TrapPanel; // 7

public:
	void OpenWorldMap();
	void CloseWorldMap();
	void UpdatePlayerIcon();

	FVector2D ConvertToMapCoord(FVector Location);
	FVector2D ConvertToWorldCoord(FVector2D Location);
	void SetPingSelectHUDPosition(FVector2D ScreenLocation);
	bool IsInWorldmap(FVector2D ScreenLocation);
	void ShowMonsterPath(bool arg);

private:
	void FindPlayer();

private:
	TObjectPtr<class ATrapperPlayer> Player;
	TObjectPtr<class ATrapperPlayer> TeamPlayer;

	float TopBorder, LeftBorder, BottomBorder, RightBorder;

	UPROPERTY() class UCanvasPanelSlot* LocalPlayerSlot = nullptr;
	UPROPERTY() class UCanvasPanelSlot* LocalPlayerSightSlot = nullptr;
	UPROPERTY() class UCanvasPanelSlot* RemotePlayerSlot = nullptr;
	UPROPERTY() class UCanvasPanelSlot* PingSelectSlot = nullptr;


public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Materials")
    UMaterialParameterCollection* MaterialParameterCollection;
	UPROPERTY() UMaterialParameterCollectionInstance* CollectionInstance;
};
