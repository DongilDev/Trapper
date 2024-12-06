// Fill out your copyright notice in the Description page of Project Settings.


#include "Bow/ClientArrow.h"
#include "Framework/TrapperGameState.h"

AClientArrow::AClientArrow()
{
	Tags.Empty();
	bReplicates = false;
	bAlwaysRelevant = false; 
}

void AClientArrow::BeginPlay()
{
	Super::BeginPlay();

	SetReplicates(false);
	SetReplicateMovement(false);
	bAlwaysRelevant = false;
	bIsSpawnedBy1P = false;
}

void AClientArrow::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void AClientArrow::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AClientArrow::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	MulticastRPCSpawnHitParticle(SweepResult.Location);
	
	if(!(GameState->bIsArrowPenetrable))
	{
		DeactivateArrow();
	}
}

void AClientArrow::OnArrowHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	MulticastRPCSpawnHitParticle(Hit.Location);
	DeactivateArrow();
}

void AClientArrow::FireArrow(FVector DirectionIn, float StrengthIn)
{
	Super::FireArrow(DirectionIn, StrengthIn);
	SpawnMagneticEffect();
}
