// Fill out your copyright notice in the Description page of Project Settings.


#include "Framework/TrapperGameState.h"
#include "Kismet/GameplayStatics.h"
#include "TrapperGameMode.h"
#include "Net/UnrealNetwork.h"
#include "Widget/PlayerHUD.h"
#include "Level/TrapperScriptActor.h"
#include "Trap/TrapBase.h"
#include "Character/TrapperPlayer.h"
#include "TrapperProject.h"
#include "EngineUtils.h"
#include "Tower/Tower.h"
#include "Framework/TrapperPlayerController.h"
#include "Trap/PlankTrap.h"
#include "Trap/GunpowderBarrelTrap.h"
#include "Trap/SpikeTrap.h"
#include "Bow/Arrow.h"
#include "GameElement/MagneticPillar.h"
#include "GameElement/Spawner.h"


ATrapperGameState::ATrapperGameState()
{
	LoadUpgradeTable();
}

void ATrapperGameState::HandleBeginPlay()
{
	Super::HandleBeginPlay();

	if (HasAuthority())
	{
		// 배열 셔플
		ShuffleArray(NormalUpgradeTableArray);
		ShuffleArray(RareUpgradeTableArray);
		ShuffleArray(EpicUpgradeTableArray);

	}

	// Spawner 받아오기
	for (TActorIterator<AActor> MyActor(GetWorld()); MyActor; ++MyActor)
	{
		if (MyActor->ActorHasTag(TEXT("Spawner")))
		{
			Spanwer = Cast<ASpawner>(*MyActor);
		}
	}

}

void ATrapperGameState::OnRep_ReplicatedHasBegunPlay()
{
	Super::OnRep_ReplicatedHasBegunPlay();
}

void ATrapperGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ATrapperGameState, bIsClientStart);

	DOREPLIFETIME(ATrapperGameState, MaintenanceTimeLeft);
	DOREPLIFETIME(ATrapperGameState, MaintenanceCount);
	DOREPLIFETIME(ATrapperGameState, bMaintenanceInProgress);
	DOREPLIFETIME(ATrapperGameState, TowerUpgradeLevel);

	DOREPLIFETIME(ATrapperGameState, WaveTimeLeft);
	DOREPLIFETIME(ATrapperGameState, bWaveInProgress);
	DOREPLIFETIME(ATrapperGameState, Wave);
	DOREPLIFETIME(ATrapperGameState, SubWave);
	DOREPLIFETIME(ATrapperGameState, MaxWave);

	DOREPLIFETIME(ATrapperGameState, bTowerTutorial);

	DOREPLIFETIME(ATrapperGameState, CurrentMonsterCount);
	DOREPLIFETIME(ATrapperGameState, RemainingMonsterCount);
	DOREPLIFETIME(ATrapperGameState, CurrentGameProgress);

	DOREPLIFETIME(ATrapperGameState, NormalUpgradeTableArray);
	DOREPLIFETIME(ATrapperGameState, RareUpgradeTableArray);
	DOREPLIFETIME(ATrapperGameState, EpicUpgradeTableArray);

	DOREPLIFETIME(ATrapperGameState, bCanPlaceClapTrap);

	DOREPLIFETIME(ATrapperGameState, MoveSpeed);
	DOREPLIFETIME(ATrapperGameState, MagneticMoveSpeed);
	DOREPLIFETIME(ATrapperGameState, MagneticMoveAfterSpeed);
	DOREPLIFETIME(ATrapperGameState, IncomingDamage);
	DOREPLIFETIME(ATrapperGameState, PlayerMaxHealth);
	DOREPLIFETIME(ATrapperGameState, bCanReviveImmediately1P);
	DOREPLIFETIME(ATrapperGameState, bCanReviveImmediately2P);

	DOREPLIFETIME(ATrapperGameState, TrapCost);
	DOREPLIFETIME(ATrapperGameState, TrapDamage);
	DOREPLIFETIME(ATrapperGameState, PlayerMaxHealth);
	DOREPLIFETIME(ATrapperGameState, bIsRemovedArrowCharge);
	DOREPLIFETIME(ATrapperGameState, ChargeSpeed);

	DOREPLIFETIME(ATrapperGameState, ServerSelect);
	DOREPLIFETIME(ATrapperGameState, ClientSelect);

	DOREPLIFETIME(ATrapperGameState, ActivatedPillarCount);
	DOREPLIFETIME(ATrapperGameState, MagneticPillarMaxCount);

	DOREPLIFETIME(ATrapperGameState, bIsInvincibilityMode);

	DOREPLIFETIME(ATrapperGameState, Bone);
	DOREPLIFETIME(ATrapperGameState, bAbleActiveF);
}

void ATrapperGameState::ServerRPCUpdatebIsClientStart_Implementation(bool isClientStart)
{
	//UE_LOG(LogTemp, Warning, TEXT("RPC Work!"));

	MulticastRPCUpdatebIsClientStart(true);
}

