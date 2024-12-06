// Fill out your copyright notice in the Description page of Project Settings.


#include "Map/MapMechanics.h"

#include "EnhancedInputComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/CanvasPanel.h"
#include "Camera/CameraComponent.h"

#include "Widget/PlayerHUD.h"
#include "Widget/MapMarkerHUD.h"
#include "Widget/WorldMapHUD.h"
#include "Widget/MiniMapHUD.h"
#include "Widget/PingSelectHUD.h"

#include "Character/TrapperPlayer.h"
#include "Framework/TrapperPlayerController.h"
#include "Framework/TrapperGameMode.h"
#include "Framework/TrapperGameState.h"
#include "Ping/PingMechanics.h"

UMapMechanics::UMapMechanics()
{
	PrimaryComponentTick.bCanEverTick = false;
}


void UMapMechanics::BeginPlay()
{
	Super::BeginPlay();

	PlayerRef = Cast<ATrapperPlayer>(GetOwner());
	if(!PlayerRef->IsLocallyControlled())
	{
		return;
	}

	SetupInputComponent();
	PlayerController = Cast<ATrapperPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));

	GetWorld()->GetTimerManager().SetTimer(
		TryGetHUDTimer, this, &UMapMechanics::TryGetHUDRef, 0.03, true);

	GetWorld()->GetTimerManager().SetTimer(
		MiniMapUpdateTimer, this, &UMapMechanics::UpdateMiniMap, 0.03, true);
	
	TryGetHUDRef();
}


void UMapMechanics::EndPlay(EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	GetWorld()->GetTimerManager().ClearAllTimersForObject(this);

}

void UMapMechanics::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}


///-----------------------------------------------------------------------------------------
///											인풋 바인딩
///-----------------------------------------------------------------------------------------

void UMapMechanics::SetupInputComponent()
{
	AActor* OwnerActor = GetOwner();
	if(!IsValid(OwnerActor))
		return;

	APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if(!PC)
		return;

	UEnhancedInputComponent* UIC = Cast<UEnhancedInputComponent>(PC->InputComponent);
	if(IsValid(UIC))
	{
		UIC->BindAction(OpenMapAction, ETriggerEvent::Started, this, &UMapMechanics::SwitchWorldMap);
		UIC->BindAction(ClaseMapAction, ETriggerEvent::Completed, this, &UMapMechanics::CloseWorldMap);
		UIC->BindAction(PlacePingAction, ETriggerEvent::Started, this, &UMapMechanics::PlaceMarkerEnter);
		UIC->BindAction(PlacePingAction, ETriggerEvent::Completed, this, &UMapMechanics::PlaceMarkerExit);
		UIC->BindAction(PlacePingCancelAction, ETriggerEvent::Triggered, this, &UMapMechanics::PlaceMarkerCancel);
	}
}

void UMapMechanics::SwitchWorldMap()
{
	if(bIsOpenedWorldMap)
	{
		CloseWorldMap();
		return;
	}
	else
	{
		OpenWorldMap();
		return;
	}
}

void UMapMechanics::OpenWorldMap()
{
	if(!PlayerRef->IsLocallyControlled() || bIsOpenedWorldMap)
	{
		return;
	}

	bIsOpenedWorldMap = true;
	PlayerRef->BlockBow();
	PlayerRef->ShowCursor();

	if(IsValid(WorldMapRef))
	{
		WorldMapRef->OpenWorldMap();
		GetWorld()->GetTimerManager().SetTimer(
			WorldMapUpdateTimer, this, &UMapMechanics::UpdateWorldMap, 0.03, true);

		GetWorld()->GetTimerManager().SetTimer(
			MapUpdateOnceTimer, this, &UMapMechanics::UpdateWorldMapOnce, 0.03, false);
	}

	ATrapperGameState* GameState = GetWorld()->GetGameState<ATrapperGameState>();
	if (GameState->CurrentGameProgress == EGameProgress::Tutorial)
	{
		if (PlayerRef->HasAuthority())
		{
			GameState->OnQuestExecute.Broadcast(20, true);
		}
		else
		{
			GameState->OnQuestExecute.Broadcast(20, false);
		}
	}
}

void UMapMechanics::CloseWorldMap()
{
	if(!PlayerRef->IsLocallyControlled() || !bIsOpenedWorldMap)
	{
		return;
	}

	bIsOpenedWorldMap = false;
	PlayerRef->UnblockBow();
	PlayerRef->HideCursor();

	if(IsValid(WorldMapRef))
	{
		WorldMapRef->CloseWorldMap();
		GetWorld()->GetTimerManager().ClearTimer(WorldMapUpdateTimer);
	}

	WorldMapRef->PingSelectHUD->OnUnhovered(HoveredPingType);
	WorldMapRef->PingSelectHUD->OnHovered(ESelectPingType::None);
	HoveredPingType = ESelectPingType::None;
	PlaceMarkerExit();
}

