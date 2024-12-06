// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "Upgrade/UpgradeTable.h"
#include "Data/EventCode.h"
#include "TrapperGameState.generated.h"


UENUM(BlueprintType)
enum class EGameProgress : uint8
{
	Tutorial			UMETA(DisplayName = "Tutorial"),
	Maintenance			UMETA(DisplayName = "Maintenance"),
	Wave				UMETA(DisplayName = "Wave"),
	BossWave			UMETA(DisplayName = "BossWave"),
	BonusWave			UMETA(DisplayName = "BonusWave"),
	Awaken				UMETA(DisplayName = "Awaken"),
	GameClear			UMETA(DisplayName = "GameClear"),
	GameOver			UMETA(DisplayName = "GameOver"),
};

enum class EScoreType : uint32
{
	MonsterScoreAll,	// 1P2P 몬스터 점수
	MonsterScore1P,		// 1P 몬스터 점수
	MonsterScore2P,		// 2P 몬스터 점수
	TrapScoreAll,		// 1P2P 함정 점수
	TrapScore1P,		// 1P 함정 점수
	TrapScore2P,		// 2P 함정 점수
	CollaborationScore, // 협동 점수
	WaveLevel,			// 웨이브 단계 증가
};

USTRUCT()
struct FScoreData
{
	GENERATED_BODY()

	UPROPERTY()
	uint32 MonsterScore1P = 0;

	UPROPERTY()
	uint32 MonsterScore2P = 0;

	UPROPERTY()
	uint32 TrapScore1P = 0;

	UPROPERTY()
	uint32 TrapScore2P = 0;

	UPROPERTY()
	uint32 CollaborationScore = 0;

	UPROPERTY()
	uint8 WaveLevel = 0;
};

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnQuestExecute, int32, bool)
DECLARE_MULTICAST_DELEGATE_OneParam(FOnEventExecute, ETrapperEventCode)

UCLASS()
class TRAPPERPROJECT_API ATrapperGameState : public AGameStateBase
{
	GENERATED_BODY()

#pragma region Setting
	/// ------------------------------------------------------------------------
	///									Setting
	/// ------------------------------------------------------------------------
public:
	ATrapperGameState();
	virtual void HandleBeginPlay() override;
	virtual void OnRep_ReplicatedHasBegunPlay() override;

	/// Replication
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	///RPC
	UFUNCTION(Server, Reliable)
	void ServerRPCUpdatebIsClientStart(bool isClientStart);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPCUpdatebIsClientStart(bool isClientStart);

public:
	UPROPERTY(Replicated)
	bool bIsClientStart = false;
#pragma endregion Setting

#pragma region PlayerUI
	/// ------------------------------------------------------------------------
	///								  Player UI
	/// ------------------------------------------------------------------------
public:
	void SetPlayerHUDRef(class UPlayerHUD* Ref);

private:
	UPROPERTY() TObjectPtr<UPlayerHUD> PlayerHUDRef;

public:
	void SetWaveInfo();
	void SetWaveInfoRemainingTime(float RemainingTime);
#pragma endregion PlayerUI

#pragma region GameProgress
	/// ------------------------------------------------------------------------
	///								  Game Progress
	/// ------------------------------------------------------------------------
public:
	UPROPERTY(ReplicatedUsing = OnRep_GameProgressSetting, BlueprintReadOnly)
	EGameProgress CurrentGameProgress = EGameProgress::Tutorial;

	UFUNCTION()
	void OnRep_GameProgressSetting();

	FOnEventExecute OnEventExecute;

#pragma endregion GameProgress

#pragma region Maintenance

public:
	// 몇번째 정비시간인지 체크
	UPROPERTY(Replicated)
	int8 MaintenanceCount = -1; //-1;

	UPROPERTY(ReplicatedUsing = OnRep_TowerUpgrade)
	int8 TowerUpgradeLevel = 0;

	UFUNCTION()
	void OnRep_TowerUpgrade();

	UPROPERTY(ReplicatedUsing = OnRep_ChangeMaintenanceTimeLeft)
	uint32 MaintenanceTimeLeft;

	void SetMaintenanceTime(float Value);

	UFUNCTION()
	void OnRep_ChangeMaintenanceTimeLeft();

	UPROPERTY(ReplicatedUsing = OnRep_ChangeMaintenanceState)
	uint32 bMaintenanceInProgress : 1;

	UFUNCTION()
	void OnRep_ChangeMaintenanceState();

	UPROPERTY(Replicated)
	uint8 bTowerTutorial : 1;

#pragma endregion Maintenance

#pragma region Wave

public:

	UPROPERTY(ReplicatedUsing = OnRep_ChangeWaveTimeLeft)
	uint32 WaveTimeLeft;

	void SetWaveTime(float Value);

	UFUNCTION()
	void OnRep_ChangeWaveTimeLeft();

	UPROPERTY(Replicated)
	uint32 bWaveInProgress : 1;

	UPROPERTY(ReplicatedUsing = OnRep_ChangeWave)
	uint32 Wave = 0;

	UFUNCTION()
	void OnRep_ChangeWave();

