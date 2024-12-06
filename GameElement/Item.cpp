// Fill out your copyright notice in the Description page of Project Settings.


#include "GameElement/Item.h"
#include "Particles/ParticleSystemComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Character/TrapperPlayer.h"
#include "Framework/TrapperGameState.h"
#include "EngineUtils.h"
#include "TrapperProject.h"

// Sets default values
AItem::AItem()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
}

// Called when the game starts or when spawned
void AItem::BeginPlay()
{
	Super::BeginPlay();

	Acceleration = FMath::FRandRange(5.f, 10.f);
	MaxAcceleration = Acceleration + 5.f;

	GetWorld()->GetTimerManager().SetTimer(ItemDestroyHandle, this, &AItem::AddBoneAndDestory, 1.f, false, 3.f);
}

void AItem::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	GetWorldTimerManager().ClearAllTimersForObject(this);
}

// Called every frame
void AItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (IsValid(PlayerRef))
	{
		BeDrawnToPlayer();
	}
}

void AItem::BeDrawnToPlayer()
{
	FVector RightHandBoneLocation = PlayerRef->GetMesh()->GetBoneLocation(TEXT("hand_r"));
	FVector ItemLocation = GetActorLocation();
	FVector NewPosition = FMath::VInterpTo(ItemLocation, RightHandBoneLocation, GetWorld()->GetDeltaSeconds(), Acceleration);

	const float Tolerance = 15.f;
	if (ItemLocation.Equals(RightHandBoneLocation, Tolerance))
	{
		AddBoneAndDestory();
	}

	Acceleration = FMath::Min(Acceleration + (0.05f * GetWorld()->GetDeltaSeconds()), MaxAcceleration);
	SetActorLocation(NewPosition);
}

void AItem::SetPlayer(ATrapperPlayer* Player)
{
	if (!IsValid(Player))
	{
		UE_LOG(LogTrap, Warning, TEXT("No Valid Player"));
		return;
	}

	PlayerRef = Player;
	SetActorTickEnabled(true);

	bool Is1P = false;
	if (Player->IsLocallyControlled())
	{
		Is1P = true;
	}

	MulticastSetPlayer(Is1P);
}

void AItem::MulticastSetPlayer_Implementation(bool Is1P)
{
	if(HasAuthority()) return;

	for (ATrapperPlayer* Player : TActorRange<ATrapperPlayer>(GetWorld()))
	{
		if (Is1P && !Player->IsLocallyControlled())
		{
			PlayerRef = Player;
		}

		if (!Is1P && Player->IsLocallyControlled())
		{
			PlayerRef = Player;
		}
	}

	SetActorTickEnabled(true);
}

void AItem::AddBoneAndDestory()
{
	if (HasAuthority())
	{
		ATrapperGameState* TrapperGameState = GetWorld()->GetGameState<ATrapperGameState>();
		TrapperGameState->AddBone();
	}

	Destroy();
}

