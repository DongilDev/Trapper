

#include "MapMarkerComponent.h"

#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInstance.h"
#include "GameFramework/Character.h"
#include "Components/SceneComponent.h"
#include "GameFramework/Actor.h"

#include "Character/TrapperPlayer.h"
#include "MapMarkerStruct.h"
#include "TrapMarkerStruct.h"
#include "Map/MapMechanics.h"
#include "TrapperProject.h"

#include "Kismet/GameplayStatics.h"
#include "Character/TrapperPlayer.h"

UMapMarkerComponent::UMapMarkerComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}


// Called when the game starts
void UMapMarkerComponent::BeginPlay()
{
	Super::BeginPlay();

	OwnerActorRef = GetOwner();

	// 월드맵에 띄울 아이콘과 연결
	TArray<AActor*> OutActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATrapperPlayer::StaticClass(), OutActors);

	for(const auto& Pawn : OutActors)
	{
		ATrapperPlayer* TrapperPlayer = Cast<ATrapperPlayer>(Pawn);
		if(!TrapperPlayer) continue;

		if(TrapperPlayer->IsLocallyControlled())
		{
			PlayerRef = TrapperPlayer;
		}
	}

	if(ObjectType != EMapObjectType::None )
	{
		SetType(ObjectType);
	}

	else if(TrapType != EMapTrapType::None)
	{
		SetTrapType(TrapType);
	}
}


void UMapMarkerComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	if(IsValid(PlayerRef))
	{
		PlayerRef->MapMechanics->DeleteMarker(GetUniqueID());
	}
}


void UMapMarkerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}


void UMapMarkerComponent::SetType(EMapObjectType type)
{
	ObjectType = type;

	if(IsValid(PlayerRef) && ObjectType != EMapObjectType::Player)
	{
		if(!bIsCreatedMarker)
		{
			PlayerRef->MapMechanics->AddObjectMarker(OwnerActorRef, GetUniqueID(), ObjectType, Offset);
			bIsCreatedMarker = true;
		}
		else
		{
			PlayerRef->MapMechanics->ChangeObjectMarker(GetUniqueID(), ObjectType);
		}
	}
}


void UMapMarkerComponent::SetTrapType(EMapTrapType type)
{
	TrapType = type;

	if(IsValid(PlayerRef) && IsValid(PlayerRef->MapMechanics))
	{
		if(!bIsCreatedMarker)
		{
			PlayerRef->MapMechanics->AddTrapMarker(OwnerActorRef, GetUniqueID(), TrapType, Offset);
			bIsCreatedMarker = true;
		}
		else
		{
			PlayerRef->MapMechanics->ChangeTrapMarker(GetUniqueID(), TrapType);
		}
	}
}


void UMapMarkerComponent::SetOffset(FVector offset)
{
	Offset = offset;
	if(IsValid(PlayerRef) && IsValid(PlayerRef->MapMechanics) && bIsCreatedMarker)
	{
		PlayerRef->MapMechanics->SetOffset(GetUniqueID(), Offset);
	}
}
