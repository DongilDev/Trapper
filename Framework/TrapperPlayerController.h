// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "TrapperPlayerController.generated.h"

/**
 *
 */
UCLASS()
class TRAPPERPROJECT_API ATrapperPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	FTimerHandle EventStartHandle;
	UFUNCTION() void EventStart();

	void PlayerDeath(class ATrapperPlayer* Player);

	UFUNCTION() void Respawn();
	void EndGame();

	UFUNCTION(BlueprintImplementableEvent)
	void InitializeChatHUD();

	void InitializeWorldInfoHUD();

	UFUNCTION(Server, Reliable)
	void ServerRPCSetWorldInfo();

	UFUNCTION(Server, Reliable)
	void ServerRPCChangeReviveImmediatelyState();

	void SetWorldInfo();

	UFUNCTION(BlueprintImplementableEvent)
	void CreatePingInformationMessage(ESelectPingType pingType);

	class USystemMenuUMG* GetSystemMenuUMG() const;

private:
	void InitializeHUD();

public:
	// 게임오버/게임 클리어시 클라이언트가 먼저 나갔을 때에 대한 처리
	void EndGameReturnTitle();
	void SetClientDisconnected(bool Disconnected);
	bool GetClientDisconnected();

	

private:
	bool ClientDisconnected = false;
	bool ServerDisconnected = false;

public:
	/// ---------------------------------------------------------------
	///								   RPC
	/// ---------------------------------------------------------------
	UFUNCTION(Server, Reliable)
	void ServerRPCUpdatebIsClientStart(bool isClientStart);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPCUpdatebIsClientStart(bool isClientStart);

	UFUNCTION(Server, Reliable)
	void ServerRPCClientEndGame();

	UFUNCTION(Client, Reliable)
	void ClientRPCServerEndGame();


	/// ---------------------------------------------------------------
	///                            Variables
	/// ---------------------------------------------------------------
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	class UInputMappingContext* DefaultIMC;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	class UInputMappingContext* SystemMenuIMC;

	TObjectPtr<class ATrapperPlayer> ControlledPlayer;

	UPROPERTY(EditAnywhere)
	float RestartDelay = 5;

	UPROPERTY(Replicated)
	uint8 bIsClientStart : 1 = false;

private:
	TObjectPtr<class UEnhancedInputLocalPlayerSubsystem> SubSystem;
	uint8 bIsGameClear : 1 = false;


	/// ---------------------------------------------------------------
	///                            HUD
	/// ---------------------------------------------------------------
