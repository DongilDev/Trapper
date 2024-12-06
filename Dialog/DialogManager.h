// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Data/DialogDataTables.h"
#include "DialogManager.generated.h"

UCLASS()
class TRAPPERPROJECT_API ADialogManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADialogManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	TObjectPtr<class ATrapperGameState> TrapperGameState;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<class UAudioComponent> AudioComponent;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	TObjectPtr<class UDataTable> DialogData;

	UFUNCTION()
	void DialogHandle(ETrapperEventCode DialogCode);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPCPlayDialog(int32 DialogCode);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPCEndDialog();

	void PlayDialog(int32 DialogCode);

	TObjectPtr<class ATrapperPlayerController> PlayerController;

	FString GetCharacterName(ECharacterName Character);
	int32 DialogIndex = 0;
	int32 LastDialogCode = 0;

	uint8 bPlayerPlaying : 1 = false;
	uint8 bGodPlaying : 1 = false;

	uint8 bIsPlayingMainDialog : 1 = false;

	float VoiceOffset = 0.5f;

	UFUNCTION() void EndDialog();
	UFUNCTION() void ContinueDialog();

private:
	FTimerHandle DialogEndHandle;
	
};
