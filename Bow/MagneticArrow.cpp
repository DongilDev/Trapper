// Fill out your copyright notice in the Description page of Project Settings.


#include "Bow/MagneticArrow.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"

AMagneticArrow::AMagneticArrow()
{
	Tags.Empty();
	Tags.Add(FName("MagneticArrow"));
	Tags.Add(FName("Arrow"));
	
	MinDamage = 45.f;
	MaxDamage = 60.f;
}

void AMagneticArrow::BeginPlay()
{
	Super::BeginPlay();

	SpawnMagneticEffect();
}

void AMagneticArrow::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AMagneticArrow::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor
	, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnOverlapBegin(OverlappedComp,OtherActor,OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
}

void AMagneticArrow::FireArrow(FVector DirectionIn, float StrengthIn)
{
	Super::FireArrow(DirectionIn, StrengthIn);
}
