// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Ping/PlayerPingType.h"
#include "Ping/MarkerComponent/MapMarkerType.h"
#include "MapMarkerHUD.generated.h"

/**
 * 
 */
UCLASS()
class TRAPPERPROJECT_API UMapMarkerHUD : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UMapMarkerHUD(const FObjectInitializer& ObjectInitializer);
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

public:
	UPROPERTY(meta = (BindWidget), EditAnywhere, BlueprintReadWrite, Category = "Description")
	TObjectPtr<class UCanvasPanel> Panel;

	UPROPERTY(meta = (BindWidget), VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<class UImage> Icon;


public:
	void SetImage(EMapObjectType Type);
	void SetImage(EMapTrapType Type);
	void SetOffset(FVector LocationOffset)	 { Offset = LocationOffset; }

	void InitializeIcon(AActor* ActorRef, class UWorldMapHUD* WorldmapHUD);
	void InitializeIcon(AActor* ActorRef, class UMiniMapHUD* MiniMapHUD);
	void UpdateWorldmapIcon(AActor* ActorRef);
	void UpdateMinimapIcon(AActor* ActorRef);

private:
	UPROPERTY() class UWorldMapHUD* WorldMapRef = nullptr;
	UPROPERTY() class UMiniMapHUD* MiniMapRef = nullptr;

	UPROPERTY() class UCanvasPanelSlot* IconSlot = nullptr;

	UPROPERTY() UDataTable* TrapTable = nullptr;
	UPROPERTY() UDataTable* ObjectTable = nullptr;

	FVector Offset = FVector();
	bool bTickEnabled = true;
};