void ATrapperGameState::MulticastRPCUpdatebIsClientStart_Implementation(bool isClientStart)
{
	//UE_LOG(LogTemp, Warning, TEXT("RPC2 Work!"));
	bIsClientStart = true;
}


void ATrapperGameState::ChangeMonsterCount(int32 Count)
{
	if (HasAuthority())
	{
		CurrentMonsterCount += Count;
		OnRep_ChangeCurrentMonster();
		//UE_LOG(LogQuest, Warning, TEXT("%d"), Count);

		if (CurrentMonsterCount == 0)
		{
			CheckAllWaveMonsterDie();
		}
	}
}

void ATrapperGameState::ChangeRemainingMonsterCount(int32 Count)
{
	if (Count == 0) return;

	if (HasAuthority())
	{
		RemainingMonsterCount += Count;
		OnRep_ChangeRemainingMonsterCount();

		if (RemainingMonsterCount == 0)
		{
			CheckAllWaveMonsterDie();
		}
	}
}

void ATrapperGameState::ClearRemainingMonsterCount()
{
	if (HasAuthority())
	{
		RemainingMonsterCount = 0;
		OnRep_ChangeRemainingMonsterCount();
	}
}

void ATrapperGameState::SetPlayerHUDRef(class UPlayerHUD* Ref)
{
	PlayerHUDRef = Ref;

	if (IsValid(PlayerHUDRef))
	{
		PlayerHUDRef->SetMagneticPillarCount(ActivatedPillarCount, MagneticPillarMaxCount);
	}
}

void ATrapperGameState::SetWaveInfo()
{
	if (PlayerHUDRef)
	{
		PlayerHUDRef->SetWaveInfo(Wave, MaxWave);
	}
}

void ATrapperGameState::SetWaveInfoRemainingTime(float RemainingTime)
{
	if (PlayerHUDRef)
	{
		PlayerHUDRef->SetWaveInfoRemainingTime(RemainingTime);
	}
}

void ATrapperGameState::OnRep_GameProgressSetting()
{

}

void ATrapperGameState::OnRep_TowerUpgrade()
{
	if (PlayerHUDRef)
	{
		PlayerHUDRef->SetTowerUpgrade(TowerUpgradeLevel);
	}
}

void ATrapperGameState::SetMaintenanceTime(float Value)
{
	MaintenanceTimeLeft = Value;
	SetWaveInfoRemainingTime(MaintenanceTimeLeft);
}

void ATrapperGameState::OnRep_ChangeMaintenanceTimeLeft()
{
	SetWaveInfoRemainingTime(MaintenanceTimeLeft);
}

void ATrapperGameState::OnRep_ChangeMaintenanceState()
{
	if (bMaintenanceInProgress && PlayerHUDRef)
	{
		PlayerHUDRef->ShowWaveInfo(true);
	}
}

void ATrapperGameState::SetWaveTime(float Value)
{
	WaveTimeLeft = Value;
	SetWaveInfoRemainingTime(WaveTimeLeft);
}

void ATrapperGameState::OnRep_ChangeWaveTimeLeft()
{
	SetWaveInfoRemainingTime(WaveTimeLeft);
}

void ATrapperGameState::OnRep_ChangeWave()
{
	SetWaveInfo();
}

void ATrapperGameState::OnRep_ChangeCurrentMonster()
{
	if (PlayerHUDRef)
	{
		PlayerHUDRef->SetCurrentMonsterCount(CurrentMonsterCount);
	}
}

void ATrapperGameState::OnRep_ChangeRemainingMonsterCount()
{
	if (PlayerHUDRef)
	{
		PlayerHUDRef->SetRemainingMonsterCount(RemainingMonsterCount);
	}
}

void ATrapperGameState::CheckAllWaveMonsterDie()
{
	if (CurrentMonsterCount == 0 && RemainingMonsterCount == 0)
	{
		OnQuestExecute.Broadcast(98, true);
	}
}


/// ------------------------------------------------------------------------
///									Item
/// ------------------------------------------------------------------------

void ATrapperGameState::AddBone(int32 Count)
{
	if (!HasAuthority()) return;

	if (Bone < BoneMax)
	{
		Bone += Count;

		if (Bone > BoneMax)
		{
			Bone = BoneMax;
		}
	}

	PlayerHUDRef->SetItemCount(Bone);
}

void ATrapperGameState::UseBone(int32 Count)
{
	if (!HasAuthority())
	{
		return;
	}

	int32 UsedBoneCount = FMath::Min(Bone, Count);
	Bone -= UsedBoneCount;
	PlayerHUDRef->SetItemCount(Bone);
	//UE_LOG(LogTemp, Warning, TEXT("Server : Use Bone"));
}

