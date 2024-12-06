// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "OpeningGM.generated.h"

/**
 * 
 */
UCLASS()
class TRAPPERPROJECT_API AOpeningGM : public AGameModeBase
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;

public:
	UFUNCTION(BlueprintCallable, Server, Reliable) // 서버에서 호출되는 함수
	void ServerTravelToTrapper(); // Trapper 레벨로 이동

	void PlayOpeningMediaGM();

	void OnPlayerLoaded(); // PlayerController가 호출하는 함수
	
	void PressedSkip(int32 Type);

public:
	bool ClientSkip = false;
	bool ServerSkip = false;

	int32 ExpectedClientNum = 2;
	int32 CurrentClientNum = 0;
};
