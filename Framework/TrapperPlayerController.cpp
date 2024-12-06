// Fill out your copyright notice in the Description page of Project Settings.


#include "TrapperPlayerController.h"
#include "EnhancedInputSubSystems.h"
#include "TimerManager.h"
#include "Character/TrapperPlayer.h"
#include "Net/UnrealNetwork.h"
#include "TrapperGameState.h"
#include "Kismet/GameplayStatics.h"
#include "Framework/TrapperGameMode.h"
#include "Quest/QuestManager.h"
#include "Dialog/DialogManager.h"
#include "Trap/Component/TrapMechanics.h"
#include "Tower/Tower.h"
#include "EngineUtils.h"
#include "Ping/PingMechanics.h"
#include "Map/Mapmechanics.h"

#include "Blueprint/UserWidget.h"
#include "Widget/TrapSelectHUD.h"
#include "Widget/PlayerHUD.h"
#include "Widget/PingSelectHUD.h"
#include "Widget/GameResultHUD.h"
#include "Widget/WorldMapHUD.h"
#include "Widget/UpgradeHUD.h"
#include "Widget/MultikillHUD.h"
#include "Widget/ScoreHUD.h"
#include "Widget/SystemMenuUMG.h"
#include "Widget/Options/OptionsUMG.h"
#include "Widget/WaveInfoWidget.h"
#include "Widget/DialogHUD.h"
#include "Widget/MediaHUD.h"
#include "Widget/BossHealthBar.h"

#include "Framework/TrapperGameMode.h"
#include "Sequence/SequenceManager.h"

#include "TrapperProject.h"
#include "Monster/BaseMonster.h"
#include "Trap/TrapBase.h"
#include "TrapperGameInstance.h"
#include "Components/TextBlock.h"


void ATrapperPlayerController::BeginPlay()
{
	Super::BeginPlay();

	SubSystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	if (SubSystem)
	{
		if(DefaultIMC) SubSystem->AddMappingContext(DefaultIMC, 0);
		if(SystemMenuIMC) SubSystem->AddMappingContext(SystemMenuIMC, 1);
	}

	if (!HasAuthority())
	{
		ServerRPCUpdatebIsClientStart(true);
	}

	if (HasAuthority() && IsLocalController())
	{
		for (AQuestManager* GetQuestManager : TActorRange<AQuestManager>(GetWorld()))
		{
			GetQuestManager->SetOwner(this);
		}
	}
	else if (!HasAuthority() && IsLocalController())
	{
		ServerRPCSetOwner();
	}

	InitializeHUD();

	ClientDisconnected = false;
	ServerDisconnected = false;
}


void ATrapperPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ATrapperPlayerController, bIsClientStart);
}

void ATrapperPlayerController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	if (MultikillHudRef)
	{
		MultikillHudRef->AllClearTimer();
	}

	GetWorld()->GetTimerManager().ClearAllTimersForObject(this);
}

