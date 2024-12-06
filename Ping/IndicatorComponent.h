// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PlayerPingType.h"
#include "IndicatorComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TRAPPERPROJECT_API UIndicatorComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UIndicatorComponent();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void SetType(EPlayerPingType type);

	
public:
	UPROPERTY(EditDefaultsOnly, Category = "HUD")
	TSubclassOf<class UPingIndicatorHUD> IndicatorHUDClass;

private:
	UPROPERTY()
	TObjectPtr<UPingIndicatorHUD> IndicatorHUDRef = nullptr;

	UPROPERTY() 
	TObjectPtr<class ATrapperPlayerController> PlayerController = nullptr;

	UPROPERTY() 
	TObjectPtr<APawn> ControlledPawn = nullptr;

	EPlayerPingType Type = EPlayerPingType::None;
	
	FTimerHandle UpdateTimer;
	void UpdateIndicator();

	FTimerHandle TryAddTimer;
	void TryAddToPlayerHUD();

	bool IsPointInEllipse(FVector2D ViewportSize, FVector2D ScreenPosition, FVector2D EllipseCenter, float RadiusX, float RadiusY);
	FVector2D FindClosestPointOnEllipse(FVector2D ViewportSize, const FVector2D& ScreenPosition, FVector2D EllipseCenter, float RadiusX, float RadiusY);

};
