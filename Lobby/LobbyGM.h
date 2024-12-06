// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "LobbyGM.generated.h"

/**
 * 
 */
UCLASS()
class TRAPPERPROJECT_API ALobbyGM : public AGameModeBase
{
	GENERATED_BODY()
public:
	virtual void PostLogin(APlayerController* NewPlayer) override;

	virtual void Logout(AController* Exiting) override;

	virtual void BeginPlay() override;

	FTimerHandle LobbyTimer;

	UFUNCTION()
	void DecreaseTime();

	void StartGame();
	void CountDown();

public:
	FORCEINLINE void OnServerPlayerTitleTravle() { bServerPlayerTitleTravle = true; }


private:
	uint8 bServerPlayerTitleTravle : 1 = false;
};