void ATrapperPlayerController::InitializeHUD()
{
	if (!IsLocalController())
		return;

	/// Z-Order 999 ----------------------------------------------------

	// Movie HUD
	if (IsValid(MovieScreenClass))
	{
		MovieScreen = CreateWidget<UMediaHUD>(this, MovieScreenClass);

		if (MovieScreen)
		{
			MovieScreen->AddToViewport(999);
			MovieScreen->SetVisibility(ESlateVisibility::Collapsed);
		}
	}

	// Respawn HUD
	if (IsValid(RespawnScreenClass))
	{
		RespawnScreen = CreateWidget(this, RespawnScreenClass);

		if (RespawnScreen)
		{
			RespawnScreen->AddToViewport(999);
			RespawnScreen->SetVisibility(ESlateVisibility::Collapsed);
		}
	}

	// Game End HUD
	if (IsValid(GameEndClass))
	{
		GameEndHUD = CreateWidget<UGameResultHUD>(this, GameEndClass);

		if (GameEndHUD)
		{
			GameEndHUD->AddToViewport(999);
			GameEndHUD->SetVisibility(ESlateVisibility::Collapsed);
		}
	}

	// Score
	if (IsValid(ScoreHudClass))
	{
		ScoreHudRef = CreateWidget<UScoreHUD>(this, ScoreHudClass);
		if (ScoreHudRef)
		{
			ScoreHudRef->AddToViewport(999);
			ScoreHudRef->SetVisibility(ESlateVisibility::Collapsed);
		}
	}

	// SystemMenu
	if (IsValid(SystemMenuUMGClass))
	{
		SystemMenuUMGRef = CreateWidget<USystemMenuUMG>(this, SystemMenuUMGClass);
		if (SystemMenuUMGRef)
		{
			SystemMenuUMGRef->AddToViewport(999);
			SystemMenuUMGRef->SetVisibility(ESlateVisibility::Collapsed);
			//OptionsUMGRef->SetVisibility(ESlateVisibility::Visible);
		}
	}

	/// -----------------------------------------------------------------------

	// Trap HUD
	if (IsValid(TrapSelectHUDClass))
	{
		TrapSelectHUDRef = CreateWidget<UTrapSelectHUD>(this, TrapSelectHUDClass);
		if (TrapSelectHUDRef)
		{
			TrapSelectHUDRef->AddToViewport(8);
			TrapSelectHUDRef->SetVisibility(ESlateVisibility::Collapsed);
		}
	}

	// Dialog HUD
	if (IsValid(DialogHUDClass))
	{
		DialogHUDRef = CreateWidget<UDialogHUD>(this, DialogHUDClass);
		if (DialogHUDRef)
		{
			DialogHUDRef->AddToViewport(9);
			DialogHUDRef->SetVisibility(ESlateVisibility::Visible);
		}
	}

	// MainHUD
	if (IsValid(PlayerHudClass))
	{
		PlayerHudRef = CreateWidget<UPlayerHUD>(this, PlayerHudClass);
		if (PlayerHudRef)
		{
			PlayerHudRef->AddToViewport();
			PlayerHudRef->SetVisibility(ESlateVisibility::Visible);
			PlayerHudRef->SetHealthUI();
			ATrapperGameState* gs = Cast<ATrapperGameState>(GetWorld()->GetGameState());
			if (gs)
			{
				gs->SetPlayerHUDRef(PlayerHudRef);
			}
		}
	}

	// Ping HUD
	if (IsValid(PingSelectHUDClass))
	{
		PingSelectHUDRef = CreateWidget<UPingSelectHUD>(this, PingSelectHUDClass);
		if (PingSelectHUDRef)
		{
			PingSelectHUDRef->AddToViewport(10);
			PingSelectHUDRef->SetVisibility(ESlateVisibility::Collapsed);
		}
	}

	// WorldMap
	if (IsValid(WorldMapHUDClass))
	{
		WorldMapHUDRef = CreateWidget<UWorldMapHUD>(this, WorldMapHUDClass);
		if (WorldMapHUDRef)
		{
			WorldMapHUDRef->AddToViewport(10);
			WorldMapHUDRef->SetVisibility(ESlateVisibility::Collapsed);
		}
	}

	// Upgrade
	if (IsValid(UpgradeHudClass))
	{
		UpgradeHudRef = CreateWidget<UUpgradeHUD>(this, UpgradeHudClass);
		if (UpgradeHudRef)
		{
			UpgradeHudRef->AddToViewport(11);
			UpgradeHudRef->SetVisibility(ESlateVisibility::Collapsed);
		}
	}

	// Muiikill
	if (IsValid(MultikillHudClass))
	{
		MultikillHudRef = CreateWidget<UMultikillHUD>(this, MultikillHudClass);
		if (MultikillHudRef)
		{
			MultikillHudRef->AddToViewport(11);
			MultikillHudRef->SetVisibility(ESlateVisibility::Collapsed);
		}
	}

	//HP HUD
	if (IsValid(HPWidgetClass))
	{
		BossHealthBar = CreateWidget<UBossHealthBar>(this, HPWidgetClass);
		if (BossHealthBar)
		{
			BossHealthBar->AddToViewport(12);
			BossHealthBar->SetVisibility(ESlateVisibility::Collapsed);
		}
	}

	InitializeChatHUD();
	InitializeWorldInfoHUD();

	ServerRPCAfterJoinClientSetting();
}

