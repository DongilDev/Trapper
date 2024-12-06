// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Ping/PlayerPingType.h"
#include "PingSelectHUD.generated.h"
/**
 * 
 */
UCLASS()
class TRAPPERPROJECT_API UPingSelectHUD : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintImplementableEvent)
	void OnHovered(ESelectPingType type);
	
	UFUNCTION(BlueprintImplementableEvent)
	void OnUnhovered(ESelectPingType type);

	FVector2D GetSize();

public:
	UPROPERTY(meta=(BindWidget), EditAnywhere, BlueprintReadWrite, Category = "Icon")
	TObjectPtr<class UImage> Default;

// 	UPROPERTY(meta=(BindWidget), EditAnywhere, BlueprintReadWrite)
// 	TObjectPtr<class UImage> TrapInstall;
// 
// 	UPROPERTY(meta=(BindWidget), EditAnywhere, BlueprintReadWrite)
// 	TObjectPtr<class UImage> TrapActivation;
// 
// 	UPROPERTY(meta=(BindWidget), EditAnywhere, BlueprintReadWrite)
// 	TObjectPtr<class UImage> Warning;
// 
// 	UPROPERTY(meta=(BindWidget), EditAnywhere, BlueprintReadWrite)
// 	TObjectPtr<class UImage> Assemble;
};
