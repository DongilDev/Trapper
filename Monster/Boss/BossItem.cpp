// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster/Boss/BossItem.h"

// Sets default values
ABossItem::ABossItem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	DummyItem = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DummyItem"));
	RootComponent = DummyItem;

	Item = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Item"));
	Item->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void ABossItem::BeginPlay()
{
	Super::BeginPlay();
	
}

void ABossItem::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

