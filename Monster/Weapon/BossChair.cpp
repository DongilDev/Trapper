// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster/Weapon/BossChair.h"

// Sets default values
ABossChair::ABossChair()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	BossChairMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Chair"));
	RootComponent = BossChairMesh;
}

// Called when the game starts or when spawned
void ABossChair::BeginPlay()
{
	Super::BeginPlay();
	
}



