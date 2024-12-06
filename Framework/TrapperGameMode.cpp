// Fill out your copyright notice in the Description page of Project Settings.


#include "TrapperGameMode.h"
#include "Framework/TrapperPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"

#include "Quest/QuestManager.h"
#include "Sequence/SequenceManager.h"
#include "Dialog/DialogManager.h"

#include "GameElement/Spawner.h"
#include "EngineUtils.h"
#include "Character/TrapperPlayer.h"
#include "TrapperProject.h"
#include "Tower/Tower.h"
#include "Trap/TrabSnabEnviroment/TrapSnapTrigger.h"
#include "Level/TrapperScriptActor.h"
#include "Monster/BaseMonster.h"

ATrapperGameMode::ATrapperGameMode()
{
	PrimaryActorTick.bCanEverTick = true;
	GameStateClass = ATrapperGameState::StaticClass();

	static ConstructorHelpers::FObjectFinder<UDataTable> WaveTable(TEXT("/Script/Engine.DataTable'/Game/Blueprints/Data/DT_WaveData.DT_WaveData'"));
	if (WaveTable.Succeeded() && WaveTable.Object)
	{
		OriginalWaveData = WaveTable.Object;
	}

	static ConstructorHelpers::FObjectFinder<UDataTable> DebugWaveTable(TEXT("/Script/Engine.DataTable'/Game/Blueprints/Data/DT_WaveData_Debug.DT_WaveData_Debug'"));
	if (DebugWaveTable.Succeeded() && DebugWaveTable.Object)
	{
		DebugWaveData = DebugWaveTable.Object;
	}
}

ATrapperGameMode::~ATrapperGameMode()
{

}

void ATrapperGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	//if (!NewPlayer->IsLocalController())
	//{
	//	FTimerHandle TimerHandle;
	//	GetWorldTimerManager().SetTimer(TimerHandle, this, &ATrapperGameMode::ClientLoginAfterSetting, 2.0f, false, 1.0f);

	//	//CreateGuillotine();
	//}
}

void ATrapperGameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);

	if (!Exiting->IsLocalController())
	{
		OnClientLogout();
		if (bServerPlayerTitleTravle)
		{
			UGameplayStatics::OpenLevel(this, "Title");
		}
	}
}

void ATrapperGameMode::BeginPlay()
{
	Super::BeginPlay();

	CreateManager();

	TrapperGameState = GetWorld()->GetGameState<ATrapperGameState>();
	TrapperGameState->OnEventExecute.AddUObject(this, &ATrapperGameMode::EventHandle);

	if (!bUseDebugWaveSheet && OriginalWaveData)
	{
		WaveData = OriginalWaveData;
	}
	else if (bUseDebugWaveSheet && DebugWaveData)
	{
		WaveData = DebugWaveData;
	}

	// Spawner 받아오기
	for (TActorIterator<AActor> MyActor(GetWorld()); MyActor; ++MyActor)
	{
		if (MyActor->ActorHasTag(TEXT("Spawner")))
		{
			Spanwer = Cast<ASpawner>(*MyActor);
		}
	}

	ALevelScriptActor* LevelScriptActor = GetWorld()->GetLevelScriptActor();
	TrapperLevel = Cast<ATrapperScriptActor>(LevelScriptActor);
}

void ATrapperGameMode::StartPlay()
{
	Super::StartPlay();
}

void ATrapperGameMode::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	GetWorldTimerManager().ClearAllTimersForObject(this);
}

void ATrapperGameMode::Tick(float DeltaTime)
{

}

void ATrapperGameMode::ClientLoginAfterSetting()
{

}

void ATrapperGameMode::CreateManager()
{
	if (!SequenceManager)
	{
		SequenceManager = GetWorld()->SpawnActor<ASequenceManager>(SequenceManagerClass, FVector::ZeroVector, FRotator::ZeroRotator);
	}

	if (!QuestManager)
	{
		QuestManager = GetWorld()->SpawnActor<AQuestManager>(QuestManagerClass, FVector::ZeroVector, FRotator::ZeroRotator);
	}

	if (!DialogManager)
	{
		DialogManager = GetWorld()->SpawnActor<ADialogManager>(DialogeManagerClass, FVector::ZeroVector, FRotator::ZeroRotator);
	}
}

void ATrapperGameMode::GameStart()
{
	// 첫 다이얼로그 재생
	TrapperGameState->OnEventExecute.Broadcast(ETrapperEventCode::D_Opening);
	TrapperLevel->MulticastPlayBackgroundSound(EBackgroundSound::Tutorial, 3.f);
}

