// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/LevelScriptActor.h"
#include "TrapperScriptActor.generated.h"

/**
 *
 */
UENUM(BlueprintType)
enum class ESystemSound : uint8
{
	QuestClear		UMETA(DisplayName = "QuestClear"),
};

UENUM(BlueprintType)
enum class EBackgroundSound : uint8
{
	Tutorial		UMETA(DisplayName = "Tutorial"),
	Maintenance		UMETA(DisplayName = "Maintenance"),
	Wave			UMETA(DisplayName = "Wave"),
	BossWave		UMETA(DisplayName = "BossWave"),
	BonusWave		UMETA(DisplayName = "BonusWave"),
	Awaken			UMETA(DisplayName = "Awaken"),
	GameClear		UMETA(DisplayName = "GameClear"),
	GameOver		UMETA(DisplayName = "GameOver"),
};

UCLASS()
class TRAPPERPROJECT_API ATrapperScriptActor : public ALevelScriptActor
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION(NetMulticast, Reliable)
	void MulticastPlayBackgroundSound(EBackgroundSound Sound, float FadeInTime);

	UFUNCTION()
	void PlayBackgroundSound(EBackgroundSound Sound, float FadeInTime);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastStopBackgroundSound(float FadeOutTime);

	UFUNCTION(NetMulticast, Unreliable)
	void MulticastPlaySystemSound(ESystemSound Sound);

	UFUNCTION()
	void AwakenEnd();

	void ReleaseTimer();

#pragma region Sound

public:
	TObjectPtr<class ASoundManager> SoundManager;
	TObjectPtr<class UAudioComponent> BackgroundAudio;

	// -------------------------------------------------

	UPROPERTY(EditDefaultsOnly, Category = "Background")
	TObjectPtr<class USoundBase> TutorialSound;

	UPROPERTY(EditDefaultsOnly, Category = "Background")
	TObjectPtr<class USoundBase> MaintenanceSound;

	UPROPERTY(EditDefaultsOnly, Category = "Background")
	TObjectPtr<class USoundBase> WaveSound;

	UPROPERTY(EditDefaultsOnly, Category = "Background")
	TObjectPtr<class USoundBase> BossWaveSound;

	UPROPERTY(EditDefaultsOnly, Category = "Background")
	TObjectPtr<class USoundBase> BonusWaveSound;

	UPROPERTY(EditDefaultsOnly, Category = "Background")
	TObjectPtr<class USoundBase> AwakenSound;

	UPROPERTY(EditDefaultsOnly, Category = "Background")
	TObjectPtr<class USoundBase> GameClearSound;

	UPROPERTY(EditDefaultsOnly, Category = "Background")
	TObjectPtr<class USoundBase> GameOverSound;

	// -------------------------------------------------

	UPROPERTY(EditDefaultsOnly, Category = "Effect")
	TObjectPtr<class USoundBase> QuestClearSound;

	// -------------------------------------------------

	FTimerHandle FadeOutHandle;
	UFUNCTION() void StopBackgroundSound();

	FTimerHandle FadeInHandle;
	FTimerDelegate ChangeSoundDelegate;

	FTimerHandle FinishTimerHandle;

#pragma endregion Sound
};