void ATrapperGameState::OnRep_Bone()
{
	PlayerHUDRef->SetItemCount(Bone);
}

/// ------------------------------------------------------------------------
///									Upgrade
/// ------------------------------------------------------------------------

void ATrapperGameState::UpgradePlayerAwaken(bool bIsServer)
{
	for (ATrapperPlayer* Player : TActorRange<ATrapperPlayer>(GetWorld()))
	{
		if (bIsServer && Player->IsLocallyControlled())
		{
			Player->S2SSetAwaken(bIsServer);
		}
		else if (!bIsServer && !Player->IsLocallyControlled())
		{
			Player->S2SSetAwaken(bIsServer);
		}
	}
}

void ATrapperGameState::OnRep_AdjustMoveSpeed()
{
	for (ATrapperPlayer* Player : TActorRange<ATrapperPlayer>(GetWorld()))
	{
		Player->AdjustMoveSpeed();
	}
}

void ATrapperGameState::OnRep_AdjustMagneticMoveSpeed()
{
	for (ATrapperPlayer* Player : TActorRange<ATrapperPlayer>(GetWorld()))
	{
		Player->AdjustMagneticMoveSpeed();
	}
}

void ATrapperGameState::OnRep_AdjustMagneticMoveAfterSpeed()
{
	for (ATrapperPlayer* Player : TActorRange<ATrapperPlayer>(GetWorld()))
	{
		Player->AdjustMagneticMoveAfterSpeed();
	}
}

void ATrapperGameState::AdjustPlayerMaxHealth()
{
	for (ATrapperPlayer* Player : TActorRange<ATrapperPlayer>(GetWorld()))
	{
		if (HasAuthority())
		{
			Player->AdjustPlayerMaxHealth();
		}
	}
}

const TArray<FUpgradeTable>& ATrapperGameState::GetUpgradeArray()
{
	switch (MaintenanceCount)
	{
	case 1:
	case 2:
		return NormalUpgradeTableArray;
	case 3:
	case 4:
		return RareUpgradeTableArray;

	case 5:
		return EpicUpgradeTableArray;

	case 6:
		return AwakenUpgradeTableArray;
	}
	return EpicUpgradeTableArray;
}


const FPillarUpgradeTable ATrapperGameState::GetMagneticPillarData()
{
	if (CurrentGameProgress == EGameProgress::Tutorial)
	{
		return PillarUpgradeTableArray[5];
	}

	switch (MaintenanceCount)
	{
	case 1:
		return PillarUpgradeTableArray[0];
	case 2:
		return PillarUpgradeTableArray[1];
	case 3:
		return PillarUpgradeTableArray[2];
	case 4:
		return PillarUpgradeTableArray[3];
	case 5:
		return PillarUpgradeTableArray[4];
	}

	return PillarUpgradeTableArray[0];
}