void ATrapperGameMode::EventHandle(ETrapperEventCode EventCode)
{
	//UE_LOG(LogQuest, Log, TEXT("Event Code : %d"), EventCode);

	switch (EventCode)
	{

		/// --------------------------------
		///				Event 
		/// --------------------------------

#pragma region Event
	// [튜토리얼] 정비 단계로 이동 (아이템 지급O)
	case ETrapperEventCode::E_Move_Maintenance_Item_Given:
	{
		SetGameProgress(EGameProgress::Maintenance);
		InitialItemSetting(InitialItemCount);
	}
	break;

	// [튜토리얼] 정비 단계로 이동 (아이템 지급X)
	case ETrapperEventCode::E_Move_Maintenance_Item_No:
	{
		SetGameProgress(EGameProgress::Maintenance);
	}
	break;

	// [튜토리얼] 액터 정리
	case ETrapperEventCode::E_Tutorial_End_Cleanup:
	{
		QuestManager->TutorialEndSetting();
	}
	break;

	// 정비시간 차감 시작
	case ETrapperEventCode::E_Maintenance_Progress:
	{
		StartMaintenanceTime(true);

		if (TrapperGameState->MaintenanceCount > 0)
		{
			SetSkipIcon(true);
		}
	}
	break;

	// 정비 단계로 이동
	case ETrapperEventCode::E_Maintenance_Enter:
	{
		SetGameProgress(EGameProgress::Maintenance);
	}
	break;

	// 신전 해방 단계 업그레이드
	case ETrapperEventCode::E_Tower_Upgrade:
	{
		TrapperGameState->TowerUpgradeLevel++;
		TrapperGameState->OnRep_TowerUpgrade();

		// 타워 이펙트
		for (ATower* Tower : TActorRange<ATower>(GetWorld()))
		{
			Tower->MulticastUpgradeTower(TrapperGameState->TowerUpgradeLevel);
		}
	}
	break;

	// 웨이브 정보 숨김
	case ETrapperEventCode::E_Hide_Wave_HUD:
	{
		for (ATrapperPlayerController* Player : TActorRange<ATrapperPlayerController>(GetWorld()))
		{
			Player->MulticastRPCShowWaveInfo(false);
		}
	}
	break;

	// 보너스 웨이브 단계로 이동 & 남은 몬스터 수 계산
	case ETrapperEventCode::E_Start_Bonus_Wave:
	{
		GetThisWaveRemainingMonsterCount();
		TrapperGameState->Wave++;
		SetGameProgress(EGameProgress::BonusWave);
	}
	break;

	// 타워 관련 대사 및 타워 튜토리얼 대사 출력
	case ETrapperEventCode::E_Turret_Tutorial_Dialog:
	{
		if (SequenceManager->IsMediaPlaying() || DialogManager->bIsPlayingMainDialog)
		{
			UE_LOG(LogTemp, Warning, TEXT("Turret Dialog Fail"));

			return;
		}

		UE_LOG(LogTemp, Warning, TEXT("Turret Dialog Success"));


		TrapperGameState->bTowerTutorial = true;
		TrapperGameState->OnEventExecute.Broadcast(ETrapperEventCode::D_Turret_Under_Attack);

		for (ATrapperPlayerController* Player : TActorRange<ATrapperPlayerController>(GetWorld()))
		{
			Player->MulticastRPCShowTutorialDialog(true, 2, TEXT("포탑을 지키기 위해 함정을 사용해 적들을 처치하세요."));
		}

		GetWorldTimerManager().SetTimer(DialogTimerHandle, this,
			&ATrapperGameMode::UnShowTutorialDialog, 1.f, false, 10.f);
	}
	break;

	// 보스 쫄따구 정리
	case ETrapperEventCode::E_Kill_All_Monster:
	{
		KillCurrentMonster();
		TrapperGameState->MaintenanceCount = 6;
		//보스체력바활성화
		bIsBossStage = true;
		//UE_LOG(LogTemp, Warning, TEXT("Maintenance %d"), TrapperGameState->MaintenanceCount);
	}

	break;

	// 게임 클리어
	case ETrapperEventCode::E_Game_Clear:
	{
		SetGameProgress(EGameProgress::GameClear);
	}
	break;

	// 각성 및 각성 단계로 이동
	case ETrapperEventCode::E_Start_Awakening:
	{
		//BossStage 종료 확인 변수
		bIsBossStage = false;

		TrapperGameState->UpgradePlayerAwaken(true);
		TrapperGameState->UpgradePlayerAwaken(false);

		SetGameProgress(EGameProgress::Awaken);
	}
	break;

	// 배경음악 중지
	case ETrapperEventCode::E_Stop_BGM:
	{
		TrapperLevel->MulticastStopBackgroundSound(2.f);
	}
	break;

	// 함정 튜토리얼 텍스트
	case ETrapperEventCode::E_Change_Trap_Tutorial_Text:
	{
		GetWorldTimerManager().SetTimer(DialogTimerHandle, this,
			&ATrapperGameMode::TrapTutorialTextChange, 1.f, false, 3.f);
	}
	break;

	// 스킵 아이콘 활성화
	case ETrapperEventCode::E_Active_Skip_Button:
	{
		SetSkipIcon(true);
	}
	break;

	// 스킵 아이콘 비활성화
	case ETrapperEventCode::E_Deactive_Skip_Button:
	{
		SetSkipIcon(false);
	}
	break;

	// 퀘스트 시작
	case ETrapperEventCode::E_Start_Main_Quest:
	{
		QuestManager->DestroyQuestActor();
		QuestManager->QuestStart();

		for (ATrapperPlayerController* Player : TActorRange<ATrapperPlayerController>(GetWorld()))
		{
			Player->MulticastRPCShowQuestLayout(true);
		}
	}
	break;

	// 시퀀스 HUD 표시
	case ETrapperEventCode::E_Show_Media_HUD:
	{
		for (ATrapperPlayerController* Player : TActorRange<ATrapperPlayerController>(GetWorld()))
		{
			Player->MulticastSequenceHudShow(true);
			//Player->DisableInput(Player);
		}
	}
	break;

	// 시퀀스 HUD 숨김
	case ETrapperEventCode::E_Hide_Media_HUD:
	{
		for (ATrapperPlayerController* Player : TActorRange<ATrapperPlayerController>(GetWorld()))
		{
			Player->MulticastSequenceHudShow(false);
			//Player->EnableInput(Player);
		}
	}
	break;

	case ETrapperEventCode::E_Skip_Quest:
		TrapperGameState->OnQuestExecute.Broadcast(4, true);
		TrapperGameState->OnQuestExecute.Broadcast(4, false);
		break;

	case ETrapperEventCode::E_Skip_Media:
	{
		if (!SequenceManager->IsMediaPlaying())
		{
			return;
		}

		MediaSkipCount++;
		UE_LOG(LogTemp, Warning, TEXT("Skip %d"), MediaSkipCount);

		if (MediaSkipCount == 2)
		{
			SequenceManager->MulticastStopSequence();
			MediaSkipCount = 0;
		}

		for (ATrapperPlayerController* Player : TActorRange<ATrapperPlayerController>(GetWorld()))
		{
			Player->MulticastMediaText(MediaSkipCount);
		}
	}
	break;

	case ETrapperEventCode::E_Initialize_Skip_Media:
	{
		MediaSkipCount = 0;

		for (ATrapperPlayerController* Player : TActorRange<ATrapperPlayerController>(GetWorld()))
		{
			Player->MulticastMediaText(MediaSkipCount);
		}
	}
	break;
	// 인풋 비활성화
	case ETrapperEventCode::E_Disabled_Input:
		for (ATrapperPlayer* Player : TActorRange<ATrapperPlayer>(GetWorld()))
		{
			Player->MulticastSetInput(false);
		}
		break;

		// 인풋 활성화
	case ETrapperEventCode::E_Enabled_Input:
		for (ATrapperPlayer* Player : TActorRange<ATrapperPlayer>(GetWorld()))
		{
			Player->MulticastSetInput(true);
		}
		break;

	case ETrapperEventCode::E_Enter_Upgrade:

		GetWorld()->GetTimerManager().SetTimer(UpgradeDelayHandle, this, &ATrapperGameMode::EnterUpgrade, 1.f, false, 2.f);
		break;

	case ETrapperEventCode::E_Start_Wave:
		SetGameProgress(EGameProgress::Wave);
		break;

	case ETrapperEventCode::E_AbleFActive:
		TrapperGameState->bAbleActiveF = true;
		break;

	case ETrapperEventCode::E_DisableFActive:
		TrapperGameState->bAbleActiveF = false;
		break;

	case ETrapperEventCode::E_GoToBossStage:
	{
		for (ATrapperPlayerController* Player : TActorRange<ATrapperPlayerController>(GetWorld()))
			Player->MulticastSetTimeDilation(1.f);

		for (ATower* Tower : TActorRange<ATower>(GetWorld()))
			Tower->HealingTower(5000.f);

		ToSkipMaintanenctCheat(5, 21, 24, ETrapperEventCode::D_Sixth_Maintenance_Entry);
	}
	break;
	case ETrapperEventCode::E_ContinueGame:
	{
		for (ATrapperPlayerController* Player : TActorRange<ATrapperPlayerController>(GetWorld()))
			Player->MulticastSetTimeDilation(1.f);

		for (ATower* Tower : TActorRange<ATower>(GetWorld()))
			Tower->HealingTower(5000.f);
	}
	break;
	case ETrapperEventCode::E_SpawnBoss:
	{
		if (IsValid(Spanwer))
		{
			Spanwer->SpawnMonsters(0, 0, 0, 0, 1);
		}
	}
	break;


#pragma endregion Event

	/// --------------------------------
	///				Sequence 
	/// --------------------------------

#pragma region Sequence
	case ETrapperEventCode::S_Ending:
	{
		bGameClear = true;
		for (ATrapperPlayer* Player : TActorRange<ATrapperPlayer>(GetWorld()))
		{
			Player->MulticastRPCStopAwakenSound();
		}
	}
	break;

#pragma endregion Sequence

	default:
		break;

#pragma region Cheat

		// 정비 1 (튜토리얼 마지막 퀘스트 업그레이드 선택 전)
	case ETrapperEventCode::C_Maintenance_1:
	{
		if (TrapperGameState->MaintenanceCount > -1)
		{
			return;
		}

		SetSkipIcon(true);
		QuestManager->SetQuest(5);
		QuestManager->QuestComplete();
		for (ATrapperPlayerController* Player : TActorRange<ATrapperPlayerController>(GetWorld()))
		{
			Player->MulticastRPCShowQuestLayout(true);
		}

		DialogManager->MulticastRPCEndDialog();
		UnShowTutorialDialog();
		UE_LOG(LogTemp, Warning, TEXT("Maintenance %d"), TrapperGameState->MaintenanceCount);
	}
	break;
	// 정비 2 (업그레이드 선택 전)
	case ETrapperEventCode::C_Maintenance_2:
		ToSkipMaintanenctCheat(1, 5, 12, ETrapperEventCode::D_Second_Maintenance_Entry);
		UE_LOG(LogTemp, Warning, TEXT("Maintenance %d"), TrapperGameState->MaintenanceCount);
		break;
		// 정비 3 (업그레이드 선택 전)
	case ETrapperEventCode::C_Maintenance_3:
		ToSkipMaintanenctCheat(2, 9, 15, ETrapperEventCode::D_Third_Maintenance_Entry);
		UE_LOG(LogTemp, Warning, TEXT("Maintenance %d"), TrapperGameState->MaintenanceCount);

		break;
		// 정비 4 (업그레이드 선택 전)
	case ETrapperEventCode::C_Maintenance_4:
		ToSkipMaintanenctCheat(3, 13, 18, ETrapperEventCode::D_Fourth_Maintenance_Entry);
		UE_LOG(LogTemp, Warning, TEXT("Maintenance %d"), TrapperGameState->MaintenanceCount);

		break;
		// 정비 5 (업그레이드 선택 전)
	case ETrapperEventCode::C_Maintenance_5:
		ToSkipMaintanenctCheat(4, 17, 21, ETrapperEventCode::D_Fifth_Maintenance_Entry);
		UE_LOG(LogTemp, Warning, TEXT("Maintenance %d"), TrapperGameState->MaintenanceCount);

		break;
		// 정비 6 (업그레이드 선택 전)
	case ETrapperEventCode::C_Maintenance_6:
		ToSkipMaintanenctCheat(5, 21, 24, ETrapperEventCode::D_Sixth_Maintenance_Entry);
		UE_LOG(LogTemp, Warning, TEXT("Maintenance %d"), TrapperGameState->MaintenanceCount);

		break;
		// After Boss (각성 업그레이드 선택 전)
	case ETrapperEventCode::C_After_Boss:
	{
		if (TrapperGameState->CurrentGameProgress == EGameProgress::BonusWave ||
			TrapperGameState->CurrentGameProgress == EGameProgress::Awaken)
		{
			return;
		}

		for (ATrapperPlayerController* Player : TActorRange<ATrapperPlayerController>(GetWorld()))
		{
			Player->MulticastRPCShowQuestLayout(false);
		}

		// 게임 상태별 처리
		if (TrapperGameState->CurrentGameProgress == EGameProgress::Tutorial)
		{
			QuestManager->DestroyQuestActor();
			QuestManager->TutorialEndSetting();
			InitialItemSetting(InitialItemCount);
			TrapperGameState->S2SReactivateAllMagneticPillar();
		}
		else if (TrapperGameState->CurrentGameProgress == EGameProgress::Wave)
		{
			KillCurrentMonster();
		}

		ClearGameTimer();

		TrapperGameState->bTowerTutorial = true;

		// 다이얼로그 종료
		DialogManager->MulticastRPCEndDialog();

		// 업그레이드 상태 (UI 표시)
		TrapperGameState->TowerUpgradeLevel = 6;
		TrapperGameState->OnRep_TowerUpgrade();

		// 웨이브 설정
		TrapperGameState->Wave = 22;
		TrapperGameState->SubWave = 1;
		RemainingMonsterCountWave = 22;

		TrapperGameState->CurrentMonsterCount = 0;
		TrapperGameState->RemainingMonsterCount = 0;
		GetThisWaveRemainingMonsterCount();

		TrapperGameState->MaintenanceCount = 6;
		SetGameProgress(EGameProgress::BonusWave);
		QuestManager->SetQuest(25);
		SetSkipIcon(false);

		TrapperGameState->OnEventExecute.Broadcast(ETrapperEventCode::E_Hide_Wave_HUD);
		TrapperGameState->OnEventExecute.Broadcast(ETrapperEventCode::D_Bonus_Wave_Start);
		TrapperGameState->OnEventExecute.Broadcast(ETrapperEventCode::E_AbleFActive);
	}
	break;
	// GameRestart
	case ETrapperEventCode::C_Game_Restart:
	{
		GetWorld()->ServerTravel(TEXT("Opening"));
	}
	break;
	// AddBone
	case ETrapperEventCode::C_Add_Bone:
		InitialItemSetting(100);
		break;
		// Invincibility
	case ETrapperEventCode::C_Invincibility_Mode:
	{
		TrapperGameState->bIsInvincibilityMode = ~TrapperGameState->bIsInvincibilityMode;

		for (ATrapperPlayerController* Player : TActorRange<ATrapperPlayerController>(GetWorld()))
		{
			Player->MulticastShowInvincibilityModeText(TrapperGameState->bIsInvincibilityMode);
		}
	}
	break;
	// MonsterKill
	case ETrapperEventCode::C_All_Monster_Kill:
		KillCurrentMonster();
		break;
	}


#pragma endregion Cheat

}

