// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TrapFailInstallHUD.generated.h"

/**
 *
 */
UCLASS()
class TRAPPERPROJECT_API UTrapFailInstallHUD : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeDestruct() override;

public:
	void ShowText(FString string);
	void SetTextLifeOnTimer();

private:
	UPROPERTY(meta = (BindWidget), VisibleAnywhere)
	TObjectPtr<class UTextBlock> TrapText;

	FTimerHandle TimerHandle;

};