void ATrapperPlayerController::RecreateRespawnHUD()
{
	RespawnScreen->RemoveFromParent();
	RespawnScreen->ConditionalBeginDestroy();
	RespawnScreen = nullptr;

	if (IsValid(RespawnScreenClass))
	{
		RespawnScreen = CreateWidget(this, RespawnScreenClass);

		if (RespawnScreen)
		{
			RespawnScreen->AddToViewport(999);
			RespawnScreen->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
}

void ATrapperPlayerController::MulticastRPCPlayerHudShow_Implementation(bool Value)
{
	if (IsLocalController() && PlayerHudRef)
	{
		if (Value)
		{
			PlayerHudRef->SetVisibility(ESlateVisibility::Visible);
		}
		else
		{
			PlayerHudRef->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
}

void ATrapperPlayerController::MulticastSequenceHudShow_Implementation(bool Value)
{
	if (IsLocalController() && PlayerHudRef)
	{
		if (Value)
		{
			MovieScreen->SetVisibility(ESlateVisibility::Visible);
		}
		else
		{
			MovieScreen->SetVisibility(ESlateVisibility::Collapsed);
		}

		ATrapperPlayer* PlayerRef = Cast<ATrapperPlayer>(GetPawn());
		if (PlayerRef)
		{
			PlayerRef->bRequiredMediaSkip = false;
			UE_LOG(LogTemp, Warning, TEXT("Skip Abled : %d"), PlayerRef->bRequiredMediaSkip);
		}
	}
}

void ATrapperPlayerController::MulticastRPCShowWaveInfo_Implementation(bool Value)
{
	if (IsLocalController() && PlayerHudRef)
	{
		if (Value)
		{
			PlayerHudRef->WaveInfo->SetVisibility(ESlateVisibility::Visible);
		}
		else
		{
			PlayerHudRef->WaveInfo->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
}

void ATrapperPlayerController::MulticastShowInvincibilityModeText_Implementation(bool Value)
{
	if (IsLocalController() && PlayerHudRef)
	{
		if (Value)
		{
			PlayerHudRef->ShowInvincibilityModeText(true);
		}
		else
		{
			PlayerHudRef->ShowInvincibilityModeText(false);
		}
	}
}

void ATrapperPlayerController::SetPlayerHPBar()
{
	if (IsLocalController() && PlayerHudRef)
	{
		PlayerHudRef->SetHealthUI();
	}
}

void ATrapperPlayerController::SetTowerHPBar(float CurrentHP, float MaxHP)
{
	if (IsLocalController() && PlayerHudRef)
	{
		PlayerHudRef->SetTowerHealth(CurrentHP, MaxHP);
	}
}

void ATrapperPlayerController::SetSkipGauge()
{
	if (IsLocalController() && PlayerHudRef)
	{
		PlayerHudRef->SetSkipGauge();
	}
}

void ATrapperPlayerController::SetItemCount(int32 Value)
{
	if (IsLocalController() && PlayerHudRef)
	{
		PlayerHudRef->SetItemCount(Value);
	}
}

void ATrapperPlayerController::ShowQuestLayout(bool Value)
{
	if (IsLocalController() && PlayerHudRef)
	{
		PlayerHudRef->ShowQuestLayout(Value);
	}
}

void ATrapperPlayerController::ShowGodDialog(bool Value)
{
	if (IsLocalController() && DialogHUDRef)
	{
		DialogHUDRef->ShowGodDialog(Value);
	}
}

void ATrapperPlayerController::MulticastSetGodDialog_Implementation(int32 BackgroundIndex, const FString& InDialog)
{
	SetGodDialog(BackgroundIndex, InDialog);
}

void ATrapperPlayerController::SetGodDialog(int32 BackgroundIndex, FString InDialog)
{
	if (IsLocalController() && DialogHUDRef)
	{
		DialogHUDRef->SetGodDialog(BackgroundIndex, InDialog);
	}
}

void ATrapperPlayerController::MulticastRPCShowQuestLayout_Implementation(bool Value)
{
	if (IsLocalController() && PlayerHudRef)
	{
		PlayerHudRef->ShowQuestLayout(Value);
	}
}

void ATrapperPlayerController::MulticastRPCShowTutorialDialog_Implementation(
	bool Value, int32 BackgroundIndex, const FString& InDialog)
{
	if (IsLocalController() && DialogHUDRef)
	{
		DialogHUDRef->SetGodDialog(BackgroundIndex, InDialog);
		DialogHUDRef->ShowGodDialog(Value);
	}
}

void ATrapperPlayerController::ShowPlayerDialog(bool Value)
{
	if (IsLocalController() && IsValid(DialogHUDRef))
	{
		DialogHUDRef->ShowPlayerDialog(Value);
	}
}

void ATrapperPlayerController::SetPlayerText(FString InName, FString InDialog)
{
	if (IsLocalController() && DialogHUDRef)
	{
		DialogHUDRef->SetPlayerText(InName, InDialog);
	}
}

void ATrapperPlayerController::ScoreAfterGameEndScreenSetting()
{
	ScoreHudRef->SetVisibility(ESlateVisibility::Collapsed);
	GameEndHUD->SetVisibility(ESlateVisibility::Visible);
	GameEndHUD->SetGameEndText(bIsGameClear);
}

void ATrapperPlayerController::UpdateClapTrapNum_Implementation(uint8 Num)
{
	FString NumString = FString::Printf(TEXT("%d/1"), Num);
	TrapSelectHUDRef->ClapText->SetText(FText::FromString(NumString));
}

void ATrapperPlayerController::HideSystemMenu()
{
	if (!UpgradeHudRef->IsVisible())
	{
		SetShowMouseCursor(false);
		FInputModeGameOnly InputMode;
		SetInputMode(InputMode);
		SubSystem->AddMappingContext(DefaultIMC, 0);
	}
	else
	{
		ATrapperPlayer* TP =  Cast<ATrapperPlayer>(GetPawn());
		if (TP)
		{
			TP->SetLookInput(false);
		}
		FInputModeGameOnly InputMode;
		SetInputMode(InputMode);
		SubSystem->AddMappingContext(DefaultIMC, 0);
	}
}

void ATrapperPlayerController::ReceivedPlayer()
{
	Super::ReceivedPlayer();

	if (HasAuthority())
	{
		return;
	}

	// 특정 레벨에 접속했을 때
	if (GetWorld()->GetMapName().Contains("Trapper"))
	{
		ServerRPCOnTrapperLevelLoaded();
	}
}

void ATrapperPlayerController::ServerRPCOnTrapperLevelLoaded_Implementation()
{
	//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan, FString::Printf(TEXT("ServerRPCOnTrapperLevelLoaded_Implementation")));

	AGameModeBase* GameMode = UGameplayStatics::GetGameMode(GetWorld());
	if (GameMode)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan, FString::Printf(TEXT("GameMode True")));
		ATrapperGameMode* TrapperGameMode = Cast<ATrapperGameMode>(GameMode);
		if (TrapperGameMode)
		{
			//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan, FString::Printf(TEXT("TrapperGameMode True")));
			TrapperGameMode->OnTrapperLevelLoaded();

			GetWorld()->GetTimerManager().SetTimer(EventStartHandle, this,
				&ATrapperPlayerController::EventStart, 1.f, false, 3.f);
		}
	}
}

void ATrapperPlayerController::ServerRPCAfterJoinClientSetting_Implementation()
{
	// 클라이언트 접속 이후 서버의 로컬 캐릭터에서 처리해야할 것
	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		ATrapperPlayerController* PC = Cast<ATrapperPlayerController>(Iterator->Get());
		if (PC && PC->IsLocalController())
		{
			PC->SetPlayerHPBar();
		}
	}
}

void ATrapperPlayerController::SetQuestInfo(const FString& InTitle, const FString& InDescription)
{
	if (IsLocalController() && PlayerHudRef)
	{
		PlayerHudRef->SetQuestText(InTitle, InDescription);
	}
}

void ATrapperPlayerController::MulticastRPCSetSkipIcon_Implementation(bool Value)
{
	if (IsLocalController() && PlayerHudRef)
	{
		PlayerHudRef->SetSkipIcon(Value);
	}
}

void ATrapperPlayerController::MulticastRPCWaveSetting_Implementation(int32 CurrentWave, int32 MaxWave)
{
	if (IsLocalController() && PlayerHudRef)
	{
		PlayerHudRef->SetWaveInfo(CurrentWave, MaxWave);
	}
}

void ATrapperPlayerController::MulticastSetTimeDilation_Implementation(float Value)
{
	GetWorldSettings()->SetTimeDilation(Value);
}

void ATrapperPlayerController::MulticastRPCGameEndScreenSetting_Implementation(bool IsGameClear)
{
	if (IsLocalController())
	{
		PlayerHudRef->SetVisibility(ESlateVisibility::Collapsed);

		GameEndHUD->SetVisibility(ESlateVisibility::Visible);
		GameEndHUD->SetGameEndText(IsGameClear);
		EndGame();
	}
}

void ATrapperPlayerController::MulticastMediaText_Implementation(uint8 Value)
{
	if (IsLocalController())
	{
		MovieScreen->SetSkipText(Value);
	}
}

void ATrapperPlayerController::MulticastGameRestart_Implementation()
{
	if(!IsLocalController()) return;

	PlayerHudRef->SetVisibility(ESlateVisibility::Visible);
	GameEndHUD->SetVisibility(ESlateVisibility::Collapsed);
	ScoreHudRef->SetVisibility(ESlateVisibility::Collapsed);

	ATrapperPlayer* LocalPlayer = Cast<ATrapperPlayer>(GetPawn());
	if (LocalPlayer)
	{
		LocalPlayer->UnblockBow();
		LocalPlayer->TrapMechanics->UnBlockSelectTrap();
	}

	FInputModeGameOnly InputMode;
	SetInputMode(InputMode);
	SetShowMouseCursor(false);
}

void ATrapperPlayerController::ClientRPCGameEndShowScore_Implementation(bool IsGameClear)
{
	bIsGameClear = IsGameClear;
	PlayerHudRef->SetVisibility(ESlateVisibility::Collapsed);
	ScoreHudRef->SetVisibility(ESlateVisibility::Visible);

	ATrapperPlayer* LocalPlayer = Cast<ATrapperPlayer>(GetPawn());
	if (LocalPlayer)
	{
		//LocalPlayer->DisableInput(this);
		LocalPlayer->BlockBow();
		LocalPlayer->TrapMechanics->BlockSelectTrap();
	}

	FInputModeUIOnly InputMode;
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::LockAlways);
	SetInputMode(InputMode);
	SetShowMouseCursor(true);
}

void ATrapperPlayerController::EventStart()
{
	ATrapperGameMode* TrapperGameMode = Cast<ATrapperGameMode>(GetWorld()->GetAuthGameMode<ATrapperGameMode>());
	if (TrapperGameMode)
	{
		TrapperGameMode->GameStart();
	}
}

void ATrapperPlayerController::PlayerDeath(ATrapperPlayer* PlayerRef)
{
	if (!IsLocalController()) return;

	ControlledPlayer = PlayerRef;
	ATrapperGameState* GameState = GetWorld()->GetGameState<ATrapperGameState>();

	// 1P의 경우
	if (PlayerRef->HasAuthority() && PlayerRef->IsLocallyControlled())
	{
		// 즉시부활 가능 상태일시 즉시 리스폰
		if (GameState->bCanReviveImmediately1P)
		{
			ControlledPlayer->MulticastCharacterRespawn();
			GameState->bCanReviveImmediately1P = false;
			return;
		}
	}
	// 2P의 경우
	else if (!PlayerRef->HasAuthority() && PlayerRef->IsLocallyControlled())
	{
		// 즉시부활 가능 상태일시 즉시 리스폰
		if (GameState->bCanReviveImmediately2P)
		{
			ControlledPlayer->ServerRPCCharacterRespawn();
			ServerRPCChangeReviveImmediatelyState();
			return;
		}
	}

	ControlledPlayer->DisableInput(this);

	if (IsValid(RespawnScreen))
	{
		RespawnScreen->SetVisibility(ESlateVisibility::Visible);
	}

	// 리스폰 타이머 설정
	GetWorldTimerManager().SetTimer(RespawnTimerHandle, this,
		&ATrapperPlayerController::Respawn, 1.0f, false, RestartDelay);
}

void ATrapperPlayerController::Respawn()
{
	if (IsValid(RespawnScreen) && IsValid(ControlledPlayer))
	{
		RecreateRespawnHUD();
		ControlledPlayer->EnableInput(this);

		if (ControlledPlayer->HasAuthority())
		{
			ControlledPlayer->MulticastCharacterRespawn();
		}
		else if (ControlledPlayer->IsLocallyControlled())
		{
			ControlledPlayer->ServerRPCCharacterRespawn();
		}
	}
}

void ATrapperPlayerController::EndGame()
{
	ATrapperPlayer* LocalPlayer = Cast<ATrapperPlayer>(GetPawn());
	if (LocalPlayer)
	{
		LocalPlayer->DisableInput(this);
		LocalPlayer->BlockBow();
		LocalPlayer->TrapMechanics->BlockSelectTrap();
	}

	SetShowMouseCursor(true);
}

void ATrapperPlayerController::InitializeWorldInfoHUD()
{
	// Tower Initialize
	TArray<AActor*> OutActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATower::StaticClass(), OutActors);

	for (const auto& Actor : OutActors)
	{
		ATower* Tower = Cast<ATower>(Actor);

		if (Tower)
		{
			SetTowerHPBar(Tower->TowerHP, Tower->MaxHP);
		}
	}

	// Quest & Wave Initialize
	if (HasAuthority())
	{
		SetWorldInfo();
	}
	else
	{
		ServerRPCSetWorldInfo();
	}
}

void ATrapperPlayerController::ServerRPCChangeReviveImmediatelyState_Implementation()
{
	ATrapperGameState* GameState = GetWorld()->GetGameState<ATrapperGameState>();
	GameState->bCanReviveImmediately2P = false;
}

void ATrapperPlayerController::SetWorldInfo()
{
	ATrapperGameMode* GameMode = Cast<ATrapperGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	//GameMode->QuestManager->RequireSetQuestUI();
}

USystemMenuUMG* ATrapperPlayerController::GetSystemMenuUMG() const
{
	return SystemMenuUMGRef;
}

void ATrapperPlayerController::BackOrSystemMenu()
{
	if (SystemMenuUMGRef->IsVisible())
	{
		SystemMenuUMGRef->OnBack();
	}
	else
	{
		ATrapperPlayer* TP = Cast<ATrapperPlayer>(GetPawn());
		if (IsValid(TP))
		{
			if (TP->MapMechanics->GetWorldMapHUD()->IsVisible())
			{
				TP->MapMechanics->CloseWorldMap();
				return;
			}
			TP->PingMechanics->Cancel();
			TP->TrapMechanics->ExitSwitchSelectMode();
			TP->TrapMechanics->ReleaseInstallationMode();
			TP->MapMechanics->PlaceMarkerCancel();
			TP->BlockBow();
		}

		SystemMenuUMGRef->OnMouseClickSound();

		SubSystem->RemoveMappingContext(DefaultIMC);
		SystemMenuUMGRef->SetVisibility(ESlateVisibility::Visible);
		SetShowMouseCursor(true);
	}
}

void ATrapperPlayerController::SetInputModeGame()
{
	FInputModeGameOnly InputMode;
	SetInputMode(InputMode);
}

void ATrapperPlayerController::SetInputModeGameAndUI()
{
	FInputModeGameAndUI InputMode;
	InputMode.SetHideCursorDuringCapture(false);
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::LockAlways);
	SetInputMode(InputMode);
}

