// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/MiniMapHUD.h"
#include "Components/Image.h"
#include "Components/CanvasPanelSlot.h"

#include "Kismet/GameplayStatics.h"
#include "Character/TrapperPlayer.h"
#include "Camera/CameraComponent.h"

#include "Materials/MaterialParameterCollection.h"
#include "Materials/MaterialParameterCollectionInstance.h"


void UMiniMapHUD::NativeConstruct()
{
	Super::NativeConstruct();

	RemotePlayerSlot = Cast<UCanvasPanelSlot>(RemotePlayerIcon->Slot);

	// 머테리얼 파라미터 컬렉션을 로드
	if(MaterialParameterCollection)
	{
		CollectionInstance = GetWorld()->GetParameterCollectionInstance(MaterialParameterCollection);

		CollectionInstance->GetScalarParameterValue(FName("Zoom"), Zoom);
		CollectionInstance->GetScalarParameterValue(FName("TopBorder"), TopBorder);
		CollectionInstance->GetScalarParameterValue(FName("BottomBorder"), BottomBorder);
		CollectionInstance->GetScalarParameterValue(FName("LeftBorder"), LeftBorder);
		CollectionInstance->GetScalarParameterValue(FName("RightBorder"), RightBorder);

		Height = BottomBorder - TopBorder;
		Width = RightBorder - LeftBorder;
	}
}


void UMiniMapHUD::NativeDestruct()
{
	Super::NativeDestruct();
	GetWorld()->GetTimerManager().ClearAllTimersForObject(this);
}


void UMiniMapHUD::UpdateMinimap(FVector LocalPlayerLocation, float LocalPlayerYaw, float LocalPlayerCameraYaw, bool bIsTeamPlayerConected, FVector TeamPlayerLocation, float TeamPlayerYaw)
{
	PlayerSight->SetRenderTransformAngle(LocalPlayerCameraYaw);
	PlayerIcon->SetRenderTransformAngle(LocalPlayerYaw);
	PlayerLocation = FVector2D(LocalPlayerLocation.X, LocalPlayerLocation.Y);

	if(CollectionInstance)
	{
		CollectionInstance->SetScalarParameterValue(FName("PlayerX"), LocalPlayerLocation.X);
		CollectionInstance->SetScalarParameterValue(FName("PlayerY"), LocalPlayerLocation.Y);
	}

	if(bIsTeamPlayerConected)
	{
		RemotePlayerIcon->SetVisibility(ESlateVisibility::HitTestInvisible);
		RemotePlayerIcon->SetRenderTransformAngle(TeamPlayerYaw);
		RemotePlayerSlot->SetPosition(ConvertToMapCoord(TeamPlayerLocation));
	}
	else
	{
		RemotePlayerIcon->SetVisibility(ESlateVisibility::Collapsed);
	}
}

FVector2D UMiniMapHUD::ConvertToMapCoord(FVector Location)
{
	FVector2D ImgSize = MapImage->GetCachedGeometry().GetLocalSize();

	float rLeft = PlayerLocation.X - Width * Zoom * 0.5f;
	float rRight = PlayerLocation.X + Width * Zoom * 0.5f;
	float rTop = PlayerLocation.Y - Height * Zoom * 0.5f;
	float rBottom = PlayerLocation.Y + Height * Zoom * 0.5f;

	float NormalizeX = (Location.X - rLeft) / (rRight - rLeft);
	float NormalizeY = (Location.Y - rTop) / (rBottom - rTop);

	return FVector2D(NormalizeY * ImgSize.Y, (1 - NormalizeX) * ImgSize.X);
}


void UMiniMapHUD::ShowMonsterPath(bool bIsShow)
{
	if(!CollectionInstance)
	{
		return;
	}

	if(bIsShow)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Cyan, FString::Printf(TEXT("bIsShow false")));
		CollectionInstance->SetScalarParameterValue(FName("ShowMonsterPath"), 1.f);
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Cyan, FString::Printf(TEXT("bIsShow false")));
		CollectionInstance->SetScalarParameterValue(FName("ShowMonsterPath"), 0.f);
	}
}
