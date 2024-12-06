// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MiniMapHUD.generated.h"

/**
 * 
 */
UCLASS()
class TRAPPERPROJECT_API UMiniMapHUD : public UUserWidget
{
	GENERATED_BODY()
	

public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	void UpdateMinimap(FVector LocalPlayerLocation, float LocalPlayerYaw, float LocalPlayerCameraYaw, bool bIsTeamPlayerConected, FVector TeamPlayerLocation = FVector(), float TeamPlayerYaw = 0);
	FVector2D ConvertToMapCoord(FVector ActorLocation);
	void ShowMonsterPath(bool bIsShow);

public:
	UPROPERTY(meta=(BindWidget), EditAnywhere, BlueprintReadWrite, Category = "Icon")
	TObjectPtr<class UImage> MapImage;

	UPROPERTY(meta=(BindWidget), EditAnywhere, BlueprintReadWrite, Category = "Icon")
	TObjectPtr<class UImage> PlayerIcon;

	UPROPERTY(meta=(BindWidget), EditAnywhere, BlueprintReadWrite, Category = "Icon")
	TObjectPtr<class UImage> PlayerSight;

	UPROPERTY(meta = (BindWidget), VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<class UImage> RemotePlayerIcon;  // 2 


	// 마커 추가를 위한 Panel ( Z-order 설정을 위함 )
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


    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Materials")
    UMaterialParameterCollection* MaterialParameterCollection;

private:
	UPROPERTY() class UCanvasPanelSlot* RemotePlayerSlot = nullptr;

	float TopBorder, LeftBorder, BottomBorder, RightBorder, Zoom;
	float Width, Height;
	FVector2D PlayerLocation;

	FTimerHandle FindPlayerTimer;
	UPROPERTY() UMaterialParameterCollectionInstance* CollectionInstance;
};