void ATrapperGameState::SelectUpgrade(const uint8& index, const bool& bIsServer)
{
	uint8 BinaryMaintenace = GetBinaryMaintenanceCount();

	if (bIsServer)
	{
		SelectedUpgradeIndexServer = index;
		ServerSelect = ServerSelect | BinaryMaintenace;
	}
	else
	{
		SelectedUpgradeIndexClient = index;
		ClientSelect = ClientSelect | BinaryMaintenace;
	}

	FUpgradeTable TableData;

	if (index != 6)
	{
		switch (MaintenanceCount)
		{
		case 1:
		case 2:
			TableData = NormalUpgradeTableArray[index];
			break;

		case 3:
		case 4:
			TableData = RareUpgradeTableArray[index];
			break;

		case 5:
			TableData = EpicUpgradeTableArray[index];
			break;

		case 6:
			return;

		default:
			TableData = EpicUpgradeTableArray[index];
			break;
		}

		if (TableData.ProgrammingName == "MoveSpeed")
		{
			MoveSpeed += TableData.EffectAmount;
			OnRep_AdjustMoveSpeed();
		}
		else if (TableData.ProgrammingName == "ChargeSpeed")
		{
			ChargeSpeed += TableData.EffectAmount;
		}
		else if (TableData.ProgrammingName == "ArrowDamage")
		{
			ArrowDamage += TableData.EffectAmount;
		}
		else if (TableData.ProgrammingName == "ArrowSpeed")
		{
			ArrowSpeed += TableData.EffectAmount;
		}
		else if (TableData.ProgrammingName == "IncomingDamage")
		{
			IncomingDamage += TableData.EffectAmount;
		}
		else if (TableData.ProgrammingName == "PlayerMaxHealth")
		{
			PlayerMaxHealth += TableData.EffectAmount;
			AdjustPlayerMaxHealth();
		}
		else if (TableData.ProgrammingName == "MagneticMoveSpeed")
		{
			MagneticMoveSpeed += TableData.EffectAmount;
			OnRep_AdjustMagneticMoveSpeed();
		}
		else if (TableData.ProgrammingName == "MagneticMoveAfterSpeed")
		{
			MagneticMoveAfterSpeed += TableData.EffectAmount;
			OnRep_AdjustMagneticMoveAfterSpeed();
		}
		else if (TableData.ProgrammingName == "TrapCost")
		{
			TrapCost += TableData.EffectAmount;
			TrapUpgradeCost(); // 동일
		}
		else if (TableData.ProgrammingName == "TrapDamage")
		{
			TrapDamage += TableData.EffectAmount;
			TrapUpgradeDamage(); // 동일
		}
		else if (TableData.ProgrammingName == "MonsterDetectRange")
		{
			MonsterDetectRange += TableData.EffectAmount;
			///TODO::윤서 
			//왐마 이거 귀찮아서 안빼둿는데 살려주세요
		}
		else if (TableData.ProgrammingName == "MonsterMoveSpeed")
		{
			MonsterMoveSpeed += TableData.EffectAmount;
			ChangeMonsterSpeed();
		}
		else if (TableData.ProgrammingName == "MonsterHealth")
		{
			MonsterHealth += TableData.EffectAmount;
			ChangeMonsterHealth();
		}
		else if (TableData.ProgrammingName == "MonsterAttackDamage")
		{
			MonsterAttackDamage += TableData.EffectAmount;
			//ChangeMonsterDamage();
		}
		else if (TableData.ProgrammingName == "RemoveHeadShoot")
		{
			bIsRemovedHeadShoot = true;
			ArrowDamage += TableData.EffectAmount;
		}
		else if (TableData.ProgrammingName == "RemoveArrowCharge")
		{
			bIsRemovedArrowCharge = true;
			ArrowDamage += TableData.EffectAmount;
		}
		else if (TableData.ProgrammingName == "ArrowPenetrable")
		{
			bIsArrowPenetrable = true;
		}
		else if (TableData.ProgrammingName == "ReviveImmediately")
		{
			bCanReviveImmediately1P = true;
			bCanReviveImmediately2P = true;
		}
		else if (TableData.ProgrammingName == "TempleHealthRecovery")
		{
			for (ATower* Tower : TActorRange<ATower>(GetWorld()))
			{
				Tower->HealingTower(TableData.EffectAmount);
			}
		}
	}
	else
	{
		int32 PillarReactivateCount = GetMagneticPillarData().EffectAmount;
		while (DeactivatedPillars.Num() > 0 && PillarReactivateCount > 0)
		{
			DeactivatedPillars.Pop()->S2SSetLock(false);
			PillarReactivateCount--;
			ActivatedPillarCount = MagneticPillars.Num() - DeactivatedPillars.Num();
			OnRep_MagneticPillarCountChanged();
		}
	}

	if (SelectedUpgradeIndexServer != 255 && SelectedUpgradeIndexClient != 255)
	{
		if (CurrentGameProgress == EGameProgress::Tutorial)
		{
			ShuffleArray(NormalUpgradeTableArray);
		}
		else
		{
			RemoveSelectedUpgradeAndSuffle();
			AddScore(EScoreType::CollaborationScore, 1500);
		}
	}
}

bool ATrapperGameState::IsUpgradeSelected(bool bIsServer)
{
	uint8 CurrentMainance = GetBinaryMaintenanceCount();
	switch (MaintenanceCount)
	{
	case 1:
		CurrentMainance = 0b10000;
		break;

	case 2:
		CurrentMainance = 0b01000;
		break;

	case 3:
		CurrentMainance = 0b00100;
		break;

	case 4:
		CurrentMainance = 0b00010;
		break;

	case 5:
		CurrentMainance = 0b00001;
		break;
	}

	if (bIsServer)
	{
		return  0 < (CurrentMainance & ServerSelect);
	}
	else
	{
		return  0 < (CurrentMainance & ClientSelect);
	}
}

bool ATrapperGameState::IsPillarReactivatedUpgradeSeletable()
{
	return (ActivatedPillarCount != MagneticPillarMaxCount);
}

void ATrapperGameState::RemoveSelectedUpgradeAndSuffle()
{
	int big = -1; int small = -1;
	if (SelectedUpgradeIndexServer < SelectedUpgradeIndexClient)
	{
		big = SelectedUpgradeIndexClient;
		small = SelectedUpgradeIndexServer;
	}
	else
	{
		big = SelectedUpgradeIndexServer;
		small = SelectedUpgradeIndexClient;
	}

	switch (MaintenanceCount)
	{
	case 1:
	case 2:
		if (big != 6)	NormalUpgradeTableArray.RemoveAt(big);
		if (small != 6)	NormalUpgradeTableArray.RemoveAt(small);
		ShuffleArray(NormalUpgradeTableArray);
		break;

	case 3:
	case 4:
		if (big != 6)	RareUpgradeTableArray.RemoveAt(big);
		if (small != 6)	RareUpgradeTableArray.RemoveAt(small);
		ShuffleArray(RareUpgradeTableArray);
		break;

	case 5:
		if (big != 6)	EpicUpgradeTableArray.RemoveAt(big);
		if (small != 6)	EpicUpgradeTableArray.RemoveAt(small);
		break;
	}

	SelectedUpgradeIndexServer = 255;
	SelectedUpgradeIndexClient = 255;
}

