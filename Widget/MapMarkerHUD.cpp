// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/MapMarkerHUD.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Image.h"
#include "WorldMapHUD.h"
#include "MiniMapHUD.h"
#include "Ping/MarkerComponent/TrapMarkerStruct.h"
#include "Ping/MarkerCOmponent/MapMarkerStruct.h"

UMapMarkerHUD::UMapMarkerHUD(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	// 데이터테이블 로드
	static ConstructorHelpers::FObjectFinder<UDataTable> DataTableFinder(
		TEXT("/Script/Engine.DataTable'/Game/Blueprints/Widget/Minimap/DT_TrapMarkerType.DT_TrapMarkerType'"));

	if (DataTableFinder.Succeeded())
	{
		TrapTable = DataTableFinder.Object;
	}

	//
	static ConstructorHelpers::FObjectFinder<UDataTable> DataTableFinder1(
		TEXT("/Script/Engine.DataTable'/Game/Blueprints/Widget/Minimap/DT_MapMarker.DT_MapMarker'"));

	if (DataTableFinder1.Succeeded())
	{
		ObjectTable = DataTableFinder1.Object;
	}
}

void UMapMarkerHUD::NativeConstruct()
{
	Super::NativeConstruct();
}


void UMapMarkerHUD::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	if(bTickEnabled)
	{
		Super::NativeTick(MyGeometry,InDeltaTime);
	}
}

void UMapMarkerHUD::SetImage(EMapObjectType Type)
{
	if (!IsValid(ObjectTable))
	{
		return;
	}

	TArray<FName> RowNames;
	RowNames = ObjectTable->GetRowNames();

	UTexture2D* texture = nullptr;
	for (const FName& RowName : RowNames)
	{
		FMapMarkerStruct* Data = ObjectTable->FindRow<FMapMarkerStruct>(RowName, FString());
		if (Data->Type == Type)
		{
			texture = Data->Texture;
			break;
		}
	}

	if (texture != nullptr)
	{
		Icon->SetBrushFromTexture(texture);
	}
}


void UMapMarkerHUD::SetImage(EMapTrapType Type)
{
	if (!IsValid(TrapTable))
	{
		return;
	}

	TArray<FName> RowNames;
	RowNames = TrapTable->GetRowNames();

	UTexture2D* texture = nullptr;
	for (const FName& RowName : RowNames)
	{
		FTrapMarkerStruct* Data = TrapTable->FindRow<FTrapMarkerStruct>(RowName, FString());
		if (Data->Type == Type)
		{
			texture = Data->Texture;
			break;
		}
	}

	Icon->SetBrushFromTexture(texture);
}


// 아이콘 초기 위치를 지정 (월드맵)
void UMapMarkerHUD::InitializeIcon(AActor* ActorRef, UWorldMapHUD* MapHUD)
{
	IconSlot = Cast<UCanvasPanelSlot>(Icon->Slot);
	WorldMapRef = MapHUD;

	FVector2D Pos = WorldMapRef->ConvertToMapCoord(ActorRef->GetActorLocation());
	IconSlot->SetPosition(Pos);
}

// 아이콘 초기 위치를 지정 (미니맵)
void UMapMarkerHUD::InitializeIcon(AActor* ActorRef, UMiniMapHUD* MiniMapHUD)
{
	IconSlot = Cast<UCanvasPanelSlot>(Icon->Slot);
	MiniMapRef = MiniMapHUD;

	FVector2D Pos = MiniMapRef->ConvertToMapCoord(ActorRef->GetActorLocation());
	IconSlot->SetPosition(Pos);
}


// 몬스터 같이 움직이는 액터들의 위치를 실시간으로 업데이트 해줌 (월드맵)
void UMapMarkerHUD::UpdateWorldmapIcon(AActor* ActorRef)
{
	if (IconSlot && IsValid(ActorRef))
	{
		FVector2D Pos = WorldMapRef->ConvertToMapCoord(ActorRef->GetActorLocation() + Offset);
		IconSlot->SetPosition(Pos);
	}
}

void UMapMarkerHUD::UpdateMinimapIcon(AActor* ActorRef)
{
	if (IconSlot && IsValid(ActorRef))
	{
		FVector2D ImgSize = MiniMapRef->MapImage->GetCachedGeometry().GetLocalSize();
		FVector2D Pos = MiniMapRef->ConvertToMapCoord(ActorRef->GetActorLocation() + Offset);
		if(Pos.X < 0 || ImgSize.X < Pos.X || Pos.Y < 0 || ImgSize.Y < Pos.Y)
		{
			Icon->SetVisibility(ESlateVisibility::Collapsed);
			bTickEnabled = false;
		}
		else
		{
			bTickEnabled = true;
			Icon->SetVisibility(ESlateVisibility::HitTestInvisible);
			IconSlot->SetPosition(Pos);
		}
	}
}