void ATrapperGameMode::SetGameProgress(EGameProgress Progress)
{
	if (!TrapperGameState)
	{
		UE_LOG(LogQuest, Warning, TEXT("No Valid Trapper Game State"));
	}

	EGameProgress PreviousProgress = TrapperGameState->CurrentGameProgress;

	if (PreviousProgress != Progress)
	{
		TrapperGameState->CurrentGameProgress = Progress;
		TrapperGameState->OnRep_GameProgressSetting();
	}

	switch (Progress)
	{
	case EGameProgress::Tutorial:
	{
		//UE_LOG(LogTemp, Warning, TEXT("Game Progress : Tutorial"));
	}
	break;
	case EGameProgress::Maintenance:
	{
		//UE_LOG(LogTemp, Warning, TEXT("Game Progress : Maintenance"));

		TrapperLevel->MulticastPlayBackgroundSound(EBackgroundSound::Maintenance, 2.f);

		for (ATrapperPlayer* Player : TActorRange<ATrapperPlayer>(GetWorld()))
		{
			Player->MulticastShowMonsterPath(true);
		}

		MaintenanceStart();
	}
	break;
	case EGameProgress::Wave:
	{
		//UE_LOG(LogTemp, Warning, TEXT("Game Progress : Wave"));

		TrapperLevel->MulticastPlayBackgroundSound(EBackgroundSound::Wave, 2.f);

		for (ATrapperPlayer* Player : TActorRange<ATrapperPlayer>(GetWorld()))
		{
			Player->MulticastShowMonsterPath(false);
		}

		WaveStart();
	}
	break;
	case EGameProgress::BossWave:
		//UE_LOG(LogTemp, Warning, TEXT("Game Progress : BossWave"));
		WaveStart();
		break;
	case EGameProgress::BonusWave:
		//UE_LOG(LogTemp, Warning, TEXT("Game Progress : BonusWave"));
		TrapperLevel->MulticastPlayBackgroundSound(EBackgroundSound::BonusWave, 2.f);
		WaveStart();
		break;
	case EGameProgress::Awaken:
		TrapperLevel->MulticastPlayBackgroundSound(EBackgroundSound::Awaken, 2.f);
		//UE_LOG(LogTemp, Warning, TEXT("Game Progress : Awaken"));
		break;
	case EGameProgress::GameClear:
		//UE_LOG(LogTemp, Warning, TEXT("Game Progress : GameClear"));
		GameClear(true);
		break;
	case EGameProgress::GameOver:
		//UE_LOG(LogTemp, Warning, TEXT("Game Progress : GameOver"));
		GameClear(false);
		break;
	default:
		break;
	}

}

