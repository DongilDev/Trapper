// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/WorldMapHUD.h"

#include "Components/Image.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Blueprint/SlateBlueprintLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

#include "Character/TrapperPlayer.h"

#include "Materials/MaterialParameterCollection.h"
#include "Materials/MaterialParameterCollectionInstance.h"

#include "Widget/PingSelectHUD.h"

void UWorldMapHUD::NativeConstruct()
{
	Super::NativeConstruct();

	// 머테리얼 파라미터 컬렉션을 로드
	if(MaterialParameterCollection)
	{
		CollectionInstance = GetWorld()->GetParameterCollectionInstance(MaterialParameterCollection);

		CollectionInstance->GetScalarParameterValue(FName("TopBorder"), TopBorder);
		CollectionInstance->GetScalarParameterValue(FName("BottomBorder"), BottomBorder);
		CollectionInstance->GetScalarParameterValue(FName("LeftBorder"), LeftBorder);
		CollectionInstance->GetScalarParameterValue(FName("RightBorder"), RightBorder);
	}
}

void UWorldMapHUD::OpenWorldMap()
{
	SetVisibility(ESlateVisibility::Visible);
	FindPlayer();

	LocalPlayerSlot = Cast<UCanvasPanelSlot>(LocalPlayerImg->Slot);
	LocalPlayerSightSlot = Cast<UCanvasPanelSlot>(LocalPlayerSight->Slot);
	RemotePlayerSlot = Cast<UCanvasPanelSlot>(RemotePlayerImg->Slot);
	PingSelectSlot = Cast<UCanvasPanelSlot>(PingSelectHUD->Slot);
}

void UWorldMapHUD::CloseWorldMap()
{
	SetVisibility(ESlateVisibility::Collapsed);
}

void UWorldMapHUD::UpdatePlayerIcon()
{
	if(IsValid(Player))
	{
		LocalPlayerImg->SetVisibility(ESlateVisibility::HitTestInvisible);
		LocalPlayerImg->SetRenderTransformAngle(Player->GetActorRotation().Yaw);
		LocalPlayerSight->SetRenderTransformAngle(Player->Camera->GetComponentRotation().Yaw);

		if(LocalPlayerSlot)
		{
			LocalPlayerSlot->SetPosition(ConvertToMapCoord(Player->GetActorLocation()));
			LocalPlayerSightSlot->SetPosition(ConvertToMapCoord(Player->GetActorLocation()));
		}
	}
	else
	{
		LocalPlayerImg->SetVisibility(ESlateVisibility::Collapsed);
	}

	if(IsValid(TeamPlayer))
	{
		RemotePlayerImg->SetVisibility(ESlateVisibility::HitTestInvisible);

		FWidgetTransform transform = RemotePlayerImg->GetRenderTransform();
		transform.Angle = TeamPlayer->GetActorRotation().Yaw;
		RemotePlayerImg->SetRenderTransform(transform);
		
		if(RemotePlayerSlot)
		{
			RemotePlayerSlot->SetPosition(ConvertToMapCoord(TeamPlayer->GetActorLocation()));
		}
	}
	else
	{
		RemotePlayerImg->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UWorldMapHUD::FindPlayer()
{
	if(IsValid(Player) && IsValid(TeamPlayer)) return;

	TArray<AActor*> OutActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATrapperPlayer::StaticClass(), OutActors);

	for(const auto& Pawn : OutActors)
	{
		ATrapperPlayer* TrapperPlayer = Cast<ATrapperPlayer>(Pawn);
		if(!TrapperPlayer) continue;

		if(TrapperPlayer->IsLocallyControlled())
		{
			Player = TrapperPlayer;
		}
		else
		{
			TeamPlayer = TrapperPlayer;
		}
	}
}


FVector2D UWorldMapHUD::ConvertToMapCoord(FVector Location)
{
	FVector2D ImgSize = Worldmap->GetCachedGeometry().GetLocalSize();

	float NormalizeX = (Location.X - LeftBorder) / (RightBorder - LeftBorder);
	float NormalizeY = (Location.Y - TopBorder) / (BottomBorder - TopBorder);

	return FVector2D(NormalizeY * ImgSize.Y, (1 - NormalizeX) * ImgSize.X);
}


FVector2D UWorldMapHUD::ConvertToWorldCoord(FVector2D Location)
{
	float ViewportScale = UWidgetLayoutLibrary::GetViewportScale(GetWorld());
	FVector2D pixelPos, viewportPos;

	FGeometry Geometry = Worldmap->GetCachedGeometry();
	USlateBlueprintLibrary::LocalToViewport(GetWorld(), Geometry, FVector2D(0, 0), pixelPos, viewportPos);

	FVector2D TopLeft = viewportPos;
	Location /= ViewportScale;
	Location -= TopLeft;

	FVector2D ImgSize = Geometry.GetLocalSize();

	// 역변환: WorldLocation에서 ImgSize를 통해 Normalize된 좌표로 변환
	float NormalizeX = 1 - (Location.Y / ImgSize.X);
	float NormalizeY = Location.X / ImgSize.Y;

	// Normalize된 좌표를 실제 월드 좌표로 변환
	float WorldX = NormalizeX * (RightBorder - LeftBorder) + LeftBorder;
	float WorldY = NormalizeY * (BottomBorder - TopBorder) + TopBorder;

	return FVector2D(WorldX, WorldY);
}

void UWorldMapHUD::SetPingSelectHUDPosition(FVector2D ScreenLocation)
{
	PingSelectSlot->SetPosition(ScreenLocation);
}


bool UWorldMapHUD::IsInWorldmap(FVector2D ScreenLocation)
{
	float ViewportScale = UWidgetLayoutLibrary::GetViewportScale(GetWorld());
	FVector2D pixelPos, viewportPos;
	
	FGeometry Geometry = Worldmap->GetCachedGeometry();
	USlateBlueprintLibrary::LocalToViewport(GetWorld(), Geometry, FVector2D(0, 0), pixelPos, viewportPos);

	FVector2D TopLeft = viewportPos;
	FVector2D BottomRight = TopLeft + FVector2D(850, 850);
	ScreenLocation /= ViewportScale;

	return 		
		ScreenLocation.X >= TopLeft.X && ScreenLocation.X <= BottomRight.X &&
		ScreenLocation.Y >= TopLeft.Y && ScreenLocation.Y <= BottomRight.Y;
}

void UWorldMapHUD::ShowMonsterPath(bool arg)
{
	if(arg)
	{
		MonsterPath->SetVisibility(ESlateVisibility::HitTestInvisible);
	}
	else
	{
		MonsterPath->SetVisibility(ESlateVisibility::Collapsed);
	}
}