void UMapMechanics::PlaceMarkerEnter()
{
	if(!PlayerRef->IsLocallyControlled() 
		|| !bIsOpenedWorldMap
		|| bIsPlacePingMode )
	{
		return;
	}
	
	bIsPlacePingMode = true;
	if(IsValid(WorldMapRef))
	{
		ClickCursorPosition = PlayerRef->GetCursorPosition();
		if(WorldMapRef->IsInWorldmap(ClickCursorPosition))
		{
			WorldMapRef->PingSelectHUD->SetVisibility(ESlateVisibility::Visible);
			WorldMapRef->SetPingSelectHUDPosition(ClickCursorPosition / PlayerRef->GetViewportScale());

			GetWorld()->GetTimerManager().SetTimer(
				CheckOnHoveredTimer, this, &UMapMechanics::CheckOnHovered, 0.03, true);
		}

		WorldMapRef->PingSelectHUD->OnUnhovered(HoveredPingType);
		WorldMapRef->PingSelectHUD->OnHovered(ESelectPingType::None);
		HoveredPingType = ESelectPingType::None;
	}
}

void UMapMechanics::PlaceMarkerExit()
{
	if(!PlayerRef->IsLocallyControlled()
		|| !bIsOpenedWorldMap
		|| !bIsPlacePingMode)
	{
		return;
	}

	bIsPlacePingMode = false;
	if(IsValid(WorldMapRef))
	{
		WorldMapRef->PingSelectHUD->SetVisibility(ESlateVisibility::Collapsed);
	}
	GetWorld()->GetTimerManager().ClearTimer(CheckOnHoveredTimer);

	if(    HoveredPingType != ESelectPingType::None
		&& IsValid(PlayerRef->PingMechanics))
	{
		FVector2D Location2D = WorldMapRef->ConvertToWorldCoord(ClickCursorPosition);
		FVector StartLocation = FVector(Location2D.X, Location2D.Y, 20000);
		FVector EndLocation = FVector(Location2D.X, Location2D.Y, 500);
		FHitResult HitResult;

		bool bHit = GetWorld()->LineTraceSingleByChannel(
			HitResult,
			StartLocation,
			EndLocation,
			ECC_Visibility);

		if(bHit)
		{
			PlayerRef->PingMechanics->SpawnPing(HoveredPingType, HitResult.Location);
		}
		else
		{
			FVector Location = FVector(Location2D.X, Location2D.Y, 10);
			PlayerRef->PingMechanics->SpawnPing(HoveredPingType, Location);
		}
	}
}

void UMapMechanics::PlaceMarkerCancel()
{
	if(!PlayerRef->IsLocallyControlled()
		|| !bIsOpenedWorldMap
		|| !bIsPlacePingMode)
	{
		return;
	}

	bIsPlacePingMode = false;
	if(IsValid(WorldMapRef))
	{
		WorldMapRef->PingSelectHUD->SetVisibility(ESlateVisibility::Collapsed);
	}
	GetWorld()->GetTimerManager().ClearTimer(CheckOnHoveredTimer);
}

///-----------------------------------------------------------------------------------------
///										 타이머 함수
///-----------------------------------------------------------------------------------------


void UMapMechanics::UpdateWorldMap()
{
	if(IsValid(WorldMapRef))
	{
		// 플레이어 아이콘
		WorldMapRef->UpdatePlayerIcon();

		// 동적 아이콘(몬스터?)
		for(auto& Elem : DynamicMarkers)
		{
			if(IsValid(Elem.Value.ActorRef) && IsValid(Elem.Value.HUD))
			{
				Elem.Value.HUD->UpdateWorldmapIcon(Elem.Value.ActorRef);
			}
		}
	}
}