EGameProgress ATrapperGameMode::GetCurrentGameProgress()
{
	return TrapperGameState->CurrentGameProgress;
}

void ATrapperGameMode::InitialItemSetting(int Value)
{
	TrapperGameState->AddBone(Value);
}

void ATrapperGameMode::MaintenanceStart()
{
	// 정비 시간 설정
	TrapperGameState->SetMaintenanceTime(MaintenanceTime);
	TrapperGameState->MaintenanceCount++;

	// 웨이브에 출현할 몬스터 계산
	GetThisWaveRemainingMonsterCount();
}


void ATrapperGameMode::StartMaintenanceTime(bool Value)
{
	TrapperGameState->bMaintenanceInProgress = Value;
	TrapperGameState->OnRep_ChangeMaintenanceState();

	if (!Value) return;

	GetWorldTimerManager().SetTimer(MaintenanceHandle, this, &ATrapperGameMode::MaintenanceTimer, 1.f, true, 1.f);
}

void ATrapperGameMode::MaintenanceTimer()
{
	TrapperGameState->SetMaintenanceTime(--TrapperGameState->MaintenanceTimeLeft);

	// 정비시간 스킵하거나 끝났을 경우
	if (bSkipMaintenance || TrapperGameState->MaintenanceTimeLeft <= 0.f)
	{
		SkipMaintenance();
		GetWorldTimerManager().ClearTimer(MaintenanceHandle);
	}
}