void ATrapperPlayerController::ServerRPCTravelTitle_Implementation()
{
	ATrapperGameMode* GameMode = Cast<ATrapperGameMode>(GetWorld()->GetAuthGameMode());
	if (GameMode)
	{
		GameMode->OnServerPlayerTitleTravle();
	}

	if (IsLocalController())
	{
		uint8 PlayerNum = GetWorld()->GetNumPlayerControllers();
		if (PlayerNum == 1)
		{
			UGameplayStatics::OpenLevel(this, "Title");
		}

		for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
		{
			APlayerController* PlayerController = It->Get();
			ATrapperPlayerController* TrapperPlayerCotroller = Cast<ATrapperPlayerController>(PlayerController);
			if (TrapperPlayerCotroller && TrapperPlayerCotroller != this)
			{
				TrapperPlayerCotroller->ClientTravel("Title", TRAVEL_Absolute);
			}
		}
	}
	else
	{
		ClientTravel("Title", TRAVEL_Absolute);
	}
}

void ATrapperPlayerController::ClientRPCTravelTitle_Implementation()
{
	UGameplayStatics::OpenLevel(this, "Title");
}

void ATrapperPlayerController::ServerRPCFriendExitLobbyOrGame_Implementation()
{
	UTrapperGameInstance* TrapperGameInstance = Cast<UTrapperGameInstance>(GetGameInstance());
	if (TrapperGameInstance)
	{
		TrapperGameInstance->SetFriendExitLobbyOrGame(true);
		TrapperGameInstance->SetExitFriendName(TEXT("디아나"));
	}
}