void UMapMechanics::UpdateMiniMap()
{
	if(IsValid(MiniMapRef) && IsValid(PlayerRef))
	{
		// 팀 플레이어가 없다면 찾기
		if(!IsValid(TeamPlayerRef))
		{
			TArray<AActor*> OutActors;
			UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATrapperPlayer::StaticClass(), OutActors);

			for(const auto& Pawn : OutActors)
			{
				ATrapperPlayer* TrapperPlayer = Cast<ATrapperPlayer>(Pawn);
				if(!TrapperPlayer)
				{
					continue;
				}

				if(!TrapperPlayer->IsLocallyControlled())
				{
					TeamPlayerRef = TrapperPlayer;
				}
			}
		}

		// 플레이어 아이콘
		if(IsValid(TeamPlayerRef) && !bIsClientLogout)
		{
			MiniMapRef->UpdateMinimap(
				PlayerRef->GetActorLocation(),
				PlayerRef->GetActorRotation().Yaw,
				PlayerRef->Camera->GetComponentRotation().Yaw,
				true,
				TeamPlayerRef->GetActorLocation(),
				TeamPlayerRef->GetActorRotation().Yaw);
		}
		else
		{
			MiniMapRef->UpdateMinimap(
				PlayerRef->GetActorLocation(), 
				PlayerRef->GetActorRotation().Yaw,
				PlayerRef->Camera->GetComponentRotation().Yaw,
				false);
		}

		// 다른 아이콘들
		for(auto& Elem : MinimapMarkers)
		{
			if(IsValid(Elem.Value.ActorRef) && IsValid(Elem.Value.HUD))
			{
				Elem.Value.HUD->UpdateMinimapIcon(Elem.Value.ActorRef);
			}
		}
	}
}


void UMapMechanics::UpdateWorldMapOnce()
{
	for(auto& Elem : StaticMarkers)
	{
		if(IsValid(Elem.Value.ActorRef) && IsValid(Elem.Value.HUD))
		{
			Elem.Value.HUD->UpdateWorldmapIcon(Elem.Value.ActorRef);
			Elem.Value.HUD->SetVisibility(ESlateVisibility::Visible);
		}
	}

	for(auto& Elem : DynamicMarkers)
	{
		if(IsValid(Elem.Value.ActorRef) && IsValid(Elem.Value.HUD))
		{
			Elem.Value.HUD->SetVisibility(ESlateVisibility::Visible);
		}
	}
}


void UMapMechanics::AddMarkerInit()
{
	for(int32 i = 0; i < ObjWaitArray.Num(); i++)
	{
		AddObjectMarker(ObjWaitArray[i].ActorRef, ObjWaitArray[i].ObjectID, ObjWaitArray[i].ObjectType);
	}
	ObjWaitArray.Empty();

	for(int32 i = 0; i < TrapWaitArray.Num(); i++)
	{
		AddTrapMarker(TrapWaitArray[i].ActorRef, TrapWaitArray[i].ObjectID, TrapWaitArray[i].TrapType);
	}
	TrapWaitArray.Empty();
}


void UMapMechanics::TryGetHUDRef()
{
	if(IsValid(PlayerController) && IsValid(PlayerController->PlayerHudRef))
	{
		WorldMapRef = PlayerController->WorldMapHUDRef;
		MiniMapRef = PlayerController->PlayerHudRef->MiniMap;

		if(	   IsValid(WorldMapMarkerHUDClass) && IsValid(WorldMapRef)
			&& IsValid(MinimapMarkerHUDClass)  && IsValid(MiniMapRef))
		{
			AddMarkerInit();
			GetWorld()->GetTimerManager().ClearTimer(TryGetHUDTimer);
		}
	}
}


void UMapMechanics::CheckOnHovered()
{
	if(!IsValid(WorldMapRef) || !IsValid(WorldMapRef->PingSelectHUD))
	{
		return;
	}
	
	FVector2D MousePosition;
	GEngine->GameViewport->GetMousePosition(MousePosition);

	float Distance = FVector2D::Distance(ClickCursorPosition, MousePosition);
	float Size = WorldMapRef->PingSelectHUD->GetSize().X;
	float Radius = Size * 0.2;

	ESelectPingType CurrentHoveredPingType = ESelectPingType::None;
	if(Distance < Radius)
	{
		CurrentHoveredPingType = ESelectPingType::E_Default;
	}
	else
	{
		// 마우스 각도 계산
		FVector2D Normal = MousePosition - ClickCursorPosition;
		Normal.Normalize();

		float Radians = FMath::Atan2(Normal.Y, Normal.X);
		float Degrees = FMath::RadiansToDegrees(Radians); //-180 ~ 180
		Degrees += 180;

		if(45 < Degrees && Degrees <= 135)
			CurrentHoveredPingType = ESelectPingType::E_TrapInstall;
		else if(135 < Degrees && Degrees <= 225)
			CurrentHoveredPingType = ESelectPingType::E_Assemble;
		else if(225 < Degrees && Degrees <= 315)
			CurrentHoveredPingType = ESelectPingType::E_Warning;
		else
			CurrentHoveredPingType = ESelectPingType::E_TrapActivate;
	}

	// 바뀌었을 때 HUD 업데이트
	if(CurrentHoveredPingType != HoveredPingType)
	{
		WorldMapRef->PingSelectHUD->OnHovered(CurrentHoveredPingType);
		WorldMapRef->PingSelectHUD->OnUnhovered(HoveredPingType);
	}

	HoveredPingType = CurrentHoveredPingType;
}


