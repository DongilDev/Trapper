// Fill out your copyright notice in the Description page of Project Settings.


#include "Level/TrapperScriptActor.h"
#include "trapperscriptactor.h"
#include "Kismet/GameplayStatics.h"
#include "Components/AudioComponent.h"
#include "Sound/SoundManager.h"
#include "Framework/TrapperGameState.h"
#include "Framework/TrapperGameMode.h"


void ATrapperScriptActor::BeginPlay()
{
	Super::BeginPlay();

	SoundManager = GetWorld()->SpawnActor<ASoundManager>(ASoundManager::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator);
	BackgroundAudio = SoundManager->Audio;
}

void ATrapperScriptActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	GetWorldTimerManager().ClearAllTimersForObject(this);
}

void ATrapperScriptActor::StopBackgroundSound()
{
	BackgroundAudio->Stop();
}

void ATrapperScriptActor::MulticastPlayBackgroundSound_Implementation(EBackgroundSound Sound, float FadeInTime)
{	
	float FadeOutTime = 2.f;

	if (BackgroundAudio->IsPlaying())
	{
		MulticastStopBackgroundSound_Implementation(FadeOutTime);
		ChangeSoundDelegate.BindUFunction(this, FName("PlayBackgroundSound"), Sound, FadeInTime);
		GetWorld()->GetTimerManager().SetTimer(FadeInHandle, ChangeSoundDelegate, 1.0f, true, FadeOutTime);
		return;
	}
	
	PlayBackgroundSound(Sound, FadeInTime);
}

void ATrapperScriptActor::PlayBackgroundSound(EBackgroundSound Sound, float FadeInTime)
{
	if (BackgroundAudio->IsPlaying())
	{
		GetWorld()->GetTimerManager().ClearTimer(FadeInHandle);
		return;
	}

	//UE_LOG(LogTemp, Warning, TEXT("Play Background Sound %d"), (int)Sound);

	switch (Sound)
	{
	case EBackgroundSound::Tutorial:
		BackgroundAudio->SetSound(TutorialSound);
		break;
	case EBackgroundSound::Maintenance:
		BackgroundAudio->SetSound(MaintenanceSound);
		break;
	case EBackgroundSound::Wave:
		BackgroundAudio->SetSound(WaveSound);
		break;
	case EBackgroundSound::BossWave:
		BackgroundAudio->SetSound(BossWaveSound);
		break;
	case EBackgroundSound::BonusWave:
		BackgroundAudio->SetSound(BonusWaveSound);
		break;
	case EBackgroundSound::Awaken:
	{
		BackgroundAudio->SetSound(AwakenSound);
		GetWorld()->GetTimerManager().SetTimer(FinishTimerHandle, this, &ATrapperScriptActor::AwakenEnd, 1.f, 
			false, AwakenSound->Duration);
	}
		break;
	case EBackgroundSound::GameClear:
		BackgroundAudio->SetSound(GameClearSound);
		break;
	case EBackgroundSound::GameOver:
		BackgroundAudio->SetSound(GameOverSound);
		break;
	default:
		break;
	}

	BackgroundAudio->FadeIn(FadeInTime);
}

void ATrapperScriptActor::AwakenEnd()
{
	if (HasAuthority())
	{
		GetWorld()->GetTimerManager().ClearAllTimersForObject(GetWorld()->GetAuthGameMode<ATrapperGameMode>());

		ATrapperGameMode* GameMode = GetWorld()->GetAuthGameMode<ATrapperGameMode>();
		if (GameMode->GetIsGameClear())
		{
			UE_LOG(LogTemp, Warning, TEXT("Already Clear"));
			return;
		}

		UE_LOG(LogTemp, Warning, TEXT("BGM Time Over Clear"));
		ATrapperGameState* GameState = GetWorld()->GetGameState<ATrapperGameState>();
		GameState->OnQuestExecute.Broadcast(98, true);
		GameState->OnEventExecute.Broadcast(ETrapperEventCode::C_All_Monster_Kill);
	}
}

void ATrapperScriptActor::ReleaseTimer()
{
	GetWorld()->GetTimerManager().ClearTimer(FinishTimerHandle);
}

void ATrapperScriptActor::MulticastStopBackgroundSound_Implementation(float FadeOutTime)
{
	//UE_LOG(LogTemp, Warning, TEXT("Stop Background Sound"));

	if (FadeOutTime <= 0.0f)
	{
		BackgroundAudio->Stop();
	}

	BackgroundAudio->FadeOut(FadeOutTime, 0.0f);

	GetWorld()->GetTimerManager().SetTimer(FadeOutHandle, this, 
		&ATrapperScriptActor::StopBackgroundSound, 1.f, false, FadeOutTime);
}

void ATrapperScriptActor::MulticastPlaySystemSound_Implementation(ESystemSound Sound)
{
	switch (Sound)
	{
	case ESystemSound::QuestClear:
		if (QuestClearSound)
		{
			UGameplayStatics::PlaySound2D(GetWorld(), QuestClearSound);
		}
		break;
	default:
		break;
	}
}