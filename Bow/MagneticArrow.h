// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Arrow.h"
#include "MagneticArrow.generated.h"

/**
 * 
 */
UCLASS()
class TRAPPERPROJECT_API AMagneticArrow : public AArrow
{
	GENERATED_BODY()
	
public:
	AMagneticArrow();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
	
	
/// ¸â¹ö ÇÔ¼ö ------------------------------------------------	
public:
	virtual void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;


/// ¸â¹ö º¯¼ö ------------------------------------------------
	virtual void FireArrow(FVector Direction, float Strength) override;
};
