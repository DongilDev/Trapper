// Fill out your copyright notice in the Description page of Project Settings.


#include "GameElement/PathArrow.h"
#include "NiagaraComponent.h"
//#include "NiagaraSystem.h"

// Sets default values
APathArrow::APathArrow()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Arrow = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Arrow"));
	RootComponent = Arrow;
}

// Called when the game starts or when spawned
void APathArrow::BeginPlay()
{
	Super::BeginPlay();
	

}