void ATrapperGameMode::WaveStart()
{
	if (!GetWaveData(CurrentWaveData))
	{
		UE_LOG(LogQuest, Warning, TEXT("Wave End"), TrapperGameState->Wave, TrapperGameState->SubWave);
		return;
	}

	UE_LOG(LogQuest, Warning, TEXT("Wave %d-%d"), TrapperGameState->Wave, TrapperGameState->SubWave);
	SpawnMonster(CurrentWaveData);

	TrapperGameState->bWaveInProgress = true;
	TrapperGameState->SetWaveTime(CurrentWaveData.NextWaveLeftTime);
	TrapperGameState->SetWaveInfo();

	if (CurrentWaveData.bLastLargeWave == true)
	{
		TrapperGameState->SubWave = 1;
		TrapperGameState->bWaveInProgress = false;
		TrapperGameState->SetWaveTime(0.f);
		return;
	}

	GetWorldTimerManager().SetTimer(WaveHandle, this, &ATrapperGameMode::WaveTimer, 1.f, true, 1.f);
}

void ATrapperGameMode::WaveTimer()
{
	TrapperGameState->SetWaveTime(--TrapperGameState->WaveTimeLeft);

	if (TrapperGameState->WaveTimeLeft <= 0.f)
	{
		/// Wave --------------------------------------------
		if (GetCurrentGameProgress() == EGameProgress::Wave ||
			GetCurrentGameProgress() == EGameProgress::BossWave)
		{
			// 다음 웨이브로 넘어감
			if (CurrentWaveData.bLastSubWave)
			{
				TrapperGameState->Wave++;
				TrapperGameState->SubWave = 1;
			}
			// 서브 웨이브 계속 진행
			else
			{
				TrapperGameState->SubWave++;
			}

			GetWorldTimerManager().ClearTimer(WaveHandle);
			WaveStart();
		}

		///// Bonus Wave ------------------------------------------
		if (GetCurrentGameProgress() == EGameProgress::BonusWave
			|| GetCurrentGameProgress() == EGameProgress::Awaken)
		{
			//UE_LOG(LogTemp, Warning, TEXT("Go Timer"));

			if (CurrentWaveData.bLastSubWave)
			{
				if (TrapperGameState->CurrentMonsterCount <= 0)
				{
					TrapperGameState->Wave++;
					TrapperGameState->SubWave = 1;
					GetWorldTimerManager().ClearTimer(WaveHandle);
					WaveStart();
					//UE_LOG(LogTemp, Warning, TEXT("SubWave %d"), TrapperGameState->SubWave);
				}
			}
			else
			{
				TrapperGameState->SubWave++;
				GetWorldTimerManager().ClearTimer(WaveHandle);
				WaveStart();
			}
		}
	}
}

