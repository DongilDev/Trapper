// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/PlayerCameraManager.h"
#include "TrapperPlayerCameraManager.generated.h"

/**
 * 
 */
UCLASS()
class TRAPPERPROJECT_API ATrapperPlayerCameraManager : public APlayerCameraManager
{
	GENERATED_BODY()

public:
	ATrapperPlayerCameraManager();

public:
	UPROPERTY(EditDefaultsOnly)
	float IncreaseFOVDurtation = 0.5f;
	float FOVTime;

	UPROPERTY(EditDefaultsOnly)
	float TargetFOV = 120.f;

	virtual void UpdateViewTarget(FTViewTarget& OutVT, float DeltaTime) override;
};
