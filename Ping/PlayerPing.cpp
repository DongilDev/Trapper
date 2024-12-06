
#include "PlayerPing.h"

#include "DrawDebugHelpers.h"
#include "Blueprint/UserWidget.h"
#include "Components/WidgetComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SphereComponent.h"

#include "MarkerComponent/MapMarkerComponent.h"
#include "Widget/PingIndicatorHUD.h"
#include "Components/TextBlock.h"
#include "Trap/TrapBase.h"
#include "Character/TrapperPlayer.h"
#include "Ping/PingMechanics.h"
#include "Ping/IndicatorComponent.h"
#include "Character/TrapperPlayer.h"
#include "Widget/PlayerHUD.h"


APlayerPing::APlayerPing()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	MapMarkerComponent = CreateDefaultSubobject<UMapMarkerComponent>(TEXT("MapMarkerComponent"));
	MapMarkerComponent->ObjectType = EMapObjectType::Ping1P;
	AddOwnedComponent(MapMarkerComponent);

	IndicatiorComponent = CreateDefaultSubobject<UIndicatorComponent>(TEXT("IndicatiorComponent"));
	AddOwnedComponent(IndicatiorComponent);
}


void APlayerPing::BeginPlay()
{
	Super::BeginPlay();

	PlayerController = UGameplayStatics::GetPlayerController(this, 0);
	if(PlayerController)
		ControlledPawn = PlayerController->GetPawn();

	// 컴퍼스에 띄울 HUD 생성
	if(IsValid(CompassHUDClass))
	{
		CompassHUDRef = CreateWidget<UUserWidget>(this->GetGameInstance(), CompassHUDClass);
		if(CompassHUDRef)
		{
// 			ATrapperPlayer* Player = Cast<ATrapperPlayer>(GetOwner());
// 			if(IsValid(Player) && IsValid(Player->PlayerHud))
// 			{
// 				Player->PlayerHud
// 			}
			CompassHUDRef->AddToViewport();
			CompassHUDRef->SetVisibility(ESlateVisibility::HitTestInvisible);
		}
	}
}


void APlayerPing::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	GetWorld()->GetTimerManager().ClearAllTimersForObject(this);

	if(CompassHUDRef)
	{
		CompassHUDRef->RemoveFromParent();
		CompassHUDRef = nullptr;
	}
}


void APlayerPing::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{	
	if(bIsTrapDetacted)
	{
		return;
	}
	
	if(!IsValid(TargetTrap) && OtherActor->ActorHasTag("Trap"))
	{
		TargetTrap = Cast<ATrapBase>(OtherActor);
		bIsTrapDetacted = true;

		// 델리게이트 바인딩
		TargetTrap->OnTrapActivated.AddUObject(this, &APlayerPing::TrapActivated);

		DeleteCollider();
	}
}


void APlayerPing::SetType(EPlayerPingType type, bool Is1P)
{
	PingType = type;
	bIs1P = Is1P;

	SetMarkerType();
	InitCompassMarkerHUD(bIs1P);
	IndicatiorComponent->SetType(type);

	if (type == EPlayerPingType::E_TrapActivate1P || type == EPlayerPingType::E_TrapActivate2P )
	{
		// 콜라이더 생성
		TrapSearchCollider = NewObject<USphereComponent>(this, USphereComponent::StaticClass());

		if(TrapSearchCollider)
		{
			TrapSearchCollider->SetupAttachment(RootComponent);
			TrapSearchCollider->RegisterComponent();

			TrapSearchCollider->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
			TrapSearchCollider->SetRelativeRotation(FRotator(0.0f, 0.0f, 0.0f));

			TrapSearchCollider->OnComponentBeginOverlap.AddDynamic(this, &APlayerPing::OnOverlapBegin);
			TrapSearchCollider->SetCollisionProfileName("PlayerPing");
		}

		// 콜라이더 삭제 타이머 설정
		GetWorld()->GetTimerManager().SetTimer(ColliderDeleteTimer,
			this, &APlayerPing::DeleteCollider, 1.f, false);
	}
}



void APlayerPing::TrapActivated()
{
	// 핑 설치한 액터에게 알림, 삭제
	ATrapperPlayer* PlayerRef = Cast<ATrapperPlayer>(GetOwner());
	if(PlayerRef)
	{
		PlayerRef->PingMechanics->DestroyPing();
	}
}


void APlayerPing::SetMarkerType()
{
	switch(PingType)
	{
	case EPlayerPingType::E_Default1P:
		MapMarkerComponent->SetType(EMapObjectType::Ping1P);
		break;

	case EPlayerPingType::E_Default2P:
		MapMarkerComponent->SetType(EMapObjectType::Ping2P);
		break;

	case EPlayerPingType::E_TrapInstall1P:
		MapMarkerComponent->SetType(EMapObjectType::E_TrapInstall1P);
		break;

	case EPlayerPingType::E_TrapInstall2P:
		MapMarkerComponent->SetType(EMapObjectType::E_TrapInstall2P);
		break;

	case EPlayerPingType::E_TrapActivate1P:
		MapMarkerComponent->SetType(EMapObjectType::E_TrapActivate1P);
		break;

	case EPlayerPingType::E_TrapActivate2P:
		MapMarkerComponent->SetType(EMapObjectType::E_TrapActivate2P);
		break;

	case EPlayerPingType::E_Warning1P:
		MapMarkerComponent->SetType(EMapObjectType::E_Warning1P);
		break;

	case EPlayerPingType::E_Warning2P:
		MapMarkerComponent->SetType(EMapObjectType::E_Warning2P);
		break;

	case EPlayerPingType::E_Assemble1P:
		MapMarkerComponent->SetType(EMapObjectType::E_Assemble1P);
		break;

	case EPlayerPingType::E_Assemble2P:
		MapMarkerComponent->SetType(EMapObjectType::E_Assemble2P);
		break;

	default:
		break;
	}
}


void APlayerPing::DeleteCollider()
{
	if(TrapSearchCollider)
	{
		TrapSearchCollider->DestroyComponent();
		TrapSearchCollider = nullptr;
	}
}

