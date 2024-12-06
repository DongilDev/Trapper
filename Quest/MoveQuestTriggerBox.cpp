// Fill out your copyright notice in the Description page of Project Settings.


#include "Quest/MoveQuestTriggerBox.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Character/TrapperPlayer.h"
#include "Character/TrapperPlayerMovementComponent.h"
#include "Framework/TrapperGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "TrapperProject.h"

// Sets default values
AMoveQuestTriggerBox::AMoveQuestTriggerBox()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	BoxComponent->SetBoxExtent(FVector(300.f, 300.f, 300.f));
	RootComponent = BoxComponent;
}

// Called when the game starts or when spawned
void AMoveQuestTriggerBox::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AMoveQuestTriggerBox::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AMoveQuestTriggerBox::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);
	ATrapperPlayer* Player = Cast<ATrapperPlayer>(OtherActor);

	if (!Player) return;

	int32 Code = 0;

	if (!bCheckMagneticMoving)
	{
		Code = 1;
	}
	else
	{
		// 자성 이동 확인
		if (Player->Movement->GetMagneticMovingState())
		{
			Code = 2;
		}
		else
		{
			return;
		}
	}

	if (Player->HasAuthority() && Player->IsLocallyControlled())
	{
		CheckActivate(Code, true);
		UE_LOG(LogQuest, Warning, TEXT("Server Check"));
		return;
	}
	else if (!Player->HasAuthority() && Player->IsLocallyControlled())
	{
		CheckActivate(Code, false);
		UE_LOG(LogQuest, Warning, TEXT("Client Check"));
		return;
	}
}