void ATrapperGameMode::GetThisWaveRemainingMonsterCount()
{
	uint32 k = 1;

	while (true)
	{
		FString WaveText = TEXT("Wave") + FString::FromInt(RemainingMonsterCountWave) + TEXT("_") + FString::FromInt(k);
		FWaveInfo* Data = WaveData->FindRow<FWaveInfo>(*WaveText, FString());
		if (!Data)
		{
			GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Cyan, TEXT("Wave Data Error"));
			return;
		}

		uint32 TotalMonster = 0;

		TotalMonster += Data->Skeleton;
		TotalMonster += Data->Mummy;
		TotalMonster += Data->Zombie;
		TotalMonster += Data->Debuffer;
		TotalMonster += Data->Boss;

		TrapperGameState->ChangeRemainingMonsterCount(TotalMonster);
		UE_LOG(LogQuest, Warning, TEXT("This Wave %d-%d, Total Spawn Monster %d"), RemainingMonsterCountWave, k, TotalMonster);

		if (Data->bLastLargeWave)
		{
			RemainingMonsterCountWave++;
			return;
		}
		else if (Data->bLastSubWave)
		{
			RemainingMonsterCountWave++;
			k = 1;
		}
		else
		{
			k++;
		}
	}

}

void ATrapperGameMode::KillCurrentMonster()
{
	TSet<TObjectPtr<class ABaseMonster>> MonsterList = Spanwer->SpawnMonsterList;

	for (auto Monster : MonsterList)
	{
		if (IsValid(Monster))
		{
			UGameplayStatics::ApplyDamage(Monster, 5000, GetInstigatorController(), this, UDamageType::StaticClass());
		}
	}
	//Spanwer->SpawnMonsterList.Empty();
}

