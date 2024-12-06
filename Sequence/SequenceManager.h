// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SequenceManager.generated.h"

UENUM(BlueprintType)
enum class ESequenceType : uint8
{
	FirstWave						UMETA(DisplayName = "FirstWave"),
	AppearBoss						UMETA(DisplayName = "AppearBoss"),
	StartBonusWave					UMETA(DisplayName = "StartBonusWave"),
	Ending							UMETA(DisplayName = "Ending"),
};

UCLASS()
class TRAPPERPROJECT_API ASequenceManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASequenceManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	UFUNCTION()
	void MediaHandle(ETrapperEventCode DialogCode);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPCPlaySequence(ESequenceType Type);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastStopSequence();

	UFUNCTION()
	void HandleMediaOpened(FString OpenedUrl);

	void MovieClear();
	bool IsMediaPlaying() { return bIsPlaying; }

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<class UMediaSoundComponent> MediaSoundComponent;

private:
	TObjectPtr<class ATrapperGameState> TrapperGameState;

	TObjectPtr<class UMediaPlayer> MoviePlayer;
	TObjectPtr<class UMediaTexture> MovieTexture;
	TArray<TObjectPtr<class UMediaSource>> MoviePlaylist;

	FTimerHandle EventTimerHandle;

	FTimerHandle MoviePlayHandle;
	ESequenceType CurrentMovie;

	uint8 bIsPlaying : 1;

	TObjectPtr<class ATrapperScriptActor> TrapperLevel;

private:
	UFUNCTION() void PlayAppearBoss();
	UFUNCTION() void PlayStartBonusWave();

	UFUNCTION() void FirstWaveFinished();
	UFUNCTION() void AppearBossFinished();
	UFUNCTION() void BonusWaveFinished();
	UFUNCTION() void EndingFinished();

};
