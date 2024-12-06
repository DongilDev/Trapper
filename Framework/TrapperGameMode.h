// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Framework/TrapperGameState.h"
#include "Data/WaveDataTables.h"
#include "Data/EventCode.h"
#include "TrapperGameMode.generated.h"


UCLASS()
class TRAPPERPROJECT_API ATrapperGameMode : public AGameModeBase
{
	GENERATED_BODY()

#pragma region Main & Override

public:
	ATrapperGameMode();
	~ATrapperGameMode();

	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void Logout(AController* Exiting) override;

protected:
	virtual void BeginPlay();
	virtual void StartPlay();
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void Tick(float DeltaTime) override;

	void ClientLoginAfterSetting();

public:
	UPROPERTY(EditAnywhere) TSubclassOf<class ASequenceManager> SequenceManagerClass;
	UPROPERTY(EditAnywhere) TSubclassOf<class AQuestManager> QuestManagerClass;
	UPROPERTY(EditAnywhere) TSubclassOf<class ADialogManager> DialogeManagerClass;

	void CreateManager();

	TObjectPtr<class ATrapperGameState> TrapperGameState;
	TObjectPtr<class AQuestManager> QuestManager;
	TObjectPtr<class ASequenceManager> SequenceManager;
	TObjectPtr<class ADialogManager> DialogManager;

public:
	FORCEINLINE void OnServerPlayerTitleTravle() { bServerPlayerTitleTravle = true; }

private:
	uint8 bServerPlayerTitleTravle : 1 = false;

#pragma endregion Main & Override

#pragma region Game

public:
	void GameStart();

	UFUNCTION()
	void EventHandle(ETrapperEventCode EventCode);

	void SetGameProgress(EGameProgress Progress);
	EGameProgress GetCurrentGameProgress();

	//void FinalQuestStart();
	void GameClear(bool IsGameClear);

	void SetSkipIcon(bool Value);
	void SkipTutorial();
	uint8 bSkipMaintenance : 1 = false;
	void SkipMaintenance();

	FTimerHandle UpgradeDelayHandle;
	UFUNCTION() void EnterUpgrade();

	uint8 MediaSkipCount = 0;

	void ToSkipMaintanenctCheat(int MaintenanceCount, int WaveCount, int QuestIndex, ETrapperEventCode Dialog);

	TObjectPtr<class ATrapperScriptActor> TrapperLevel;
	void OnClientLogout();

	bool GetIsGameClear() { return bGameClear; }
	void ClearGameTimer();

private:
	uint8 bGameClear : 1;

#pragma endregion Game

#pragma region Event Function

public:
	FTimerHandle DialogTimerHandle;
	void UnShowTutorialDialog();
	void TrapTutorialTextChange();

#pragma endregion Event Function 


#pragma region Maintenance

public:
	// 초기 지급되는 아이템 갯수
	UPROPERTY(EditDefaultsOnly, Category = "Game Setting")
	int32 InitialItemCount = 1000;
	void InitialItemSetting(int Value);

	UPROPERTY(EditDefaultsOnly, Category = "Game Setting")
	int32 MaintenanceTime = 120;

	void MaintenanceStart();

	void StartMaintenanceTime(bool Value);
	FTimerHandle MaintenanceHandle;
	UFUNCTION() void MaintenanceTimer();

#pragma endregion Maintenance


#pragma region Wave

public:
	// 웨이브 디버그용 시트
	UPROPERTY(EditDefaultsOnly, Category = "Debug")
	uint8 bUseDebugWaveSheet : 1;

	TObjectPtr<class UDataTable> DebugWaveData;
	TObjectPtr<class UDataTable> OriginalWaveData;

	FTimerHandle WaveHandle;

	FWaveInfo CurrentWaveData;

	TObjectPtr<class UDataTable> WaveData;
	uint8 RemainingMonsterCountWave = 1;

	bool GetWaveData(struct FWaveInfo& OutData);
	void WaveStart();
	UFUNCTION() void WaveTimer();

#pragma endregion Wave

#pragma region Trap

public:
	void CreateGuillotine();
	void OnTrapperLevelLoaded();
	FORCEINLINE TArray<TSoftObjectPtr<class ATrapSnapTrigger>> GetTrapSnapTriggerArray() const { return TrapSnapTriggerArray; }

	UPROPERTY(EditAnywhere, Category = "Trap")
	TArray<TSoftObjectPtr<class ATrapSnapTrigger>> TrapSnapTriggerArray;

	UPROPERTY(EditAnywhere, Category = "Arch")
	TSoftObjectPtr<class AArch> FristArch;

	UPROPERTY(EditAnywhere, Category = "Arch")
	TSoftObjectPtr<class AArch> SecondArch;

#pragma endregion Trap

#pragma region Monster

public:

	// 몬스터 스폰 유무(디버깅용)
	UPROPERTY(EditDefaultsOnly, Category = "Debug")
	uint8 bSpawnMonster : 1 = true;

	//보스 체력바 활성화 확인용 
	uint8 bIsBossStage : 1 = false;

	TObjectPtr<class ASpawner> Spanwer;
	void SpawnMonster(struct FWaveInfo& OutData);

	// 메인 웨이브에 출현할 몬스터 수
	void GetThisWaveRemainingMonsterCount();

	void KillCurrentMonster();

#pragma endregion Monster

};