uint8 ATrapperGameState::GetBinaryMaintenanceCount()
{
	if (CurrentGameProgress == EGameProgress::Tutorial)
	{
		return 0b000000;
	}

	switch (MaintenanceCount)
	{
	case 1:
		return 0b100000;

	case 2:
		return 0b010000;

	case 3:
		return 0b001000;

	case 4:
		return 0b000100;

	case 5:
		return 0b000010;

	case 6:
		return 0b000001;

	default:
		return 0b000000;
	}
}

void ATrapperGameState::LoadUpgradeTable()
{
	// 데이터 테이블 로드
	static ConstructorHelpers::FObjectFinder<UDataTable> DataTableFinder(
		TEXT("/Script/Engine.DataTable'/Game/Blueprints/Data/DT_Upgrade_Normal.DT_Upgrade_Normal'"));

	if (DataTableFinder.Succeeded())
	{
		NormalTable = DataTableFinder.Object;
	}

	//
	static ConstructorHelpers::FObjectFinder<UDataTable> DataTableFinder1(
		TEXT("/Script/Engine.DataTable'/Game/Blueprints/Data/DT_Upgrade_Rare.DT_Upgrade_Rare'"));

	if (DataTableFinder1.Succeeded())
	{
		RareTable = DataTableFinder1.Object;
	}

	//
	static ConstructorHelpers::FObjectFinder<UDataTable> DataTableFinder2(
		TEXT("/Script/Engine.DataTable'/Game/Blueprints/Data/DT_Upgrade_Epic.DT_Upgrade_Epic'"));

	if (DataTableFinder2.Succeeded())
	{
		EpicTable = DataTableFinder2.Object;
	}

	static ConstructorHelpers::FObjectFinder<UDataTable> DataTableFinder3(
		TEXT("/Script/Engine.DataTable'/Game/Blueprints/Data/DT_Upgrade_Awaken.DT_Upgrade_Awaken'"));

	if (DataTableFinder3.Succeeded())
	{
		AwakenTable = DataTableFinder3.Object;
	}

	static ConstructorHelpers::FObjectFinder<UDataTable> DataTableFinder4(
		TEXT("/Script/Engine.DataTable'/Game/Blueprints/Data/DT_Upgrade_Pillar.DT_Upgrade_Pillar'"));

	if (DataTableFinder4.Succeeded())
	{
		PillarTable = DataTableFinder4.Object;
	}

	if (!NormalTable || !RareTable || !EpicTable || !AwakenTable || !PillarTable)
	{
		UE_LOG(LogTemp, Warning, TEXT("업그레이드 테이블 정보를 불러올 수 없음 ATrapperGameState LoadUpgradeTable"));
		return;
	}

	// 배열 초기화
	TArray<FName> RowNames;
	RowNames = NormalTable->GetRowNames();
	for (const FName& RowName : RowNames)
	{
		FUpgradeTable* Data = NormalTable->FindRow<FUpgradeTable>(RowName, FString());
		NormalUpgradeTableArray.Add(*Data);
	}

	RowNames = RareTable->GetRowNames();
	for (const FName& RowName : RowNames)
	{
		FUpgradeTable* Data = RareTable->FindRow<FUpgradeTable>(RowName, FString());
		RareUpgradeTableArray.Add(*Data);
	}

	RowNames = EpicTable->GetRowNames();
	for (const FName& RowName : RowNames)
	{
		FUpgradeTable* Data = EpicTable->FindRow<FUpgradeTable>(RowName, FString());
		EpicUpgradeTableArray.Add(*Data);
	}

	RowNames = PillarTable->GetRowNames();
	for (const FName& RowName : RowNames)
	{
		FPillarUpgradeTable* Data = PillarTable->FindRow<FPillarUpgradeTable>(RowName, FString());
		PillarUpgradeTableArray.Add(*Data);
	}

	RowNames = AwakenTable->GetRowNames();
	FUpgradeTable* Data = AwakenTable->FindRow<FUpgradeTable>("Awaken", FString());

	AwakenUpgradeTableArray.Add(*Data);
	AwakenUpgradeTableArray.Add(*Data);
	AwakenUpgradeTableArray.Add(*Data);
	AwakenUpgradeTableArray.Add(*Data);
}

