// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PlayerPingType.h"
#include "PlayerPing.generated.h"



UCLASS()
class TRAPPERPROJECT_API APlayerPing : public AActor
{
	GENERATED_BODY()
	
public:	
	APlayerPing();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	void SetType(EPlayerPingType type, bool Is1P);
	
	UFUNCTION()
	void TrapActivated();

	UFUNCTION(BlueprintImplementableEvent)
	void InitCompassMarkerHUD(bool Is1P);

private:
	void SetMarkerType();

public:
	UPROPERTY(BlueprintReadOnly, Category = "HUD")
	EPlayerPingType PingType = EPlayerPingType::E_Default1P;
	
	UPROPERTY() 
	class UMapMarkerComponent* MapMarkerComponent;
	
	UPROPERTY(EditDefaultsOnly) 
	class UIndicatorComponent* IndicatiorComponent;

	UPROPERTY(EditDefaultsOnly)
	class USphereComponent* TrapSearchCollider;

	UPROPERTY(EditDefaultsOnly, Category = "HUD")
	TSubclassOf<class UUserWidget> CompassHUDClass;
	
	UPROPERTY(BlueprintReadWrite, Category = "HUD")
	UUserWidget* CompassHUDRef;

private:
	FTimerHandle ColliderDeleteTimer;
	void DeleteCollider();

	bool bIs1P = true;
	bool bIsTrapDetacted = false;

	UPROPERTY() APlayerController* PlayerController = nullptr;
	UPROPERTY() APawn* ControlledPawn = nullptr;
	UPROPERTY() class ATrapBase* TargetTrap = nullptr;
};