///-----------------------------------------------------------------------------------------
///									외부에서 호출하는 함수 
///-----------------------------------------------------------------------------------------

void UMapMechanics::AddObjectMarker(AActor* ActorRef, int32 ID, EMapObjectType ObjectType, FVector LocationOffset)
{
	if(	   !IsValid(WorldMapMarkerHUDClass) || !IsValid(WorldMapRef)
		|| !IsValid(MinimapMarkerHUDClass) || !IsValid(MiniMapRef) )
	{
		FObjIconInitData data = FObjIconInitData(ActorRef, ID, ObjectType, LocationOffset);
		ObjWaitArray.Add(data);

		return;
	}

	//이미 있는 액터가 또 들어왔을 때 처리
	if(StaticMarkers.Contains(ID) || DynamicMarkers.Contains(ID))
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, FString::Printf(
			TEXT("MapMechanics::AddObjectMarker 이미 만든걸 또 만들려고 하고있음!!")));
		return;
	}

	UMapMarkerHUD* WorldIcon = CreateWidget<UMapMarkerHUD>(PlayerController, WorldMapMarkerHUDClass);
	UMapMarkerHUD* MiniIcon = CreateWidget<UMapMarkerHUD>(PlayerController, MinimapMarkerHUDClass);

	if(!IsValid(WorldIcon) || !IsValid(MiniIcon))
	{
		return;
	}

	switch(ObjectType)
	{
	case EMapObjectType::Monster:
		WorldMapRef->MonsterPanel->AddChild(WorldIcon);
		DynamicMarkers.Add(ID, FMapIconData(ActorRef, WorldIcon));
		MiniMapRef->MonsterPanel->AddChild(MiniIcon);
		MinimapMarkers.Add(ID, FMapIconData(ActorRef, MiniIcon));
		break;

	case EMapObjectType::Pillar:
	case EMapObjectType::LockedPillar:
		WorldMapRef->PillarPanel->AddChild(WorldIcon);
		StaticMarkers.Add(ID, FMapIconData(ActorRef, WorldIcon));
		MiniMapRef->PillarPanel->AddChild(MiniIcon);
		MinimapMarkers.Add(ID, FMapIconData(ActorRef, MiniIcon));
		break;

	case EMapObjectType::Temple:
		WorldMapRef->TemplePanel->AddChild(WorldIcon);
		StaticMarkers.Add(ID, FMapIconData(ActorRef, WorldIcon));
		MiniMapRef->TemplePanel->AddChild(MiniIcon);
		MinimapMarkers.Add(ID, FMapIconData(ActorRef, MiniIcon));
		break;

	case EMapObjectType::Player:
		break;

	default:
		WorldMapRef->PingPanel->AddChild(WorldIcon);
		StaticMarkers.Add(ID, FMapIconData(ActorRef, WorldIcon));
		MiniMapRef->PingPanel->AddChild(MiniIcon);
		MinimapMarkers.Add(ID, FMapIconData(ActorRef, MiniIcon));
		break;
	}

	WorldIcon->SetVisibility(ESlateVisibility::Visible);
	WorldIcon->SetImage(ObjectType);
	WorldIcon->InitializeIcon(ActorRef, WorldMapRef);
	WorldIcon->SetOffset(LocationOffset);

	MiniIcon->SetVisibility(ESlateVisibility::Visible);
	MiniIcon->SetImage(ObjectType);
	MiniIcon->InitializeIcon(ActorRef, MiniMapRef);
	WorldIcon->SetOffset(LocationOffset);

	return;
}