void ATrapperGameState::TrapUpgradeDamage()
{
	//UE_LOG(LogTrap, Warning, TEXT("TrapUpgradeDamage"));

	UWorld* World = GetWorld();
	if (World)
	{
		TArray<AActor*> AllActors;
		UGameplayStatics::GetAllActorsOfClass(World, ATrapBase::StaticClass(), AllActors);

		for (AActor* Actor : AllActors)
		{
			ATrapBase* Trap = Cast<ATrapBase>(Actor);
			if (Trap)
			{
				Trap->UpgradeDamage(TrapDamage);
			}
		}

		UGameplayStatics::GetAllActorsOfClass(World, ATrapperPlayer::StaticClass(), AllActors);
		for (AActor* Actor : AllActors)
		{
			ATrapperPlayer* Player = Cast<ATrapperPlayer>(Actor);
			if (Player)
			{
				Player->UpgradeTrapDamage(TrapDamage);
			}
		}
	}
}

void ATrapperGameState::TrapUpgradeCost()
{
	//UE_LOG(LogTrap, Warning, TEXT("TrapUpgradeCost"));

	UWorld* World = GetWorld();
	if (World)
	{
		TArray<AActor*> AllActors;
		UGameplayStatics::GetAllActorsOfClass(World, ATrapperPlayer::StaticClass(), AllActors);

		for (AActor* Actor : AllActors)
		{
			ATrapperPlayer* Player = Cast<ATrapperPlayer>(Actor);
			if (Player)
			{
				Player->UpgradeTrapInstallCost(TrapCost);
			}
		}
	}
}

/// ------------------------------------------------------------------------
///									MultiKill
/// ------------------------------------------------------------------------

void ATrapperGameState::ResetKillCount(AActor* DamageCauser)
{
	// UE_LOG(LogTrap, Warning, TEXT("ResetCount"));
	//UE_LOG(LogTrap, Warning, TEXT("DamageCauser name : %s"), *DamageCauser->GetName());

	if (KillCountMap.Contains(DamageCauser))
	{
		if (DamageCauser->ActorHasTag("SpikeTrap"))
		{
			uint8 KillCount = KillCountMap[DamageCauser];
			if (KillCount >= 2)
			{
				ASpikeTrap* SpikeTrap = Cast<ASpikeTrap>(DamageCauser);
				if (SpikeTrap)
				{
					if (SpikeTrap->IsInstalled1P())
					{
						//UE_LOG(LogTrap, Warning, TEXT("Multikill Score Spike 1P : %d"), KillCount * 100);
						AddScore(EScoreType::MonsterScore1P, KillCount * 100);
					}
					else
					{
						//UE_LOG(LogTrap, Warning, TEXT("Multikill Score Spike 2P : %d"), KillCount * 100);
						AddScore(EScoreType::MonsterScore2P, KillCount * 100);
					}
				}
			}
		}

		KillCountMap[DamageCauser] = 0;
	}
}

void ATrapperGameState::RegisterKill(AActor* DamageCauser)
{
	//UE_LOG(LogTrap, Warning, TEXT("RegisterKill"));
	//UE_LOG(LogTrap, Warning, TEXT("DamageCauser name : %s"), *DamageCauser->GetName());

	if (KillCountMap.Contains(DamageCauser))
	{
		++KillCountMap[DamageCauser];
	}
	else
	{
		KillCountMap.Add(DamageCauser, 1);
	}

	//UE_LOG(LogTrap, Warning, TEXT("KillCount : %d"), KillCountMap[DamageCauser]);
}

