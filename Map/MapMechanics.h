// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Ping/MarkerComponent/MapMarkerType.h"
#include "Ping/PlayerPingType.h"
#include "MapMechanics.generated.h"


USTRUCT()
struct FObjIconInitData
{
	GENERATED_BODY()

	AActor* ActorRef;
	int32 ObjectID;
	EMapObjectType ObjectType;
	FVector LocationOffset;

	FObjIconInitData()
		: ActorRef(nullptr), ObjectID(0), ObjectType(EMapObjectType::None), LocationOffset(FVector())
	{
	}

	FObjIconInitData(AActor* actor, int32 ID, EMapObjectType type, FVector Offset = FVector())
		: ActorRef(actor), ObjectID(ID), ObjectType(type), LocationOffset(Offset)
	{
	}
};

USTRUCT()
struct FTrapIconInitData
{
	GENERATED_BODY()

	AActor* ActorRef;
	int32 ObjectID;
	EMapTrapType TrapType;
	FVector LocationOffset;

	FTrapIconInitData()
		: ActorRef(nullptr), ObjectID(0), TrapType(EMapTrapType::None), LocationOffset(FVector())
	{
	}

	FTrapIconInitData(AActor* actor, int32 ID, EMapTrapType type, FVector Offset = FVector())
		: ActorRef(actor), ObjectID(ID), TrapType(type), LocationOffset(Offset)
	{
	}
};

USTRUCT()
struct FMapIconData
{
	GENERATED_BODY()

	AActor* ActorRef;
	TObjectPtr<class UMapMarkerHUD> HUD;

	FMapIconData()
		: ActorRef(nullptr), HUD()
	{
	}

	FMapIconData(AActor* actor, TObjectPtr<class UMapMarkerHUD> HUDRef)
		: ActorRef(actor), HUD(HUDRef)
	{
	}
};


UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class TRAPPERPROJECT_API UMapMechanics : public UActorComponent
{
	GENERATED_BODY()

public:
	UMapMechanics();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(EEndPlayReason::Type EndPlayReason) override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/// 멤버 함수 ---------------------------------
public:
	// ----- Getter -----
	bool IsOpenedWorldMap() const { return bIsOpenedWorldMap; }


	// ----- 외부에서 호출되는 함수 -----
	void AddObjectMarker(AActor* ActorRef, int32 ID, EMapObjectType ObjectType, FVector LocationOffset = FVector() );
	void AddTrapMarker(AActor* ActorRef, int32 ID, EMapTrapType ObjectType, FVector LocationOffset = FVector());
	void ChangeTrapMarker(int32 ID, EMapTrapType TrapType);
	void ChangeObjectMarker(int32 ID, EMapObjectType ObjectType);
	void DeleteMarker(int32 ID);
	void SetOffset(int32 ID, FVector Offset);
	void ShowMonsterPath(bool arg);

	FORCEINLINE class UWorldMapHUD* GetWorldMapHUD() const { return WorldMapRef; }

	// ----- 인풋 바인딩 -----
private:
	void SetupInputComponent();
	void SwitchWorldMap();
	void OpenWorldMap();
	void PlaceMarkerEnter();
	void PlaceMarkerExit();
public:
	void CloseWorldMap();
	void PlaceMarkerCancel();

private:
	// ----- 업데이트 -----
	FTimerHandle WorldMapUpdateTimer;
	FTimerHandle MiniMapUpdateTimer;
	FTimerHandle MapUpdateOnceTimer;
	FTimerHandle TryGetHUDTimer;
	FTimerHandle CheckOnHoveredTimer;

	void UpdateWorldMap();
	void UpdateMiniMap();
	void UpdateWorldMapOnce();
	void AddMarkerInit();
	void TryGetHUDRef();
	void CheckOnHovered();

	/// 멤버 변수 ---------------------------------
public:
	bool bIsClientLogout = false;

public:
	// ---- 인풋 -----
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	class UInputAction* OpenMapAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	class UInputAction* ClaseMapAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	class UInputAction* PlacePingAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	class UInputAction* PlacePingCancelAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "HUD")
	TSubclassOf<class UMapMarkerHUD> WorldMapMarkerHUDClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "HUD")
	TSubclassOf<class UMapMarkerHUD> MinimapMarkerHUDClass;

private:
	bool bIsOpenedWorldMap = false;
	bool bIsPlacePingMode = false;

	TObjectPtr<class ATrapperPlayerController> PlayerController;
	TObjectPtr<class ATrapperPlayer> PlayerRef;
	TObjectPtr<class ATrapperPlayer> TeamPlayerRef;
	TObjectPtr<class UWorldMapHUD> WorldMapRef;
	TObjectPtr<class UMiniMapHUD> MiniMapRef;

	TMap<int32, FMapIconData> StaticMarkers;
	TMap<int32, FMapIconData> DynamicMarkers;
	TMap<int32, FMapIconData> MinimapMarkers;

	TArray<FObjIconInitData> ObjWaitArray;
	TArray<FTrapIconInitData> TrapWaitArray;
	
	FVector2D ClickCursorPosition = FVector2D();
	ESelectPingType HoveredPingType = ESelectPingType::None;
};