	UPROPERTY(Replicated)
	uint32 SubWave = 1;

	UPROPERTY(Replicated)
	uint32 MaxWave = 20;

#pragma endregion Wave

#pragma region Monster State

public:
	void ChangeMonsterCount(int32 Count);
	void ChangeRemainingMonsterCount(int32 Count);
	void ClearRemainingMonsterCount();

public:
	UPROPERTY(ReplicatedUsing = OnRep_ChangeCurrentMonster)
	int32 CurrentMonsterCount = 0;

	UFUNCTION()
	void OnRep_ChangeCurrentMonster();

	UPROPERTY(ReplicatedUsing = OnRep_ChangeRemainingMonsterCount)
	int32 RemainingMonsterCount = 0;

	UFUNCTION()
	void OnRep_ChangeRemainingMonsterCount();

	void CheckAllWaveMonsterDie();

#pragma endregion Monster State

#pragma region Quest

	// 퀘스트 델리게이트
	FOnQuestExecute OnQuestExecute;

#pragma endregion Quest

#pragma region Item

public:
	void AddBone(int32 Count = 1);
	void UseBone(int32 Count);

	UFUNCTION()
	void OnRep_Bone();

	int32 GetBoneCount() { return Bone; }

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Item")
	int32 BoneMax = 9999;

	UPROPERTY(Replicated)
	uint8 bAbleActiveF : 1 = false;

private:
	UPROPERTY(ReplicatedUsing = "OnRep_Bone")
	int32 Bone = 0;

#pragma endregion Item

#pragma region Cheat

public:
	UPROPERTY(Replicated)
	uint8 bIsInvincibilityMode : 1;	

#pragma endregion Cheat


#pragma region Upgrade
	/// ------------------------------------------------------------------------
	///									Upgrade
	/// ------------------------------------------------------------------------
public:
	float ArrowDamage = 1.f;				// 화살 데미지 증감율
	float ArrowSpeed = 1.f;					// 화살 속도 증감율

	float MonsterMoveSpeed = 1.f;			// 몬스터 이동 속도 증감율
	float MonsterDetectRange = 1.f;			// 몬스터 감지 범위 증감율
	float MonsterHealth = 1.f;				// 몬스터 체력 증감율
	float MonsterAttackSpeed = 1.f;			// 몬스터 공격 속도 증감율
	float MonsterAttackDamage = 1.f;		// 몬스터 공격력 증감율

	// ------------------------------------------------------------------------
	//							Upgrade - Character
	// ------------------------------------------------------------------------

	UPROPERTY(ReplicatedUsing = OnRep_AdjustMoveSpeed)
	float MoveSpeed = 1.f;					// 이동속도 증감율
	UPROPERTY(ReplicatedUsing = OnRep_AdjustMagneticMoveSpeed)
	float MagneticMoveSpeed = 1.f;			// 자성이동속도 증감율
	UPROPERTY(ReplicatedUsing = OnRep_AdjustMagneticMoveAfterSpeed)
	float MagneticMoveAfterSpeed = 1.f;		// 자성이동 후 이동속도 증감율, 3초간 적용
	UPROPERTY(Replicated)
	float IncomingDamage = 1.f;				// 받는 데미지 증감율
	UPROPERTY(Replicated)
	float PlayerMaxHealth = 1.f;			// 플레이어 최대 체력 증감율

	UFUNCTION() void OnRep_AdjustMoveSpeed();
	UFUNCTION() void OnRep_AdjustMagneticMoveSpeed();
	UFUNCTION() void OnRep_AdjustMagneticMoveAfterSpeed();
	UFUNCTION() void AdjustPlayerMaxHealth();

	void UpgradePlayerAwaken(bool bIsServer);

	UPROPERTY(Replicated)
	float TrapCost = 1.f;					// 함정 재료 소모량 증감율
	UPROPERTY(Replicated)
	float TrapDamage = 1.f;					// 함정 데미지 증감율

	UPROPERTY(Replicated)
	float ChargeSpeed = 1.f;				// 차징속도 증감율

	UPROPERTY(Replicated)
	bool bIsRemovedArrowCharge = false;		// 화살 차징시간 삭제, 화살 데미지 30% 감소
	bool bIsArrowPenetrable = false;		// 화살 관통
	bool bIsRemovedHeadShoot = false;		// 헤드샷 판정이 사라지고 화살 데미지 30% 증가

	UPROPERTY(Replicated)
	bool bCanReviveImmediately1P = false;	// 다음에 죽을 시 딱 한번 즉시 부활
	UPROPERTY(Replicated)
	bool bCanReviveImmediately2P = false;	// 다음에 죽을 시 딱 한번 즉시 부활


public:
	// 업그레이드 받아오는 함수
	const TArray<FUpgradeTable>& GetUpgradeArray();
	const FPillarUpgradeTable GetMagneticPillarData();
	void SelectUpgrade(const uint8& index, const bool& bIsServer);
	bool IsUpgradeSelected(bool bIsServer);
	bool IsPillarReactivatedUpgradeSeletable();

private:
	void LoadUpgradeTable();
	void RemoveSelectedUpgradeAndSuffle();

