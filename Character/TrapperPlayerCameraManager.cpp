// Fill out your copyright notice in the Description page of Project Settings.


#include "TrapperPlayerCameraManager.h"
#include "Character/TrapperPlayer.h"
#include "Character/TrapperPlayerMovementComponent.h"

ATrapperPlayerCameraManager::ATrapperPlayerCameraManager()
{

}

void ATrapperPlayerCameraManager::UpdateViewTarget(FTViewTarget& OutVT, float DeltaTime)
{
	Super::UpdateViewTarget(OutVT, DeltaTime);

	if (ATrapperPlayer* Player = Cast<ATrapperPlayer>(GetOwningPlayerController()->GetPawn()))
	{
		UTrapperPlayerMovementComponent* Movement = Player->Movement;
		if (Movement->IsCustomMovementMode(ECustomMovementMode::CMOVE_MagneticMove))
		{
			FOVTime = FMath::Clamp(FOVTime + DeltaTime, 0.f, IncreaseFOVDurtation);
			Player->SpawnSpeedLineEffect(true);
		}
		else
		{
			FOVTime = FMath::Clamp(FOVTime - DeltaTime, 0.f, IncreaseFOVDurtation);
			Player->SpawnSpeedLineEffect(false);
		}

		float CurrentFOV = OutVT.POV.FOV;
		float InterpFOV = FMath::Lerp(CurrentFOV, TargetFOV, FMath::Clamp(FOVTime / IncreaseFOVDurtation, 0.f, 1.f));

		//UE_LOG(LogTemp, Warning, TEXT("FOV : %f"), InterpFOV);
		OutVT.POV.FOV = InterpFOV;
	}
}
