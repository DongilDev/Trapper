// Fill out your copyright notice in the Description page of Project Settings.


#include "Ping/QuestPing.h"
#include "Ping/IndicatorComponent.h"

// Sets default values
AQuestPing::AQuestPing()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	IndicatorComponent = CreateDefaultSubobject<UIndicatorComponent>(TEXT("IndicatorComponent"));
	AddOwnedComponent(IndicatorComponent);
}

// Called when the game starts or when spawned
void AQuestPing::BeginPlay()
{
	Super::BeginPlay();

	IndicatorComponent->SetType(EPlayerPingType::E_Quest);
}

// Called every frame
void AQuestPing::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

