// Fill out your copyright notice in the Description page of Project Settings.


#include "Quest/Interact.h"
#include "Framework/TrapperGameState.h"
#include "Kismet/GameplayStatics.h"
#include "Quest/QuestManager.h"
#include "TrapperProject.h"
#include "Net/UnrealNetwork.h"

// Sets default values
AInteract::AInteract()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AInteract::BeginPlay()
{
	Super::BeginPlay();
}

void AInteract::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	//DOREPLIFETIME(AInteract, bFirstPlayerActive);
	//DOREPLIFETIME(AInteract, bSecondPlayerActive);
}

// Called every frame
void AInteract::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AInteract::CheckActivate(int32 Value, bool Is1P)
{
	if (Is1P) 
	{
		GetWorld()->GetGameState<ATrapperGameState>()->OnQuestExecute.Broadcast(Value, true);
		bFirstPlayerActive = true;
	}

	if (!Is1P)
	{
		GetWorld()->GetGameState<ATrapperGameState>()->OnQuestExecute.Broadcast(Value, false);
		bSecondPlayerActive = true;
	}
}
