// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "OpeningPC.generated.h"

/**
 * 
 */
UCLASS()
class TRAPPERPROJECT_API AOpeningPC : public APlayerController
{
	GENERATED_BODY()
public:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "UI")
	TSubclassOf<class UOpeningWidgetBase> OpeningWidgetClass;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "UI")
	class UOpeningWidgetBase* OpeningWidgetObject;

// F키 눌러서 스킵하게
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<class UInputMappingContext> OpeningIMC;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<class UInputAction> SkipAction;

	UFUNCTION(Server, Reliable)
	void ServerClientPressSkip();

	UFUNCTION(Client, Reliable)
	void ClientShowLoading();

	UFUNCTION(Server, Reliable)
	void ReportLoadingComplete();

	UFUNCTION(Client, Reliable)
	void ClientPlayMedia();

	void PlayOpeningMediaPC();

private:
	UFUNCTION()
	void SkipOpening();
};