void ATrapperGameState::CheckKillCount(AController* EventInstigator, AActor* DamageCauser)
{
	if (!KillCountMap.Contains(DamageCauser))
	{
		return;
	}

	uint8 KillCount = KillCountMap[DamageCauser];
	if (KillCount >= 2)
	{
		ATrapperPlayerController* PlayerController = Cast<ATrapperPlayerController>(EventInstigator);
		if (PlayerController)
		{
			PlayerController->ClientRPCUpdateMultikillHUD(KillCount);
		}

		if (DamageCauser->ActorHasTag("Trap"))
		{
			if (DamageCauser->ActorHasTag("SpikeTrap"))
			{
				return;
			}
			else if (DamageCauser->ActorHasTag("PlankTrap"))
			{
				APlankTrap* PlankTrap = Cast<APlankTrap>(DamageCauser);
				if (PlankTrap)
				{
					if (PlankTrap->IsTriggered1P())
					{
						//UE_LOG(LogTrap, Warning, TEXT("Multikill Score Plank 1P : %d"), KillCount * 100);
						AddScore(EScoreType::MonsterScore1P, KillCount * 100);
					}
					else
					{
						//UE_LOG(LogTrap, Warning, TEXT("Multikill Score Plank 2P : %d"), KillCount * 100);
						AddScore(EScoreType::MonsterScore2P, KillCount * 100);
					}
				}
			}
			else if (DamageCauser->ActorHasTag("GunpowderBarrelTrap"))
			{
				AGunpowderBarrelTrap* GunpowderBarrelTrap = Cast<AGunpowderBarrelTrap>(DamageCauser);
				if (GunpowderBarrelTrap->IsTriggeredExternal())
				{
					if (GunpowderBarrelTrap->IsInstalled1P())
					{
						//UE_LOG(LogTrap, Warning, TEXT("Multikill Score GunpowderBarrel 1P : %d"), KillCount * 100);
						AddScore(EScoreType::MonsterScore1P, KillCount * 100);
					}
					else
					{
						//UE_LOG(LogTrap, Warning, TEXT("Multikill Score GunpowderBarrel 2P : %d"), KillCount * 100);
						AddScore(EScoreType::MonsterScore2P, KillCount * 100);
					}
				}
				else
				{
					if (GunpowderBarrelTrap->IsTriggered1P())
					{
						//UE_LOG(LogTrap, Warning, TEXT("Multikill Score GunpowderBarrel 1P : %d"), KillCount * 100);
						AddScore(EScoreType::MonsterScore1P, KillCount * 100);
					}
					else
					{
						//UE_LOG(LogTrap, Warning, TEXT("Multikill Score GunpowderBarrel 2P : %d"), KillCount * 100);
						AddScore(EScoreType::MonsterScore2P, KillCount * 100);
					}
				}
			}
		}
		else if (DamageCauser->ActorHasTag("Arrow"))
		{
			AArrow* Arrow = Cast<AArrow>(DamageCauser);
			if (Arrow)
			{
				if (Arrow->IsSpawnedBy1P())
				{
					AddScore(EScoreType::MonsterScore1P, KillCount * 100);
				}
				else
				{
					AddScore(EScoreType::MonsterScore2P, KillCount * 100);
				}
			}

		}
	}
}

void ATrapperGameState::CheckAndResetKillCount(AController* EventInstigator, AActor* DamageCauser)
{
	CheckKillCount(EventInstigator, DamageCauser);
	ResetKillCount(DamageCauser);
}

void ATrapperGameState::CheckKillCountCollaboration(AActor* DamageCauser)
{
	if (!KillCountMap.Contains(DamageCauser))
	{
		return;
	}

	uint8 KillCount = KillCountMap[DamageCauser];
	if (KillCount >= 2)
	{
		TArray<AActor*> PlayerControllers;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerController::StaticClass(), PlayerControllers);

		for (AActor* Actor : PlayerControllers)
		{
			ATrapperPlayerController* PlayerController = Cast<ATrapperPlayerController>(Actor);
			if (PlayerController)
			{
				PlayerController->ClientRPCUpdateMultikillHUD(KillCount);
			}
		}

		if (DamageCauser->ActorHasTag("Trap"))
		{
			//UE_LOG(LogTrap, Warning, TEXT("Multikill Score Collaboration : %d"), KillCount * 100);
			AddScore(EScoreType::MonsterScoreAll, KillCount * 100);
		}
	}
}

void ATrapperGameState::CheckAndResetKillCountCollaboration(AActor* DamageCauser)
{
	CheckKillCountCollaboration(DamageCauser);
	ResetKillCount(DamageCauser);
}

void ATrapperGameState::CheckAndResetKillCountCollaborationForClapTrap(AActor* DamageCauser, AActor* OtherClap)
{
	uint8 KillCount = 0;

	if (KillCountMap.Contains(DamageCauser))
	{
		KillCount += KillCountMap[DamageCauser];
		KillCountMap[DamageCauser] = 0;
	}
	if (KillCountMap.Contains(OtherClap))
	{
		KillCount += KillCountMap[OtherClap];
		KillCountMap[OtherClap] = 0;
	}

	if (KillCount >= 2)
	{
		TArray<AActor*> PlayerControllers;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerController::StaticClass(), PlayerControllers);

		for (AActor* Actor : PlayerControllers)
		{
			ATrapperPlayerController* PlayerController = Cast<ATrapperPlayerController>(Actor);
			if (PlayerController)
			{
				PlayerController->ClientRPCUpdateMultikillHUD(KillCount);
			}
		}

		//UE_LOG(LogTrap, Warning, TEXT("Multikill Score ClapTrap Collaboration : %d"), KillCount * 100);
		AddScore(EScoreType::MonsterScoreAll, KillCount * 100);
	}
}