	// 업그레이드 테이블
	UDataTable* NormalTable = nullptr;
	UDataTable* RareTable = nullptr;
	UDataTable* EpicTable = nullptr;
	UDataTable* AwakenTable = nullptr;
	UDataTable* PillarTable = nullptr;

	uint8 SelectedUpgradeIndexServer = 255;
	uint8 SelectedUpgradeIndexClient = 255;

	uint8 GetBinaryMaintenanceCount();
	UPROPERTY(Replicated) uint8 ServerSelect = 0b00000;
	UPROPERTY(Replicated) uint8 ClientSelect = 0b00000;
	//																				S	   C	 선택한건 배열에서 빼고 다시 셔플
	UPROPERTY(Replicated) TArray<FUpgradeTable> NormalUpgradeTableArray; /// 0 1 2 - 3 4 5
	UPROPERTY(Replicated) TArray<FUpgradeTable> RareUpgradeTableArray;	 /// 0 1 2 - 3 4 5
	UPROPERTY(Replicated) TArray<FUpgradeTable> EpicUpgradeTableArray;	 /// 0 1 2 - 3 4 5
	UPROPERTY(Replicated) TArray<FUpgradeTable> AwakenUpgradeTableArray;
	UPROPERTY(Replicated) TArray<FPillarUpgradeTable> PillarUpgradeTableArray;

	template<typename T>
	void ShuffleArray(TArray<T>& Array)
	{
		int32 LastIndex = Array.Num() - 1;

		for (int32 i = LastIndex; i > 0; --i)
		{
			int32 RandomIndex = FMath::RandRange(0, i);
			if (i != RandomIndex)
			{
				Array.Swap(i, RandomIndex);
			}
		}
	}
#pragma endregion Upgrade

#pragma region Trap
	/// ------------------------------------------------------------------------
	///									Trap
	/// ------------------------------------------------------------------------
public:
	FORCEINLINE bool IsCanPlaceClapTrap() const { return bCanPlaceClapTrap; }
	void NextCanPlaceClapTrap(bool bCanPlace);

private:
	void TrapUpgradeDamage();
	void TrapUpgradeCost();

private:
	UPROPERTY(Replicated)
	uint8 bCanPlaceClapTrap : 1 = true;

#pragma endregion Trap

#pragma region Multikill
	/// ------------------------------------------------------------------------
	///									Multikill
	/// ------------------------------------------------------------------------
public:
	void RegisterKill(AActor* DamageCauser);
	void ResetKillCount(AActor* DamageCauser);

	void CheckKillCount(AController* EventInstigator, AActor* DamageCauser);
	void CheckAndResetKillCount(AController* EventInstigator, AActor* DamageCauser);

	void CheckKillCountCollaboration(AActor* DamageCauser);
	void CheckAndResetKillCountCollaboration(AActor* DamageCauser);

	void CheckAndResetKillCountCollaborationForClapTrap(AActor* DamageCauser, AActor* OtherClap);

private:
	UPROPERTY() TMap<TWeakObjectPtr<AActor>, uint8> KillCountMap;
#pragma endregion Multikill

#pragma region Score
	/// ------------------------------------------------------------------------
	///									Score
	/// ------------------------------------------------------------------------
public:
	void AddScore(EScoreType ScoreType, uint32 Value = 0);
	void UpdateScoreHUD();

private:
	FScoreData Scores;

	/*uint32 ContributionPercent1P = 0;
	uint32 ContributionPercent2P = 0;
	uint8 ContributionBar = 0;
	uint32 TotalScore = 0;
	uint32 RankScore = 0;*/
#pragma endregion Score


#pragma region MagneticPillar
	/// ------------------------------------------------------------------------
	///									자성기둥
	/// ------------------------------------------------------------------------
public:
	void S2SRegistMagneticPillar(class AMagneticPillar* Pillar);
	void PillarDeactivated(class AMagneticPillar* Pillar);
	void PillarBroken(class AMagneticPillar* Pillar);

	void S2SReactivateAllMagneticPillar();
	void S2SDeactivateAllMagneticPillar(); //디버깅용 임시

private:
	UPROPERTY() TArray<TObjectPtr<AMagneticPillar>> MagneticPillars;
	UPROPERTY() TArray<TObjectPtr<AMagneticPillar>> DeactivatedPillars;

	UPROPERTY(ReplicatedUsing = OnRep_MagneticPillarCountChanged)
	int32 ActivatedPillarCount = 0;

	UPROPERTY(ReplicatedUsing = OnRep_MagneticPillarCountChanged)
	int32 MagneticPillarMaxCount = 0;

	UFUNCTION()
	void OnRep_MagneticPillarCountChanged();
#pragma endregion MagneticPillar

#pragma region Monster
	/// ------------------------------------------------------------------------
	///									Monster
	/// ------------------------------------------------------------------------
public:
	void ChangeMonsterSpeed();
	void ChangeMonsterHealth();
	void ChangeMonsterDamage();


private:
	TObjectPtr<class ASpawner> Spanwer;

#pragma endregion Monster
};