void ATrapperPlayerController::ClientRPCFriendExitLobbyOrGame_Implementation()
{
	UTrapperGameInstance* TrapperGameInstance = Cast<UTrapperGameInstance>(GetGameInstance());
	if (TrapperGameInstance)
	{
		TrapperGameInstance->SetFriendExitLobbyOrGame(true);
		TrapperGameInstance->SetExitFriendName(TEXT("리아나"));
	}
}

void ATrapperPlayerController::ClientRPCUpdateMultikillHUD_Implementation(uint8 KillCount)
{
	//UE_LOG(LogTrap, Warning, TEXT("ClientRPCUpdateMultikillHUD_Implementation"));

	if (MultikillHudRef)
	{
		MultikillHudRef->SetMultikillCount(KillCount);
	}
}

void ATrapperPlayerController::ClientRPCUpdateScoreHUD_Implementation(FScoreData Scores)
{
	//UE_LOG(LogTrap, Warning, TEXT("ClientRPCUpdateScoreHUD_Implementation"));

	if (ScoreHudRef)
	{
		ScoreHudRef->UpdateScoreHUD(Scores);
	}
}

void ATrapperPlayerController::ServerRPCSetOwner_Implementation()
{
	for (AQuestManager* GetQuestManager : TActorRange<AQuestManager>(GetWorld()))
	{
		GetQuestManager->SetOwner(this);
	}
}