void ATrapperGameState::AddScore(EScoreType ScoreType, uint32 Value /*= 0*/)
{
	switch (ScoreType)
	{
	case EScoreType::MonsterScoreAll:
		Scores.MonsterScore1P += Value;
		Scores.MonsterScore2P += Value;
		break;
	case EScoreType::MonsterScore1P:
		Scores.MonsterScore1P += Value;
		break;
	case EScoreType::MonsterScore2P:
		Scores.MonsterScore2P += Value;
		break;
	case EScoreType::TrapScoreAll:
		Scores.TrapScore1P += Value;
		Scores.TrapScore2P += Value;
		break;
	case EScoreType::TrapScore1P:
		Scores.TrapScore1P += Value;
		break;
	case EScoreType::TrapScore2P:
		Scores.TrapScore2P += Value;
		break;
	case EScoreType::CollaborationScore:
		Scores.CollaborationScore += Value;
		break;
	case EScoreType::WaveLevel:
		++Scores.WaveLevel;
		break;
	default:
		UE_LOG(LogTrap, Error, TEXT("ATrapperGameState::AddScore Failed"));
		break;
	}
}

void ATrapperGameState::UpdateScoreHUD()
{
	/*Scores.MonsterScore1P = 1000;
	Scores.MonsterScore2P = 2000;
	Scores.TrapScore1P = 500;
	Scores.TrapScore2P = 1000;
	Scores.CollaborationScore = 50000;
	Scores.WaveLevel = 11;*/

	TArray<AActor*> PlayerControllers;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerController::StaticClass(), PlayerControllers);

	for (AActor* Actor : PlayerControllers)
	{
		ATrapperPlayerController* PlayerController = Cast<ATrapperPlayerController>(Actor);
		if (PlayerController)
		{
			PlayerController->ClientRPCUpdateScoreHUD(Scores);
		}
	}
}

void ATrapperGameState::S2SRegistMagneticPillar(class AMagneticPillar* Pillar)
{
	MagneticPillars.Add(Pillar);

	ActivatedPillarCount = MagneticPillars.Num();
	MagneticPillarMaxCount = MagneticPillars.Num();
	OnRep_MagneticPillarCountChanged();
}

void ATrapperGameState::PillarDeactivated(class AMagneticPillar* Pillar)
{
	if (!DeactivatedPillars.Contains(Pillar))
	{
		DeactivatedPillars.Add(Pillar);
	}

	ActivatedPillarCount = MagneticPillars.Num() - DeactivatedPillars.Num();
	OnRep_MagneticPillarCountChanged();
}

void ATrapperGameState::PillarBroken(class AMagneticPillar* Pillar)
{
	if (DeactivatedPillars.Contains(Pillar))
	{
		DeactivatedPillars.Remove(Pillar);
	}

	if (MagneticPillars.Contains(Pillar))
	{
		MagneticPillars.Remove(Pillar);
	}

	MagneticPillarMaxCount = MagneticPillars.Num();
	ActivatedPillarCount = MagneticPillars.Num() - DeactivatedPillars.Num();
	OnRep_MagneticPillarCountChanged();
}

void ATrapperGameState::S2SReactivateAllMagneticPillar()
{
	//UE_LOG(LogTemp, Warning, TEXT("Magnetic Pillar Active Function"));

	for (TObjectPtr<AMagneticPillar>& PillarPtr : MagneticPillars)
	{
		if (IsValid(PillarPtr))
		{
			PillarPtr->S2SSetLock(false);
		}
	}

	ActivatedPillarCount = MagneticPillars.Num();
	MagneticPillarMaxCount = MagneticPillars.Num();
	OnRep_MagneticPillarCountChanged();
}

void ATrapperGameState::S2SDeactivateAllMagneticPillar()
{
	for (TObjectPtr<AMagneticPillar>& PillarPtr : MagneticPillars)
	{
		if (IsValid(PillarPtr))
		{
			PillarPtr->S2SSetLock(true);
		}
	}
}

void ATrapperGameState::OnRep_MagneticPillarCountChanged()
{
	if (IsValid(PlayerHUDRef))
	{
		PlayerHUDRef->SetMagneticPillarCount(ActivatedPillarCount, MagneticPillarMaxCount);
	}
}

void ATrapperGameState::ChangeMonsterSpeed()
{
	//Spanwer->UpadateSpeed();
}

void ATrapperGameState::ChangeMonsterHealth()
{
	//Spanwer->UpdateHealth();
}

void ATrapperGameState::ChangeMonsterDamage()
{
	//Spanwer->UpdateDamage();
}

void ATrapperGameState::NextCanPlaceClapTrap(bool bCanPlace)
{
	bCanPlaceClapTrap = bCanPlace;

	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		APlayerController* PC = It->Get();
		ATrapperPlayerController* TrapperPlayerController = Cast<ATrapperPlayerController>(PC);
		if (TrapperPlayerController)
		{
			uint8 Num = 0;
			if (bCanPlaceClapTrap)
			{
				Num = 1;
			}
			else
			{
				Num = 0;
			}
			TrapperPlayerController->UpdateClapTrapNum(Num);
		}
	}
}