public:

	// Respawn
	UPROPERTY(EditAnywhere, Category = "Custom HUD")
	TSubclassOf<class UUserWidget> RespawnScreenClass;

	TObjectPtr<UUserWidget> RespawnScreen;
	FTimerHandle RespawnTimerHandle;
	void RecreateRespawnHUD();

	// Movie
	UPROPERTY(EditAnywhere, Category = "Custom HUD")
	TSubclassOf<class UMediaHUD> MovieScreenClass;

	TObjectPtr<UMediaHUD> MovieScreen;

	// Dialog
	UPROPERTY(EditAnywhere, Category = "Custom HUD")
	TSubclassOf<class UDialogHUD> DialogHUDClass;
	TObjectPtr<UDialogHUD> DialogHUDRef;

	// Game End
	UFUNCTION(NetMulticast, Reliable)
	void MulticastSetTimeDilation(float Value);

	UPROPERTY(EditAnywhere, Category = "Custom HUD")
	TSubclassOf<class UGameResultHUD> GameEndClass;
	TObjectPtr<UGameResultHUD> GameEndHUD;

	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPCGameEndScreenSetting(bool IsGameClear);
	void MulticastRPCGameEndScreenSetting_Implementation(bool IsGameClear);

	UFUNCTION(Client, Reliable)
	void ClientRPCGameEndShowScore(bool IsGameClear);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastGameRestart();

	// Trap
	UPROPERTY(EditDefaultsOnly, Category = "Custom HUD")
	TSubclassOf<class UTrapSelectHUD> TrapSelectHUDClass;
	TObjectPtr<UTrapSelectHUD> TrapSelectHUDRef;

	// Ping
	UPROPERTY(EditDefaultsOnly, Category = "Custom HUD")
	TSubclassOf<class UPingSelectHUD> PingSelectHUDClass;
	TObjectPtr<UPingSelectHUD> PingSelectHUDRef;

	// WorldMap
	UPROPERTY(EditDefaultsOnly, Category = "Custom HUD")
	TSubclassOf<class UWorldMapHUD> WorldMapHUDClass;
	TObjectPtr<UWorldMapHUD> WorldMapHUDRef;

	// MediaHUD
	UFUNCTION(NetMulticast, Reliable)
	void MulticastMediaText(uint8 Value);

	// MainHUD
	UPROPERTY(EditDefaultsOnly, Category = "Custom HUD")
	TSubclassOf<class UPlayerHUD> PlayerHudClass;

	UPROPERTY(BlueprintReadWrite, Category = "Custom HUD")
	TObjectPtr<UPlayerHUD> PlayerHudRef;

	// Upgrade HUD
	UPROPERTY(EditDefaultsOnly, Category = "Custom HUD")
	TSubclassOf<class UUpgradeHUD> UpgradeHudClass;

	UPROPERTY(BlueprintReadWrite, Category = "Custom HUD")
	TObjectPtr<UUpgradeHUD> UpgradeHudRef;

	// Multikill HUD
	UPROPERTY(EditDefaultsOnly, Category = "Custom HUD")
	TSubclassOf<class UMultikillHUD> MultikillHudClass;
	TObjectPtr<UMultikillHUD> MultikillHudRef;

	// Score HUD
	UPROPERTY(EditDefaultsOnly, Category = "Custom HUD")
	TSubclassOf<class UScoreHUD> ScoreHudClass;
	TObjectPtr<UScoreHUD> ScoreHudRef;

	// System UMG
	UPROPERTY(EditDefaultsOnly, Category = "Custom HUD")
	TSubclassOf<class USystemMenuUMG> SystemMenuUMGClass;
	TObjectPtr<USystemMenuUMG> SystemMenuUMGRef;

	//BossHP
	UPROPERTY(EditDefaultsOnly, Category = "Custom HUD")
	TSubclassOf<class UBossHealthBar> HPWidgetClass;
	TObjectPtr<UBossHealthBar> BossHealthBar;

	UFUNCTION(Client, Reliable)
	void ClientRPCUpdateMultikillHUD(uint8 KillCount);

	UFUNCTION(Client, Reliable)
	void ClientRPCUpdateScoreHUD(FScoreData Scores);

	UFUNCTION(Server, Reliable)
	void ServerRPCSetOwner();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPCPlayerHudShow(bool Value);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastSequenceHudShow(bool Value);


	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPCShowQuestLayout(bool Value);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPCShowWaveInfo(bool Value);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPCShowTutorialDialog(bool Value, int32 BackgroundIndex = 0, const FString& InDialog = TEXT(""));

	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPCWaveSetting(int32 CurrentWave, int32 MaxWave);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPCSetSkipIcon(bool Value);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastShowInvincibilityModeText(bool Value);

	UFUNCTION(Server, Reliable)
	void ServerRPCAfterJoinClientSetting();

	void SetPlayerHPBar();
	void SetTowerHPBar(float CurrentHP, float MaxHP);
	void SetSkipGauge();
	void SetWaveInfo();
	void SetQuestInfo(const FString& InTitle, const FString& InDescription);
	void SetItemCount(int32 Value);

	void ShowQuestLayout(bool Value);
	void ShowGodDialog(bool Value);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastSetGodDialog(int32 BackgroundIndex, const FString& InDialog);
	void SetGodDialog(int32 BackgroundIndex, FString InDialog);

	void ShowPlayerDialog(bool Value);
	void SetPlayerText(FString InName, FString InDialog);

	void ScoreAfterGameEndScreenSetting();

	UFUNCTION(Client, Reliable)
	void UpdateClapTrapNum(uint8 Num);

	/// ---------------------------------------------------------------
	///                           Trap
	/// ---------------------------------------------------------------
protected:
	virtual void ReceivedPlayer() override;

	UFUNCTION(Server, Reliable)
	void ServerRPCOnTrapperLevelLoaded();

	/// ---------------------------------------------------------------
	///                           SystemMenu
	/// ---------------------------------------------------------------
public:
	void HideSystemMenu();
	void BackOrSystemMenu();
	void SetInputModeGame();
	void SetInputModeGameAndUI();

	UFUNCTION(Server, Reliable)
	void ServerRPCTravelTitle();
	
	UFUNCTION(Client, Reliable)
	void ClientRPCTravelTitle();

	UFUNCTION(Server, Reliable)
	void ServerRPCFriendExitLobbyOrGame();

	UFUNCTION(Client, Reliable)
	void ClientRPCFriendExitLobbyOrGame();
};