void ATrapperPlayerController::ServerRPCSetWorldInfo_Implementation()
{
	SetWorldInfo();
}

void ATrapperPlayerController::EndGameReturnTitle()
{
	ServerRPCTravelTitle();

	//if (!HasAuthority())
	//{
	//	if (ServerDisconnected)
	//	{
	//		//UGameplayStatics::OpenLevel(this, TEXT("Title"));
	//	}
	//	else
	//	{
	//		ServerRPCClientEndGame();
	//		ClientTravel("Title", TRAVEL_Absolute);
	//	}
	//}
	//else
	//{
	//	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	//	{
	//		ATrapperPlayerController* PC = Cast<ATrapperPlayerController>(Iterator->Get());
	//		if (PC && !(PC->IsLocalController()))
	//		{
	//			PC->ClientRPCServerEndGame();
	//		}
	//	}

	//	// 서버에서 레벨 전환
	//	//ClientTravel("Title", TRAVEL_Absolute);
	//	GetWorld()->ServerTravel("Title", true);  // true로 세팅하면 서버가 종료되고 클라이언트와 새로운 세션 시작
	//}
}

void ATrapperPlayerController::SetClientDisconnected(bool Disconnected)
{
	if (HasAuthority())
	{
		ClientDisconnected = Disconnected;
	}
}

bool ATrapperPlayerController::GetClientDisconnected()
{
	return ClientDisconnected;
}

/// -------------------------------------------------------------------------------------------
///										   RPC
/// -------------------------------------------------------------------------------------------

void ATrapperPlayerController::ServerRPCUpdatebIsClientStart_Implementation(bool isClientStart)
{
	MulticastRPCUpdatebIsClientStart(true);
}

void ATrapperPlayerController::MulticastRPCUpdatebIsClientStart_Implementation(bool isClientStart)
{
	bIsClientStart = true;
}

void ATrapperPlayerController::ServerRPCClientEndGame_Implementation()
{
	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		ATrapperPlayerController* PC = Cast<ATrapperPlayerController>(Iterator->Get());
		if (PC)
		{
			if (HasAuthority())
			{
				PC->SetClientDisconnected(true);
			}
		}

	}
}

void ATrapperPlayerController::ClientRPCServerEndGame_Implementation()
{
	ServerDisconnected = true;
}