bool ATrapperGameMode::GetWaveData(FWaveInfo& OutData)
{
	FString WaveText = TEXT("Wave") + FString::FromInt(TrapperGameState->Wave) + TEXT("_") + FString::FromInt(TrapperGameState->SubWave);
	FWaveInfo* Data = WaveData->FindRow<FWaveInfo>(*WaveText, FString());
	if (!Data) return false;

	OutData.bLastSubWave = Data->bLastSubWave;
	OutData.bLastLargeWave = Data->bLastLargeWave;
	OutData.Skeleton = Data->Skeleton;
	OutData.Mummy = Data->Mummy;
	OutData.Zombie = Data->Zombie;
	OutData.Debuffer = Data->Debuffer;
	OutData.Boss = Data->Boss;
	OutData.NextWaveLeftTime = Data->NextWaveLeftTime;
	return true;
}

void ATrapperGameMode::SpawnMonster(struct FWaveInfo& OutData)
{
	/// *********** Create Monster ***********
	if (!bSpawnMonster) return;

	//UE_LOG(LogQuest, Warning, TEXT("Create Monster - Skeleton %d / Mummy %d / Zombie %d / Debuffer %d / Boss %d"),
	//	OutData.Skeleton, OutData.Mummy, OutData.Zombie, OutData.Debuffer, OutData.Boss);

	if (OutData.Boss >= 1)
	{
		return;
	}

	if (IsValid(Spanwer))
	{
		Spanwer->SpawnMonsters(OutData.Skeleton, OutData.Mummy, OutData.Zombie, OutData.Debuffer, OutData.Boss);
		//UE_LOG(LogTemp, Warning, TEXT("Check"));
	}

	//UE_LOG(LogQuest, Warning, TEXT("Delete Monster : %d"), DeleteMonsterCount);
}

void ATrapperGameMode::GameClear(bool IsGameClear)
{
	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		ATrapperPlayerController* PlayerController = Cast<ATrapperPlayerController>(Iterator->Get());
		if (PlayerController)
		{
			//PlayerController->MulticastRPCGameEndScreenSetting(IsGameClear);
			TrapperGameState->UpdateScoreHUD();
			PlayerController->ClientRPCGameEndShowScore(IsGameClear);
			PlayerController->MulticastSetTimeDilation(0.01f);
		}
	}
}

void ATrapperGameMode::SkipTutorial()
{
	//UE_LOG(LogQuest, Warning, TEXT("Skip Tutorial"));

	InitialItemSetting(InitialItemCount);
	QuestManager->SkipTutorial();
	SetSkipIcon(false);

	SetGameProgress(EGameProgress::Maintenance);

	TrapperGameState->OnEventExecute.Broadcast(ETrapperEventCode::E_Tower_Upgrade);
	TrapperGameState->OnEventExecute.Broadcast(ETrapperEventCode::D_First_Maintenance_Entry);

	TrapperGameState->S2SReactivateAllMagneticPillar();
}

void ATrapperGameMode::SkipMaintenance()
{
	bSkipMaintenance = false;

	TrapperGameState->bMaintenanceInProgress = false;
	SetSkipIcon(false);

	TrapperGameState->OnQuestExecute.Broadcast(99, true);

	TrapperGameState->Wave++;

	if (TrapperGameState->MaintenanceCount == 0) return;

	if (TrapperGameState->Wave >= 20)
	{
		SetGameProgress(EGameProgress::BossWave);
	}
	else
	{
		SetGameProgress(EGameProgress::Wave);
	}
}

void ATrapperGameMode::EnterUpgrade()
{
	for (ATrapperPlayer* Player : TActorRange<ATrapperPlayer>(GetWorld()))
	{
		Player->MulticastEnterUpgrade();
	}
}