void UMapMechanics::AddTrapMarker(AActor* ActorRef, int32 ID, EMapTrapType ObjectType, FVector LocationOffset)
{
	if(!IsValid(WorldMapMarkerHUDClass) || !IsValid(WorldMapRef)
		|| !IsValid(MinimapMarkerHUDClass) || !IsValid(MiniMapRef))
	{
		TrapWaitArray.Add(FTrapIconInitData(ActorRef, ID, ObjectType, LocationOffset));
		return;
	}

	// 월드맵
	UMapMarkerHUD* WorldIcon = CreateWidget<UMapMarkerHUD>(PlayerController, WorldMapMarkerHUDClass);
	if(IsValid(WorldIcon))
	{
		WorldIcon->SetVisibility(ESlateVisibility::Visible);
		WorldMapRef->TrapPanel->AddChild(WorldIcon);
		StaticMarkers.Add(ID, FMapIconData(ActorRef, WorldIcon));
		WorldIcon->SetImage(ObjectType);
		WorldIcon->InitializeIcon(ActorRef, WorldMapRef);
		WorldIcon->SetOffset(LocationOffset);
	}

	// 미니맵
	UMapMarkerHUD* MiniIcon = CreateWidget<UMapMarkerHUD>(PlayerController, MinimapMarkerHUDClass);
	if(IsValid(MiniIcon))
	{
		MiniIcon->SetVisibility(ESlateVisibility::Visible);
		MiniMapRef->TrapPanel->AddChild(MiniIcon);
		MinimapMarkers.Add(ID, FMapIconData(ActorRef, MiniIcon));
		MiniIcon->SetImage(ObjectType);
		MiniIcon->InitializeIcon(ActorRef, MiniMapRef);
		MiniIcon->SetOffset(LocationOffset);
	}
}

void UMapMechanics::ChangeTrapMarker(int32 ID, EMapTrapType TrapType)
{
	if(!StaticMarkers.Contains(ID) || !MinimapMarkers.Contains(ID))
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, FString::Printf(
			TEXT("MapMechanics::ChangeTrapMarker 그럴리가 없는데????")));
		return;
	}

	UMapMarkerHUD* HUD = StaticMarkers[ID].HUD;
	if(IsValid(HUD))
		HUD->SetImage(TrapType);

	HUD = MinimapMarkers[ID].HUD;
	if(IsValid(HUD))
		HUD->SetImage(TrapType);
}

void UMapMechanics::ChangeObjectMarker(int32 ID, EMapObjectType ObjectType)
{
	if(!StaticMarkers.Contains(ID) || !MinimapMarkers.Contains(ID))
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Magenta, FString::Printf(
			TEXT("MapMechanics::ChangeObjectMarker 그럴리가 없는데????")));
		return;
	}

	UMapMarkerHUD* HUD = StaticMarkers[ID].HUD;
	if(IsValid(HUD))
	{
		HUD->SetImage(ObjectType);
	}

	HUD = MinimapMarkers[ID].HUD;
	if(IsValid(HUD))
	{
		HUD->SetImage(ObjectType);
	}
}

void UMapMechanics::DeleteMarker(int32 ID)
{
	// 월드맵 -----------------------------------------------
	UMapMarkerHUD* MapMarker = nullptr;
	if(StaticMarkers.Contains(ID))
	{
		MapMarker = StaticMarkers[ID].HUD;
		StaticMarkers.Remove(ID);
	}
	else if(DynamicMarkers.Contains(ID))
	{
		MapMarker = DynamicMarkers[ID].HUD;
		DynamicMarkers.Remove(ID);
	}

	if(IsValid(MapMarker))
	{
		MapMarker->RemoveFromParent();
	}

	// 미니맵 -----------------------------------------------
	MapMarker = nullptr;
	if(MinimapMarkers.Contains(ID))
	{
		MapMarker = MinimapMarkers[ID].HUD;
		MinimapMarkers.Remove(ID);
	}

	if(IsValid(MapMarker))
	{
		MapMarker->RemoveFromParent();
	}
}

void UMapMechanics::SetOffset(int32 ID, FVector Offset)
{
	UMapMarkerHUD* MapMarker = nullptr;
	if(StaticMarkers.Contains(ID))
	{
		MapMarker = StaticMarkers[ID].HUD;
	}
	else if(DynamicMarkers.Contains(ID))
	{
		MapMarker = DynamicMarkers[ID].HUD;
	}

	if(IsValid(MapMarker))
	{
		MapMarker->SetOffset(Offset);
	}

	MapMarker = nullptr;
	if(MinimapMarkers.Contains(ID))
	{
		MapMarker = MinimapMarkers[ID].HUD;
	}
	if(IsValid(MapMarker))
	{
		MapMarker->SetOffset(Offset);
	}
}

void UMapMechanics::ShowMonsterPath(bool arg)
{
	if(IsValid(WorldMapRef))
	{
		WorldMapRef->ShowMonsterPath(arg);
	}

	if(IsValid(MiniMapRef))
	{
		MiniMapRef->ShowMonsterPath(arg);
	}
}
