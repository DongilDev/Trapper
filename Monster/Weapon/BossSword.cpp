// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster/Weapon/BossSword.h"

// Sets default values
ABossSword::ABossSword()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	BossSwordMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Sword"));
	RootComponent = BossSwordMesh;
}

// Called when the game starts or when spawned
void ABossSword::BeginPlay()
{
	Super::BeginPlay();
	
}


