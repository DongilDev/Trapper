// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "LobbyPS.generated.h"

/**
 * 
 */
UCLASS()
class TRAPPERPROJECT_API ALobbyPS : public APlayerState
{
	GENERATED_BODY()

public:
	ALobbyPS();

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Network")
	bool bReady = false;

	UFUNCTION()
	void SetReady(bool NewReady);
};