void ATrapperGameMode::ToSkipMaintanenctCheat(
	int MaintenanceCount,
	int WaveCount,
	int QuestIndex,
	ETrapperEventCode Dialog)
{
	// 같거나 낮은 단계로 스킵 방지
	if (TrapperGameState->MaintenanceCount > MaintenanceCount - 1)
	{
		UE_LOG(LogTemp, Warning, TEXT("Skip Failed / Current Count %d"), TrapperGameState->MaintenanceCount);
		return;
	}

	// 게임 상태별 처리
	if (TrapperGameState->CurrentGameProgress == EGameProgress::Tutorial)
	{
		UE_LOG(LogTemp, Warning, TEXT("Magnetic Pillar Active"));

		QuestManager->DestroyQuestActor();
		QuestManager->TutorialEndSetting();
		InitialItemSetting(InitialItemCount);
		TrapperGameState->S2SReactivateAllMagneticPillar();
	}
	else if (TrapperGameState->CurrentGameProgress == EGameProgress::Wave)
	{
		KillCurrentMonster();
	}

	ClearGameTimer();

	// 타워 튜토리얼 메세지 나오지 않게 설정
	TrapperGameState->bTowerTutorial = true;

	// 다이얼로그 종료
	DialogManager->MulticastRPCEndDialog();
	UnShowTutorialDialog();

	// 업그레이드 상태 (UI 표시)
	TrapperGameState->TowerUpgradeLevel = MaintenanceCount;
	TrapperGameState->OnRep_TowerUpgrade();

	// 웨이브 설정
	TrapperGameState->Wave = WaveCount - 1;
	TrapperGameState->OnRep_ChangeWave();
	TrapperGameState->SubWave = 1;
	RemainingMonsterCountWave = WaveCount;

	TrapperGameState->CurrentMonsterCount = 0;
	TrapperGameState->RemainingMonsterCount = 0;
	SetGameProgress(EGameProgress::Maintenance);
	TrapperGameState->MaintenanceCount = MaintenanceCount;

	//UE_LOG(LogTemp, Warning, TEXT("Skip Success / Change Count %d"), TrapperGameState->MaintenanceCount);

	// 퀘스트 설정
	QuestManager->SetQuest(QuestIndex - 2);
	SetSkipIcon(false);

	TrapperGameState->OnEventExecute.Broadcast(ETrapperEventCode::E_Enter_Upgrade);
	TrapperGameState->OnEventExecute.Broadcast(ETrapperEventCode::E_Start_Main_Quest);
}

void ATrapperGameMode::OnClientLogout()
{
	for (ATrapperPlayerController* Player : TActorRange<ATrapperPlayerController>(GetWorld()))
	{
		APawn* PlayerPawn = Player->GetPawn();
		if (IsValid(PlayerPawn))
		{
			ATrapperPlayer* TP = Cast<ATrapperPlayer>(PlayerPawn);
			if (IsValid(TP))
			{
				TP->OnClientLogout();
			}
		}
	}
}

void ATrapperGameMode::ClearGameTimer()
{
	GetWorldTimerManager().ClearTimer(MaintenanceHandle);
	GetWorldTimerManager().ClearTimer(WaveHandle);
	GetWorldTimerManager().ClearTimer(DialogTimerHandle);
}

void ATrapperGameMode::UnShowTutorialDialog() // false
{
	for (ATrapperPlayerController* Player : TActorRange<ATrapperPlayerController>(GetWorld()))
	{
		Player->MulticastRPCShowTutorialDialog(false);
	}
}

void ATrapperGameMode::TrapTutorialTextChange()
{
	for (ATrapperPlayerController* Player : TActorRange<ATrapperPlayerController>(GetWorld()))
	{
		Player->MulticastSetGodDialog(2, TEXT("포탑으로 몰려오는 몬스터들을 효율적으로 처치할 수 있도록 함정을 설치하세요."));
	}

	GetWorldTimerManager().SetTimer(DialogTimerHandle, this,
		&ATrapperGameMode::UnShowTutorialDialog, 1.f, false, 3.f);
}


void ATrapperGameMode::SetSkipIcon(bool Value)
{
	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		ATrapperPlayerController* PlayerController = Cast<ATrapperPlayerController>(Iterator->Get());
		if (PlayerController)
		{
			PlayerController->MulticastRPCSetSkipIcon(Value);
		}
	}
}

void ATrapperGameMode::OnTrapperLevelLoaded()
{
	CreateGuillotine();
}

void ATrapperGameMode::CreateGuillotine()
{
	FString LevelName = GetWorld()->GetMapName();
	//UE_LOG(LogTrap, Warning, TEXT("Current Level name: %s"), *LevelName);

	LevelName.RemoveFromStart(FString("UEDPIE_0_"));
	//UE_LOG(LogTrap, Warning, TEXT("Fixed Level Name : %s"), *LevelName);

	//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan, FString::Printf(TEXT("Level Name : %s"), *LevelName));

	if (LevelName == "Trapper" || LevelName == "UEDPIE_0_Trapper" || LevelName == "UEDPIE_1_Trapper")
	{
		//GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Cyan, FString::Printf(TEXT("Good")));
		//GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Cyan, FString::Printf(TEXT("Level Name : %s"), *LevelName));

		for (const auto& SnapTrigger : TrapSnapTriggerArray)
		{
			SnapTrigger->SpawnSnapGuillotine();
			/*SnapTrigger->ForceNetRelevant();
			SnapTrigger->ForceNetUpdate();*/
		}
	}
}



