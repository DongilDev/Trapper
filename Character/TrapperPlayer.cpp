// Fill out your copyright notice in the Description page of Project Settings.


#include "TrapperPlayer.h"

#include "Components/CapsuleComponent.h"
#include "Components/BoxComponent.h"
#include "Components/TextBlock.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Components/AudioComponent.h"
#include "Components/SplineComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/PlayerState.h"
#include "TrapperProject.h"
#include "EngineUtils.h"
#include "EnhancedInputComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Blueprint/UserWidget.h"
#include "Net/UnrealNetwork.h"
#include "Animation/AnimMontage.h"
#include "TimerManager.h"
#include "DrawDebugHelpers.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"

#include "GameElement/Item.h"
#include "GameElement/MagneticPillar.h"
#include "Framework/TrapperPlayerState.h"
#include "Framework/TrapperPlayerController.h"
#include "Framework/TrapperGameMode.h"
#include "Framework/TrapperGameState.h"
#include "Character/TrapperPlayerMovementComponent.h"
#include "Widget/PlayerHUD.h"
#include "Widget/TrapSelectHUD.h"
#include "Widget/SystemMenuUMG.h"
#include "Quest/Interact.h"
#include "Quest/QuestManager.h"
#include "Data/CharacterDataTables.h"

#include "Trap/TrapBase.h"
#include "Trap/Component/TrapMechanics.h"
#include "Trap/GuillotinePendulumTrap.h"
#include "Trap/GunpowderBarrelTrap.h"
#include "Bow/BowMechanics.h"
#include "Map/MapMechanics.h"
#include "Upgrade/UpgradeMechanics.h"
#include "Ping/PingMechanics.h"
#include "Ping/IndicatorComponent.h"
#include "Ping/HitIndicatorComponent.h"
#include "Ping/MarkerComponent/MapMarkerComponent.h"
#include "Monster/BaseMonster.h"

#include "Engine/DamageEvents.h"
#include "DamageType/DamageTypeDown.h"
#include "DamageType/DamageTypeKnockBack.h"
#include "DamageType/DamageTypeStun.h"


/// ------------------------------------------------------------------------------
///									Main & Override
/// ------------------------------------------------------------------------------

// 생성 & 소멸자
// 언리얼 내장 함수 오버라이딩

#pragma region Main & Override

ATrapperPlayer::ATrapperPlayer(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UTrapperPlayerMovementComponent>(ACharacter::CharacterMovementComponentName))
	, bIsAltPressed(false)
	, bCanCameraTurn(false)
	, IsDamaged(false)
	, IsDead(false)
	, bInstallationMode(false)
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	Movement = Cast<UTrapperPlayerMovementComponent>(GetCharacterMovement());
	Movement->SetIsReplicated(true);

	GetMesh()->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));
	GetMesh()->SetRelativeLocation(FVector(0.f, 0.f, -GetCapsuleComponent()->GetScaledCapsuleHalfHeight()));

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);

	SpeedLineEffectComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("SpeedLine"));
	SpeedLineEffectComponent->SetupAttachment(Camera);
	SpeedLineEffectComponent->SetAutoActivate(false);
	SpeedLineEffectComponent->SetRelativeLocation(FVector(200.f, 0.f, 0.f));

	MapMarkerComponent = CreateDefaultSubobject<UMapMarkerComponent>(TEXT("MapMarkerComponent"));
	MapMarkerComponent->ObjectType = EMapObjectType::Player;
	AddOwnedComponent(MapMarkerComponent);

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	SpringArm->bUsePawnControlRotation = true;
	SpringArm->bInheritPitch = true;
	SpringArm->bInheritYaw = true;
	SpringArm->bInheritRoll = false;

	Camera->bUsePawnControlRotation = false;
	GetCharacterMovement()->bUseControllerDesiredRotation = false;
	GetCharacterMovement()->bOrientRotationToMovement = false;
	InitialRotationRate = GetCharacterMovement()->RotationRate;

	Movement = Cast<UTrapperPlayerMovementComponent>(GetCharacterMovement());

	AudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("AudioComponent"));
	AudioComponent->SetupAttachment(RootComponent);

	AwakenAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("AwakenAudioComponent"));
	AwakenAudioComponent->SetupAttachment(RootComponent);

	BowMechanics = CreateDefaultSubobject<UBowMachanics>(TEXT("BowMechanics"));
	AddOwnedComponent(BowMechanics);

	TrapMechanics = CreateDefaultSubobject<UTrapMechanics>(TEXT("TrapMechanics"));
	AddOwnedComponent(TrapMechanics);

	PingMechanics = CreateDefaultSubobject<UPingMechanics>(TEXT("PingMechanics"));
	AddOwnedComponent(PingMechanics);

	MapMechanics = CreateDefaultSubobject<UMapMechanics>(TEXT("MapMechanics"));
	AddOwnedComponent(MapMechanics);

	UpgradeMechanics = CreateDefaultSubobject<UUpgradeMechanics>(TEXT("UpgradeMechanics"));
	AddOwnedComponent(UpgradeMechanics);

	IndicatorComponent = CreateDefaultSubobject<UIndicatorComponent>(TEXT("IndicatorComponent"));
	AddOwnedComponent(IndicatorComponent);

	HitIndicatorComponent = CreateDefaultSubobject<UHitIndicatorComponent>(TEXT("HitIndicatorComponent"));
	AddOwnedComponent(HitIndicatorComponent);

	static ConstructorHelpers::FObjectFinder<UDataTable> CharacterTable(TEXT("/Script/Engine.DataTable'/Game/Blueprints/Data/DT_CharacterData.DT_CharacterData'"));
	if (CharacterTable.Succeeded() && CharacterTable.Object)
	{
		CharacterData = CharacterTable.Object;
	}

	//CurrentHP = MaxHP;
	//Movement->MaxFlySpeed = 5000.f;
}

void ATrapperPlayer::BeginPlay()
{
	Super::BeginPlay();

	SetCharacterMesh();

	InitialFOV = Camera->FieldOfView;
	InitialCameraArmOffset = SpringArm->SocketOffset;

	Movement->InitWalkSpeed = Movement->MaxWalkSpeed;
	Movement->InitFlySpeed = Movement->MaxFlySpeed;


	PlayerController = Cast<ATrapperPlayerController>(GetController());

	CreatePlayerHUD();

	if (IsValid(BowCameraCurve))
	{
		FOnTimelineFloat BowCameraTimelineProgress;
		BowCameraTimelineProgress.BindDynamic(this, &ATrapperPlayer::SetCameraAim);
		BowCameraTimeLine.AddInterpFloat(BowCameraCurve, BowCameraTimelineProgress);
		BowCameraTimeLine.SetTimelineLength(1.f);
	}

	if (IsValid(BowCameraCurveShoot))
	{
		FOnTimelineFloat BowCameraTimelineProgress;
		BowCameraTimelineProgress.BindDynamic(this, &ATrapperPlayer::SetCameraShoot);
		BowCameraShootTimeLine.AddInterpFloat(BowCameraCurveShoot, BowCameraTimelineProgress);
		BowCameraShootTimeLine.SetTimelineLength(1.f);
	}

	RespawnPoint = GetActorLocation();

	if (IsLocallyControlled())
	{
		GetMesh()->SetCustomDepthStencilValue(1);
		//GetMesh()->SetRenderCustomDepth(false);

		IndicatorComponent->DestroyComponent();

		GetWorld()->GetTimerManager().SetTimer(
			CheckPlayerSightTimer, this, &ATrapperPlayer::CheckPlayerSight, 0.1, true);
	}
	else
	{
		MapMechanics->DestroyComponent();
		HitIndicatorComponent->DestroyComponent();

		if (HasAuthority())
		{
			IndicatorComponent->SetType(EPlayerPingType::E_OtherPlayer2P);
		}
		else
		{
			IndicatorComponent->SetType(EPlayerPingType::E_OtherPlayer1P);
		}
	}

	TrapperGameState = GetWorld()->GetGameState<ATrapperGameState>();
	//UE_LOG(LogTemp, Warning, TEXT("Player BeginPlay"));

	ApplyCharacterData(StaticCast<uint8>(ECharacterData::Default));

	if (HasAuthority())
	{
		if (IsLocallyControlled())
		{
			b1Player = true;
		}
	}
	else
	{
		if (!IsLocallyControlled())
		{
			b1Player = true;
		}
	}

	OriginalBrakingDecelerationFalling = Movement->BrakingDecelerationFalling;
	SpeedLineEffectComponent->SetAsset(SpeedLineEffect);
}

void ATrapperPlayer::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	GetWorld()->GetTimerManager().ClearAllTimersForObject(this);
}

void ATrapperPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	BowCameraShootTimeLine.TickTimeline(DeltaTime);
	BowCameraTimeLine.TickTimeline(DeltaTime);

	if (bCanCameraTurn)
	{
		TurnInterp();
	}

	//FindMagneticItem();

	if (!IsValid(PlayerHud))
	{
		CreatePlayerHUD();
	}
}

void ATrapperPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* UIC = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (UIC)
	{
		UIC->BindAction(LookAction, ETriggerEvent::Triggered, this, &ATrapperPlayer::Look);
		UIC->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ATrapperPlayer::Move);
		UIC->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ATrapperPlayer::Jump);
		UIC->BindAction(InteractAction, ETriggerEvent::Triggered, this, &ATrapperPlayer::Interact);

		UIC->BindAction(FClickAction, ETriggerEvent::Started, this, &ATrapperPlayer::FClickStarted);
		UIC->BindAction(FClickAction, ETriggerEvent::Ongoing, this, &ATrapperPlayer::FClickOngoing);
		UIC->BindAction(FClickAction, ETriggerEvent::Canceled, this, &ATrapperPlayer::FClickCanceled);
		UIC->BindAction(FClickAction, ETriggerEvent::Completed, this, &ATrapperPlayer::FClickCompleted);

		UIC->BindAction(CameraFixAction, ETriggerEvent::Started, this, &ATrapperPlayer::CameraFixStart);
		UIC->BindAction(CameraFixAction, ETriggerEvent::Completed, this, &ATrapperPlayer::CameraFixEnd);

		UIC->BindAction(BackAction, ETriggerEvent::Started, this, &ATrapperPlayer::BackOrOnSystemMenu);

		/// Cheat Key
		UIC->BindAction(DebugActiveAction, ETriggerEvent::Triggered, this, &ATrapperPlayer::DebugActive);
		UIC->BindAction(ToMaintenance1Action, ETriggerEvent::Triggered, this, &ATrapperPlayer::GoMaintenance1);
		UIC->BindAction(ToMaintenance2Action, ETriggerEvent::Triggered, this, &ATrapperPlayer::GoMaintenance2);
		UIC->BindAction(ToMaintenance3Action, ETriggerEvent::Triggered, this, &ATrapperPlayer::GoMaintenance3);
		UIC->BindAction(ToMaintenance4Action, ETriggerEvent::Triggered, this, &ATrapperPlayer::GoMaintenance4);
		UIC->BindAction(ToMaintenance5Action, ETriggerEvent::Triggered, this, &ATrapperPlayer::GoMaintenance5);
		UIC->BindAction(ToMaintenance6Action, ETriggerEvent::Triggered, this, &ATrapperPlayer::GoMaintenance6);
		UIC->BindAction(AfterBossAction, ETriggerEvent::Triggered, this, &ATrapperPlayer::AfterBoss);
		UIC->BindAction(RestartAction, ETriggerEvent::Triggered, this, &ATrapperPlayer::GameRestart);
		UIC->BindAction(AddBoneAction, ETriggerEvent::Triggered, this, &ATrapperPlayer::AddBone);
		UIC->BindAction(InvincibilityAction, ETriggerEvent::Triggered, this, &ATrapperPlayer::Invincibility);
		UIC->BindAction(KillMonstersAction, ETriggerEvent::Triggered, this, &ATrapperPlayer::MonsterKill);
		UIC->BindAction(KillSelfAction, ETriggerEvent::Triggered, this, &ATrapperPlayer::SelfKill);
		UIC->BindAction(QuestSkipAction, ETriggerEvent::Triggered, this, &ATrapperPlayer::SkipQuest);
	}
}

void ATrapperPlayer::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ATrapperPlayer, CurrentHP);
	DOREPLIFETIME(ATrapperPlayer, MaxHP);
	DOREPLIFETIME(ATrapperPlayer, SkipGauge);
}

void ATrapperPlayer::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);

	Movement->BrakingDecelerationFalling = OriginalBrakingDecelerationFalling;
	//Movement->GravityScale = Movement->DefaultGravityScale;
}

#pragma endregion Main & Override


/// ------------------------------------------------------------------------------
///								Input Action Binding
/// ------------------------------------------------------------------------------

// 인풋 관련 함수 & 변수들

#pragma region Input Action Binding

void ATrapperPlayer::Look(const FInputActionValue& Value)
{
	if (bCanCameraTurn
		|| PingMechanics->IsPingSelectMode()
		|| TrapMechanics->IsTrapSelectMode()
		|| MapMechanics->IsOpenedWorldMap()
		|| !bLookInput)
	{
		return;
	}

	FVector2D Data = Value.Get<FVector2D>();
	float Scale = 0.f;
	if (BowMechanics->IsDrawingBow())
	{
		float RealAimSensitivityScale = 1.0f + (MouseSensivityScale * 100.0f - 50) * 0.02f;
		Scale = RealAimSensitivityScale;
	}
	else
	{
		float RealMouseSensitivityScale = 1.0f + (MouseSensivityScale * 100.0f - 50) * 0.02f;
		Scale = RealMouseSensitivityScale;
	}

	Data.X *= Scale;
	Data.Y *= Scale;

	AddControllerPitchInput(Data.Y);
	AddControllerYawInput(Data.X);
	CharacterControlTypeCheck();
}

void ATrapperPlayer::Move(const FInputActionValue& Value)
{
	if (!bMoveInput)
	{
		return;
	}

	FVector2D Data = Value.Get<FVector2D>();

	FRotator Rotation;
	if (ControlState == ECharacterControlType::MovingAlt)
	{
		Rotation = GetActorRotation();
	}
	else
	{
		Rotation = GetControlRotation();
	}

	const FRotator ForwordRotation = FRotator(0, Rotation.Yaw, 0);
	const FVector ForwordVector = UKismetMathLibrary::GetForwardVector(ForwordRotation);
	const FVector RightVector = UKismetMathLibrary::GetRightVector(ForwordRotation);

	AddMovementInput(ForwordVector, Data.Y);
	AddMovementInput(RightVector, Data.X);

	CharacterControlTypeCheck();
}

void ATrapperPlayer::Jump()
{
	if (!bMoveInput)
	{
		return;
	}

	WantsToStopMagneticMove();

	Super::Jump();
}


void ATrapperPlayer::AllMonsterKill()
{
	if (HasAuthority())
	{
		GetWorld()->GetAuthGameMode<ATrapperGameMode>()->KillCurrentMonster();
	}
}

void ATrapperPlayer::ServerSelfKill_Implementation()
{
	MulticastSelfKill();
}

void ATrapperPlayer::MulticastSelfKill_Implementation()
{
	UGameplayStatics::ApplyDamage(this, 9999.f, GetController(), this, UDamageType::StaticClass());
}

void ATrapperPlayer::ServerRequiredCheat_Implementation(ETrapperEventCode Code)
{
	TrapperGameState->OnEventExecute.Broadcast(Code);
}

void ATrapperPlayer::BackOrOnSystemMenu()
{
	if (!IsLocallyControlled())
	{
		return;
	}

	PlayerController->BackOrSystemMenu();
}

void ATrapperPlayer::DebugActive()
{
	bIsDebugActive = ~bIsDebugActive;

	if (bIsDebugActive)
	{
		PlayerHud->ShowCheatModeText(true);
	}
	else
	{
		PlayerHud->ShowCheatModeText(false);
	}
}

void ATrapperPlayer::FClickStarted(const FInputActionValue& Value)
{
	if (!IsLocallyControlled()) return;

	if (!bRequiredMediaSkip)
	{
		if (HasAuthority())
		{
			TrapperGameState->OnEventExecute.Broadcast(ETrapperEventCode::E_Skip_Media);
		}
		else
		{
			ServerRequiredCheat(ETrapperEventCode::E_Skip_Media);
		}

		bRequiredMediaSkip = true;
	}
	
	if (SkipGauge >= 1.f)
	{
		if (HasAuthority())
		{
			SkipGauge = 0.f;
			OnRep_SkipGauge();
		}
		else
		{
			ServerRPCSkipGaugeChange(0.f);
		}
	}

	FVector Start = GetActorLocation();
	FVector End = Start + GetActorForwardVector() * InteractDistance;

	FCollisionShape Sphere = FCollisionShape::MakeSphere(InteractRadius);
	FHitResult HitResult;
	bool HasHit = GetWorld()->SweepSingleByChannel(
		HitResult,
		Start, End,
		FQuat::Identity,
		ECC_GameTraceChannel9,
		Sphere
	);

	if (HasHit)
	{
		//UE_LOG(LogQuest, Warning, TEXT("Hit Actor : %s"), *HitResult.GetActor()->GetName());

		AInteract* InteractActor = Cast<AInteract>(HitResult.GetActor());
		if (!InteractActor)
		{
			return;
		}

		if (HasAuthority() && IsLocallyControlled())
		{
			InteractActor->CheckActivate(30, true);
			MulticastRPCPlayInstallAnim();
		}

		if (!HasAuthority() && IsLocallyControlled())
		{
			InteractActor->CheckActivate(30, false);
			ServerRPCPlayInstallAnim();
		}

		if (TrapperGameState->CurrentGameProgress == EGameProgress::Tutorial)
		{
			UpgradeMechanics->EnterUpgradeModeTutorial();
		}
		else if (TrapperGameState->CurrentGameProgress == EGameProgress::BonusWave)
		{
			UpgradeMechanics->EnterUpgradeMode();
		}
	}

	//S2SSetAwaken(HasAuthority());
}

void ATrapperPlayer::FClickOngoing(const FInputActionValue& Value)
{
	SkipAccumulatelTime += GetWorld()->GetDeltaSeconds();
	float Gauge = SkipAccumulatelTime / SkipTime;
	Gauge = FMath::Clamp(Gauge, Gauge, 1);

	if (HasAuthority())
	{
		SkipGauge = Gauge;
		OnRep_SkipGauge();
	}
	else
	{
		ServerRPCSkipGaugeChange(Gauge);
	}
}

void ATrapperPlayer::FClickCanceled(const FInputActionValue& Value)
{
	SkipAccumulatelTime = 0.f;

	if (HasAuthority())
	{
		SkipGauge = 0.f;
		OnRep_SkipGauge();
	}
	else
	{
		ServerRPCSkipGaugeChange(0.f);
	}
}

void ATrapperPlayer::FClickCompleted(const FInputActionValue& Value)
{
	SkipAccumulatelTime = 0.f;

	if (HasAuthority())
	{
		SkipGauge = 1.f;

		if (SkipCurrentStage())
		{
			SkipGauge = 0.f;
		}

		OnRep_SkipGauge();
	}
	else
	{
		ServerRPCSkip();
	}
}

void ATrapperPlayer::CameraFixStart(const FInputActionValue& Value)
{
	//UE_LOG(LogTemp, Warning, TEXT("Camera Fix Start"));
	bIsAltPressed = true;

	BlockBow();
	CharacterControlTypeCheck();
	UnblockBow();
}

void ATrapperPlayer::CameraFixEnd(const FInputActionValue& Value)
{
	//UE_LOG(LogTemp, Warning, TEXT("Camera Fix End"));
	bIsAltPressed = false;
	bCanCameraTurn = true;

	CharacterControlTypeCheck();
}

/// Cheat Mode --------------------------------------------------------

void ATrapperPlayer::GoMaintenance1(const FInputActionValue& Value)
{
	if (!bIsDebugActive) return;

	if (HasAuthority())
	{
		TrapperGameState->OnEventExecute.Broadcast(ETrapperEventCode::C_Maintenance_1);
	}
	else
	{
		ServerRequiredCheat(ETrapperEventCode::C_Maintenance_1);
	}
}

void ATrapperPlayer::GoMaintenance2(const FInputActionValue& Value)
{
	if (!bIsDebugActive) return;

	if (HasAuthority())
	{
		TrapperGameState->OnEventExecute.Broadcast(ETrapperEventCode::C_Maintenance_2);
	}
	else
	{
		ServerRequiredCheat(ETrapperEventCode::C_Maintenance_2);
	}
}

void ATrapperPlayer::GoMaintenance3(const FInputActionValue& Value)
{
	if (!bIsDebugActive) return;

	if (HasAuthority())
	{
		TrapperGameState->OnEventExecute.Broadcast(ETrapperEventCode::C_Maintenance_3);
	}
	else
	{
		ServerRequiredCheat(ETrapperEventCode::C_Maintenance_3);
	}

}

void ATrapperPlayer::GoMaintenance4(const FInputActionValue& Value)
{
	if (!bIsDebugActive) return;

	if (HasAuthority())
	{
		TrapperGameState->OnEventExecute.Broadcast(ETrapperEventCode::C_Maintenance_4);
	}
	else
	{
		ServerRequiredCheat(ETrapperEventCode::C_Maintenance_4);
	}
}

void ATrapperPlayer::GoMaintenance5(const FInputActionValue& Value)
{
	if (!bIsDebugActive) return;

	if (HasAuthority())
	{
		TrapperGameState->OnEventExecute.Broadcast(ETrapperEventCode::C_Maintenance_5);
	}
	else
	{
		ServerRequiredCheat(ETrapperEventCode::C_Maintenance_5);
	}

}

void ATrapperPlayer::GoMaintenance6(const FInputActionValue& Value)
{
	if (!bIsDebugActive) return;

	if (HasAuthority())
	{
		TrapperGameState->OnEventExecute.Broadcast(ETrapperEventCode::C_Maintenance_6);
	}
	else
	{
		ServerRequiredCheat(ETrapperEventCode::C_Maintenance_6);
	}
}

void ATrapperPlayer::AfterBoss(const FInputActionValue& Value)
{
	if (!bIsDebugActive) return;

	if (HasAuthority())
	{
		TrapperGameState->OnEventExecute.Broadcast(ETrapperEventCode::C_After_Boss);
	}
	else
	{
		ServerRequiredCheat(ETrapperEventCode::C_After_Boss);
	}

}

void ATrapperPlayer::GameRestart(const FInputActionValue& Value)
{
	if (!bIsDebugActive) return;

	if (HasAuthority())
	{
		TrapperGameState->OnEventExecute.Broadcast(ETrapperEventCode::C_Game_Restart);
	}
	else
	{
		ServerRequiredCheat(ETrapperEventCode::C_Game_Restart);
	}
}

void ATrapperPlayer::AddBone(const FInputActionValue& Value)
{
	if (!bIsDebugActive) return;

	if (HasAuthority())
	{
		TrapperGameState->OnEventExecute.Broadcast(ETrapperEventCode::C_Add_Bone);
	}
	else
	{
		ServerRequiredCheat(ETrapperEventCode::C_Add_Bone);
	}

}

void ATrapperPlayer::Invincibility(const FInputActionValue& Value)
{
	if (!bIsDebugActive) return;

	if (HasAuthority())
	{
		TrapperGameState->OnEventExecute.Broadcast(ETrapperEventCode::C_Invincibility_Mode);
	}
	else
	{
		ServerRequiredCheat(ETrapperEventCode::C_Invincibility_Mode);
	}
}

void ATrapperPlayer::MonsterKill(const FInputActionValue& Value)
{
	if (!bIsDebugActive) return;

	if (HasAuthority())
	{
		TrapperGameState->OnEventExecute.Broadcast(ETrapperEventCode::C_All_Monster_Kill);
	}
	else
	{
		ServerRequiredCheat(ETrapperEventCode::C_All_Monster_Kill);
	}
}

void ATrapperPlayer::SelfKill(const FInputActionValue& Value)
{
	if (!bIsDebugActive) return;

	if (HasAuthority())
	{
		MulticastSelfKill();
	}
	else
	{
		ServerSelfKill();
	}
}

void ATrapperPlayer::SkipQuest(const FInputActionValue& Value)
{
	if (!bIsDebugActive) return;

	if (HasAuthority())
	{
		TrapperGameState->OnEventExecute.Broadcast(ETrapperEventCode::E_Skip_Quest);
	}
	else
	{
		ServerRequiredCheat(ETrapperEventCode::E_Skip_Quest);
	}
}

#pragma endregion Input Action Binding


/// ------------------------------------------------------------------------------
///										Move
/// ------------------------------------------------------------------------------

// 캐릭터 & 카메라 움직임 관련 함수

#pragma region Move

void ATrapperPlayer::CharacterControlTypeCheck()
{
	ECharacterControlType TempType = ControlState;

	if (GetVelocity().IsNearlyZero())
	{
		bIsAltPressed ? ControlState = ECharacterControlType::IdleAlt : ControlState = ECharacterControlType::Idle;
	}
	else
	{
		bIsAltPressed ? ControlState = ECharacterControlType::MovingAlt : ControlState = ECharacterControlType::Moving;
	}

	if (BowMechanics->IsDrawingBow())
	{
		ControlState = ECharacterControlType::Drawing;
	}

	if (Movement->GetMagneticMovingState())
	{
		ControlState = ECharacterControlType::MagneticMoving;
	}

	// 만약 상태가 달라진다면 함수 호출
	if (TempType == ControlState)
	{
		return;
	}

	if (HasAuthority())
	{
		MulticastChangeControlOptions(ControlState);
	}
	else if (IsLocallyControlled())
	{
		SetCharacterControlData(ControlState);
		ServerRPCChangeControlOptions(ControlState);
	}
}

void ATrapperPlayer::SetCharacterControlData(ECharacterControlType type)
{
	FRotator CameraWorld = Camera->GetComponentRotation();

	switch (type)
	{
	case ECharacterControlType::Moving:
		if (!bCanCameraTurn) bUseControllerRotationYaw = true;
		GetCharacterMovement()->bOrientRotationToMovement = false;
		GetCharacterMovement()->bUseControllerDesiredRotation = true;
		GetCharacterMovement()->RotationRate = InitialRotationRate;
		//UE_LOG(LogTemp, Warning, TEXT("Moving"));
		break;

	case ECharacterControlType::MovingAlt:
		bUseControllerRotationYaw = false;
		GetCharacterMovement()->bOrientRotationToMovement = false;
		GetCharacterMovement()->bUseControllerDesiredRotation = false;
		GetCharacterMovement()->RotationRate = InitialRotationRate;
		//UE_LOG(LogTemp, Warning, TEXT("Moving Alt"));
		break;

	case ECharacterControlType::Idle:
		bUseControllerRotationYaw = false;
		GetCharacterMovement()->bOrientRotationToMovement = false;
		GetCharacterMovement()->bUseControllerDesiredRotation = false;
		GetCharacterMovement()->RotationRate = InitialRotationRate;
		//UE_LOG(LogTemp, Warning, TEXT("Idle"));
		break;

	case ECharacterControlType::IdleAlt:
		bUseControllerRotationYaw = false;
		GetCharacterMovement()->bOrientRotationToMovement = false;
		GetCharacterMovement()->bUseControllerDesiredRotation = false;
		GetCharacterMovement()->RotationRate = InitialRotationRate;
		//UE_LOG(LogTemp, Warning, TEXT("Idle Alt"));
		break;

	case ECharacterControlType::Drawing:

		bUseControllerRotationYaw = true;
		GetCharacterMovement()->bOrientRotationToMovement = false;
		GetCharacterMovement()->bUseControllerDesiredRotation = true;
		GetCharacterMovement()->RotationRate = InitialRotationRate;
		//GetCharacterMovement()->RotationRate = FRotator(InitialRotationRate.Pitch, InitialRotationRate.Yaw, 1000);
		//UE_LOG(LogTemp, Warning, TEXT("control type : Drawing"));

		break;

	case ECharacterControlType::MagneticMoving:
		bUseControllerRotationYaw = false;
		GetCharacterMovement()->bOrientRotationToMovement = false;
		GetCharacterMovement()->bUseControllerDesiredRotation = false;
		GetCharacterMovement()->RotationRate = InitialRotationRate;
		//UE_LOG(LogTemp, Warning, TEXT("control type : MagneticMoving"));

	default:
		break;
	}
}

void ATrapperPlayer::TurnInterp()
{
	FRotator SoketRelativeRotation = SpringArm->GetSocketTransform(USpringArmComponent::SocketName, RTS_Component).Rotator();
	SoketRelativeRotation.Normalize();

	float SmoothYawInput = SoketRelativeRotation.Yaw * TurnSmoothFactor;

	const float Tolerance = 0.1f;
	if (FMath::IsNearlyEqual(SoketRelativeRotation.Yaw, 0.f, Tolerance))
	{
		bCanCameraTurn = false;
		CharacterControlTypeCheck();
	}
	else
	{
		AddControllerYawInput(-SmoothYawInput);
	}
}

float ATrapperPlayer::GetLookRotation()
{
	FVector ForwardVector = GetActorForwardVector().GetSafeNormal();
	FVector LookVector = GetControlRotation().Vector().GetSafeNormal();

	float DotResult = FVector::DotProduct(ForwardVector, LookVector);
	float Angle = UKismetMathLibrary::DegAcos(DotResult);

	FVector CrossResult = FVector::CrossProduct(ForwardVector, LookVector);
	float Sign = UKismetMathLibrary::SignOfFloat(CrossResult.Z);

	float SmoothedValue = FMath::Clamp(FMath::Lerp(PreviousAngle, Angle * Sign, LookSmoothFactor), -90, 90);
	PreviousAngle = SmoothedValue;

	// 컨트롤러의 회전 방향을 향해 쳐다보는게 자연스러워서 반전
	return -SmoothedValue;
}

FRotator ATrapperPlayer::GetAimOffset() const
{
	const FVector AimDirWS = GetBaseAimRotation().Vector();
	const FVector AimDirLS = ActorToWorld().InverseTransformVectorNoScale(AimDirWS);
	const FRotator AimRotLS = AimDirLS.Rotation();

	return AimRotLS;
}

void ATrapperPlayer::AdjustMoveSpeed()
{
	Movement->MaxWalkSpeed = Movement->InitWalkSpeed * TrapperGameState->MoveSpeed;
}

void ATrapperPlayer::AdjustMagneticMoveSpeed()
{
	Movement->MaxFlySpeed = Movement->InitFlySpeed * TrapperGameState->MagneticMoveSpeed;
}

void ATrapperPlayer::AdjustMagneticMoveAfterSpeed()
{
	Movement->bUseMagneticAfterSpeed = true;
	Movement->MagneticMoveAfterSpeed = TrapperGameState->MagneticMoveAfterSpeed;
}

void ATrapperPlayer::MulticastSetInput_Implementation(bool Value)
{
	if (!IsLocallyControlled())
	{
		return;
	}

	SetMoveInput(Value);
	SetLookInput(Value);

	if (Value)
	{
		UnblockBow();
		TrapMechanics->UnBlockSelectTrap();
	}
	else
	{
		BlockBow();
		TrapMechanics->BlockSelectTrap();
	}

}

void ATrapperPlayer::ServerRPCChangeControlOptions_Implementation(ECharacterControlType type)
{
	SetCharacterControlData(type);
}

void ATrapperPlayer::MulticastChangeControlOptions_Implementation(ECharacterControlType type)
{
	SetCharacterControlData(type);
}

#pragma endregion Move


/// ------------------------------------------------------------------------------
///									Character State
/// ------------------------------------------------------------------------------

// 캐릭터 피격, 사망, 리스폰 관련 함수들

#pragma region Character State

void ATrapperPlayer::SetCharacterMesh()
{
	if (HasAuthority())
	{
		if (IsLocallyControlled())
		{
			GetMesh()->SetSkeletalMeshAsset(FirstCharacterMesh);
		}
		else
		{
			GetMesh()->SetSkeletalMeshAsset(SecondCharacterMesh);
		}
	}
	else
	{
		if (IsLocallyControlled())
		{
			GetMesh()->SetSkeletalMeshAsset(SecondCharacterMesh);
		}
		else
		{
			GetMesh()->SetSkeletalMeshAsset(FirstCharacterMesh);
		}
	}
}

void ATrapperPlayer::ApplyCharacterData(uint8 Type)
{
	auto Data = CharacterData->GetRowNames();
	FCharacterInfo* GetData = CharacterData->FindRow<FCharacterInfo>(Data[Type], FString());
	if (!GetData)
	{
		UE_LOG(LogTemp, Warning, TEXT("Character DataTable Not Exist"));
		return;
	}

	MaxHP = GetData->MaxHealth;
	CurrentHP = MaxHP;

	HealthCoolTime = GetData->HealthCoolTime;
	RecoveryDelayTime = GetData->RecoveryDelayTime;
	HealthRecoveryAmount = GetData->HealthRecoveryAmount;
	InvincibilityTime = GetData->InvincibilityTime;

	Movement->SlowReductionRate = GetData->SlowReductionRate;
	Movement->MaxWalkSpeed = GetData->MoveSpeed;
	Movement->InitWalkSpeed = Movement->MaxWalkSpeed;
	Movement->MaxFlySpeed = GetData->MagneticMoveSpeed;
	Movement->InitFlySpeed = Movement->MaxFlySpeed;

	BowMechanics->SetDrawTime(GetData->ChargeTime);
	BowMechanics->ArrowMinSpeed = GetData->ArrowMinSpeed;
	BowMechanics->ArrowMaxSpeed = GetData->ArrowMaxSpeed;
	BowMechanics->NormalArrowMinDamage = GetData->NormalArrowMinDamage;
	BowMechanics->NormalArrowMaxDamage = GetData->NormalArrowMaxDamage;
	BowMechanics->MagneticArrowMinDamage = GetData->MagneticArrowMinDamage;
	BowMechanics->MagneticArrowMaxDamage = GetData->MagneticArrowMaxDamage;

	if (HasAuthority())
	{
		UE_LOG(LogTemp, Warning, TEXT("[Server] Start Health : %f / %f"), CurrentHP, GetData->MaxHealth);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[Client] Start Health : %f / %f"), CurrentHP, GetData->MaxHealth);
	}

	if (HasAuthority() && IsLocallyControlled())
	{
		GetWorld()->GetAuthGameMode<ATrapperGameMode>()->InitialItemCount = GetData->StartMoney;
	}

	ATrapperPlayerController* PC = Cast<ATrapperPlayerController>(GetController());
	if (PC)
	{
		PC->RestartDelay = GetData->RevivalTime;
	}
}

float ATrapperPlayer::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float Damage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	if (HasAuthority())
	{
		if (TrapperGameState->bIsInvincibilityMode || bIsInvincibility || IsDead)
		{
			return 0.f;
		}

		Damage = ServerTakeDamage(Damage, DamageEvent, DamageCauser);

		if (IsLocallyControlled() && IsValid(PlayerHud))
		{
			PlayerHud->OnHit();
		}

		if (!IsValid(DamageCauser))
		{
			return 0.f;
		}

		if (DamageCauser->GetOwner()->ActorHasTag("Monster"))
		{
			ABaseMonster* Monster = Cast<ABaseMonster>(DamageCauser->GetOwner());
			if (IsValid(Monster))
			{
				ClientRPCTakeDamage(Monster, true);
				if (IsLocallyControlled())
				{
					HitIndicatorComponent->OnPlayerHit(Monster);
				}
			}
		}
		else if (DamageCauser->ActorHasTag("Monster"))
		{
			ABaseMonster* Monster = Cast<ABaseMonster>(DamageCauser);
			if (IsValid(Monster))
			{
				ClientRPCTakeDamage(Monster, true);
				if (IsLocallyControlled())
				{
					HitIndicatorComponent->OnPlayerHit(Monster);
				}
			}
		}
		else
		{
			ClientRPCTakeDamage(DamageCauser, false);
		}
	}
	// 	else if (IsLocallyControlled())
	// 	{
	// 		ServerRPCCharacterDamaged(Damage, DamageEvent, DamageCauser);
	// 	}

	return Damage;
}

float ATrapperPlayer::ServerTakeDamage(float Damage, FDamageEvent const& DamageEvent, AActor* DamageCauser)
{
	Damage *= TrapperGameState->IncomingDamage;
	Damage = FMath::Min(CurrentHP, FMath::RoundToInt(Damage));

	CurrentHP -= Damage;
	OnRep_CurrentHP();

	if (CurrentHP <= 0)
	{
		MulticastRPCCharacterDeath();
		return Damage;
	}

	MulticastRPCCharacterAlive(Damage, DamageEvent, DamageCauser);
	return Damage;
}

bool ATrapperPlayer::PlayDamageTypeAnimation(float Damage, FDamageEvent const& DamageEvent)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (!IsValid(AnimInstance) && !IsValid(DamagedAnimationMontage))
	{
		return false;
	}

	AnimInstance->Montage_Play(DamagedAnimationMontage, 1.0);

	// 데미지 클래스 안보냈을 때 예외처리
	if (!IsValid(DamageEvent.DamageTypeClass))
	{
		if (Damage < 70)
		{
			AnimInstance->Montage_JumpToSection(FName("Light"), DamagedAnimationMontage);
		}
		else
		{
			AnimInstance->Montage_JumpToSection(FName("Heavy"), DamagedAnimationMontage);
		}
	}
	else
	{
		// 데미지 타입별 애니메이션 재생
		const UDamageType* DamageType = DamageEvent.DamageTypeClass->GetDefaultObject<UDamageType>();

		if (DamageType->IsA<UDamageTypeDown>())
		{
			AnimInstance->Montage_JumpToSection(FName("Down"), DamagedAnimationMontage);
		}
		else if (DamageType->IsA<UDamageTypeStun>())
		{
			AnimInstance->Montage_JumpToSection(FName("StunStart"), DamagedAnimationMontage);
		}
		else if (DamageType->IsA<UDamageTypeKnockBack>())
		{
			AnimInstance->Montage_JumpToSection(FName("KnockBack"), DamagedAnimationMontage);
		}
		else
		{
			if (Damage < 70)
			{
				AnimInstance->Montage_JumpToSection(FName("Light"), DamagedAnimationMontage);
			}
			else
			{
				AnimInstance->Montage_JumpToSection(FName("Heavy"), DamagedAnimationMontage);
			}
		}
	}

	// 몽타주 끝났을 때 델리게이트 바인딩
	FOnMontageEnded EndDelegate;
	EndDelegate.BindUObject(this, &ATrapperPlayer::DamagedEnd);
	AnimInstance->Montage_SetEndDelegate(EndDelegate, DamagedAnimationMontage);

	return true;
}

void ATrapperPlayer::ClientRPCTakeDamage_Implementation(AActor* DamageCauser, bool bIsMonster)
{
	if (!IsLocallyControlled())
	{
		return;
	}

	if (IsValid(PlayerHud))
	{
		PlayerHud->OnHit();
	}

	if (bIsMonster)
	{
		ABaseMonster* Monster = Cast<ABaseMonster>(DamageCauser);
		if (IsValid(Monster))
		{
			HitIndicatorComponent->OnPlayerHit(Monster);
		}
	}
}


void ATrapperPlayer::ServerRPCCharacterDamaged_Implementation(float Damage, FDamageEvent const& DamageEvent, AActor* DamageCauser)
{
	ServerTakeDamage(Damage, DamageEvent, DamageCauser);
}

void ATrapperPlayer::MulticastRPCCharacterAlive_Implementation(float Damage, FDamageEvent const& DamageEvent, AActor* DamageCauser)
{
	if (IsValid(DamageCauser) && DamageCauser->ActorHasTag("NoCharacterHitAnimationTrap"))
	{
		return;
	}

	IsDamaged = true;
	GetCharacterMovement()->SetMovementMode(MOVE_None);

	bool IsPlayAnimation = PlayDamageTypeAnimation(Damage, DamageEvent);
	if (!IsPlayAnimation)
	{
		return;
	}

	// 몽타주 재생할때 적용해야 할 것
	BlockBow();

	// 무적시간 적용
	SetInvincibility();

	// 체력회복 타이머 설정
	SetRecoveryHealth();
}

void ATrapperPlayer::DamagedEnd(UAnimMontage* Montage, bool bInterrupted)
{
	UnblockBow();
	IsDamaged = false;
	GetCharacterMovement()->SetMovementMode(MOVE_Walking);
}

void ATrapperPlayer::MulticastRPCCharacterDeath_Implementation()
{
	IsDead = true;
	GetCharacterMovement()->SetMovementMode(MOVE_None);
	BlockBow();
	TrapMechanics->BlockSelectTrap();
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && DeathAnimationMontage)
		AnimInstance->Montage_Play(DeathAnimationMontage, 1.0);

	if (PlayerController && IsLocallyControlled())
	{
		PlayerHud->SetVisibility(ESlateVisibility::Collapsed);
		PlayerController->PlayerDeath(this);
	}
}

void ATrapperPlayer::ServerRPCCharacterRespawn_Implementation()
{
	MulticastCharacterRespawn();
}

void ATrapperPlayer::MulticastCharacterRespawn_Implementation()
{
	if (IsLocallyControlled())
	{
		PlayerHud->SetVisibility(ESlateVisibility::Visible);
	}

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && DeathAnimationMontage)
		AnimInstance->Montage_Stop(0.f, DeathAnimationMontage);

	GetCharacterMovement()->SetMovementMode(MOVE_Walking);
	UnblockBow();
	TrapMechanics->UnBlockSelectTrap();

	TeleportTo(RespawnPoint, GetActorRotation());
	IsDamaged = false;
	IsDead = false;
	CurrentHP = MaxHP;
	OnRep_CurrentHP();
}


void ATrapperPlayer::OnRep_CurrentHP()
{
	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		ATrapperPlayerController* PC = Cast<ATrapperPlayerController>(Iterator->Get());
		if (PC)
		{
			PC->SetPlayerHPBar();
		}
	}
}

void ATrapperPlayer::AdjustPlayerMaxHealth()
{
	MaxHP = MaxHP * TrapperGameState->PlayerMaxHealth;
	CurrentHP = CurrentHP * TrapperGameState->PlayerMaxHealth;
	OnRep_CurrentHP();
}

void ATrapperPlayer::SetInvincibility()
{
	bIsInvincibility = true;
	GetWorld()->GetTimerManager().SetTimer(InvincibilityHandle, this,
		&ATrapperPlayer::EndInvincibilityTime, 1.f, false, InvincibilityTime);
}

void ATrapperPlayer::EndInvincibilityTime()
{
	bIsInvincibility = false;
}

void ATrapperPlayer::CheckHealthCoolTime()
{
	GetWorld()->GetTimerManager().SetTimer(RecoveryHandle, this,
		&ATrapperPlayer::RecoveryHealth, 1.f, true, RecoveryDelayTime);
}

void ATrapperPlayer::SetRecoveryHealth()
{
	// 설정되어있는 경우 해제하고 다시 설정
	if (!IsLocallyControlled()) return;

	if (GetWorldTimerManager().IsTimerActive(HealthCoolTimeHandle))
	{
		//UE_LOG(LogTemp, Warning, TEXT("HealthCoolTimer 이미 있음, 재설정"));
		GetWorldTimerManager().ClearTimer(HealthCoolTimeHandle);
	}

	if (GetWorldTimerManager().IsTimerActive(RecoveryHandle))
	{
		//UE_LOG(LogTemp, Warning, TEXT("RecoveryTiemr 이미 있음, 해제"));
		GetWorldTimerManager().ClearTimer(RecoveryHandle);
	}

	// 피격 후 30초동안 재피격 없을 시 설정
	GetWorld()->GetTimerManager().SetTimer(HealthCoolTimeHandle, this,
		&ATrapperPlayer::CheckHealthCoolTime, 1.f, false, HealthCoolTime);
}

void ATrapperPlayer::RecoveryHealth()
{
	// 체력 가득 차면 해제
	if (CurrentHP >= MaxHP)
	{
		GetWorldTimerManager().ClearTimer(RecoveryHandle);
		return;
	}

	if (HasAuthority())
	{
		//UE_LOG(LogTemp, Warning, TEXT("[Server] 체력 회복됨. 이전 체력 : %f / 현재 체력 : %f"), CurrentHP, CurrentHP + HealthRecoveryAmount);
		CurrentHP += HealthRecoveryAmount;
		OnRep_CurrentHP();
	}
	else
	{
		ServerRPCRecoveryHealth();
	}
}

void ATrapperPlayer::OnClientLogout()
{
	if(IsValid(MapMechanics))
	{
		MapMechanics->bIsClientLogout = true;
	}
}

void ATrapperPlayer::ServerRPCRecoveryHealth_Implementation()
{
	//UE_LOG(LogTemp, Warning, TEXT("[Client] 체력 회복됨. 이전 체력 : %f / 현재 체력 : %f"), CurrentHP, CurrentHP + HealthRecoveryAmount);
	CurrentHP += HealthRecoveryAmount;
	OnRep_CurrentHP();
}


#pragma endregion Character State


/// ------------------------------------------------------------------------------
///								Magnetic Interactive
/// ------------------------------------------------------------------------------

// 자성 이동 관련

#pragma region Magnetic Interactive

void ATrapperPlayer::Interact(const FInputActionValue& Value)
{
	float Data = Value.Get<float>();

	if (!bMoveInput)
	{
		return;
	}

	if (Movement->CanMagneticMoving() && !Movement->GetMagneticMovingState())
	{
		Movement->bCanMagneticMoving = false;

		GetWorld()->GetTimerManager().SetTimer(MagneticMoveHandle, this,
			&ATrapperPlayer::WantsToStopMagneticMove, 1.f, false, 2.f);

		if (Movement->IsFalling())
		{
			// Start Magnetic Move
			if (HasAuthority() && IsLocallyControlled())
			{
				MulticastWantsToMagneticMove();
			}
			if (!HasAuthority() && IsLocallyControlled())
			{
				WantsToMagneticMove();
				ServerWantsToMagneticMove();
			}
		}
		else
		{
			// Cast Animation
			if (HasAuthority() && IsLocallyControlled())
			{
				MulticastPlayCastAnimation();
			}
			if (!HasAuthority() && IsLocallyControlled())
			{
				PlayCastAnimation();
				ServerPlayCastAnimation();
			}
		}
	}

	return;
}

void ATrapperPlayer::MulticastEnterUpgrade_Implementation()
{
	if (IsLocallyControlled())
	{
		UpgradeMechanics->EnterUpgradeMode();
	}
}

void ATrapperPlayer::FindMagneticPillar()
{
	if (Movement->GetMagneticMovingState() || !IsLocallyControlled())
	{
		return;
	}

	FVector Location;
	FRotator Rotation;
	GetController()->GetPlayerViewPoint(Location, Rotation);

	FVector TraceDirection = Rotation.Vector();
	FVector Start = Location + TraceDirection * 600.f;
	FVector End = Location + TraceDirection * Movement->MagneticDistance;

	float SphereRadius = 100.f;

	FHitResult Result;
	bool HasHit = GetWorld()->SweepSingleByChannel(
		Result, Start, End, FQuat::Identity,
		ECC_GameTraceChannel1, FCollisionShape::MakeSphere(SphereRadius)
	);

	if (HasHit)
	{
		AActor* Target = Result.GetActor();
		AMagneticPillar* NewTarget = Cast<AMagneticPillar>(Target);

		// 갖고있는 타겟이 없을때 한번만 호출
		if (!Movement->GetTarget())
		{
			if (NewTarget)
			{
				//UE_LOG(LogTemp, Warning, TEXT("Set Target"));
				Movement->SetTarget(NewTarget);

				//DrawDebugLine(GetWorld(), Start, End, FColor::Green, false, 1.0f, 0, 1.0f);  // 시작부터 끝까지의 경로
				//DrawDebugSphere(GetWorld(), End, SphereRadius, 12, FColor::Green, false, 1.0f);  // 끝 구
			}
		}
		// 타겟을 갖고있는데 검사한 액터가 투명벽이면 해제
		else
		{
			if (!NewTarget)
			{
				Movement->ClearTarget();
			}
		}
	}
	else
	{
		Movement->ClearTarget();
	}
}

void ATrapperPlayer::ServerPlayCastAnimation_Implementation()
{
	PlayCastAnimation();
}

void ATrapperPlayer::MulticastPlayCastAnimation_Implementation()
{
	PlayCastAnimation();
}

void ATrapperPlayer::ServerWantsToMagneticMove_Implementation()
{
	WantsToMagneticMove();
}

void ATrapperPlayer::MulticastWantsToMagneticMove_Implementation()
{
	WantsToMagneticMove();
}

void ATrapperPlayer::WantsToMagneticMove()
{
	Movement->bWantsToMagneticMove = true;
	Movement->bWantsToMagneticCast = true;
}

void ATrapperPlayer::PlayCastAnimation()
{
	Movement->bWantsToMagneticCast = true;

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (!IsValid(AnimInstance) && !IsValid(MagneticMoveMontage))
	{
		return;
	}

	BlockBow();
	AnimInstance->Montage_Play(MagneticMoveMontage, 1.0);
	FOnMontageEnded EndDelegate;
	EndDelegate.BindUObject(this, &ATrapperPlayer::CastEnd);
	AnimInstance->Montage_SetEndDelegate(EndDelegate, MagneticMoveMontage);
}

void ATrapperPlayer::CastEnd(UAnimMontage* Montage, bool bInterrupted)
{
	UnblockBow();
	Movement->bWantsToMagneticMove = true;
}

void ATrapperPlayer::StartManeticMovingSetting()
{
	BlockBow();
	CharacterControlTypeCheck();
}

void ATrapperPlayer::FinishMagneticMovingSetting()
{
	UnblockBow();
	CharacterControlTypeCheck();
}

bool ATrapperPlayer::IsServerProxy()
{
	if (!HasAuthority() && !IsLocallyControlled()) return true;
	else return false;
}

void ATrapperPlayer::SpawnSpeedLineEffect(bool Value)
{
	if (!SpeedLineEffect)
	{
		UE_LOG(LogTemp, Warning, TEXT("No SpeedLine Effect"));
		return;
	}

	if (Value)
	{
		SpeedLineEffectComponent->Activate();
	}
	else
	{
		SpeedLineEffectComponent->Deactivate();
	}
}

void ATrapperPlayer::WantsToStopMagneticMove()
{
	if (Movement->GetMagneticMovingState())
	{
		Movement->StopMagneticMove();
		return;
	}
}

#pragma endregion Magnetic Interactive

/// ------------------------------------------------------------------------------
///									Interact
/// ------------------------------------------------------------------------------

// 상호작용 관련

#pragma region Interact

void ATrapperPlayer::OnRep_SkipGauge()
{
	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		ATrapperPlayerController* PC = Cast<ATrapperPlayerController>(Iterator->Get());
		if (PC)
		{
			PC->SetSkipGauge();
		}
	}
}

bool ATrapperPlayer::SkipCurrentStage()
{
	//UE_LOG(LogQuest, Warning, TEXT("Skip"));
	TArray<AActor*> OutActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATrapperPlayer::StaticClass(), OutActors);

	ATrapperPlayer* FirstPlayer = nullptr;
	ATrapperPlayer* SecondPlayer = nullptr;

	for (const auto& Pawn : OutActors)
	{
		ATrapperPlayer* TrapperPlayer = Cast<ATrapperPlayer>(Pawn);
		if (!TrapperPlayer) continue;

		if (TrapperPlayer->IsLocallyControlled())
		{
			FirstPlayer = TrapperPlayer;
		}
		else
		{
			SecondPlayer = TrapperPlayer;
		}
	}

	if (!IsValid(FirstPlayer) || !IsValid(SecondPlayer))
	{
		return false;
	}

	if ((FirstPlayer->SkipGauge < 1.f) || (SecondPlayer->SkipGauge < 1.f))
	{
		return false;
	}

	FirstPlayer->SkipGauge = 0.f;
	SecondPlayer->SkipGauge = 0.f;
	OnRep_SkipGauge();

	AGameModeBase* GameMode = UGameplayStatics::GetGameMode(GetWorld());
	ATrapperGameMode* MyGameMode = Cast<ATrapperGameMode>(GameMode);
	if (!MyGameMode) return false;

	EGameProgress CurrentStage = MyGameMode->GetCurrentGameProgress();
	switch (CurrentStage)
	{
	case EGameProgress::Tutorial:
		MyGameMode->SkipTutorial();
		break;
	case EGameProgress::Maintenance:
		MyGameMode->bSkipMaintenance = true;
		break;
	default:
		break;
	}

	return true;
}

void ATrapperPlayer::CheckPlayerSight()
{
	FHitResult Hit;
	FVector Start = Camera->GetComponentLocation() + Camera->GetForwardVector() * FVector::Distance(GetActorLocation(), Camera->GetComponentLocation());
	//FVector End = Camera->GetComponentLocation() + Camera->GetForwardVector() * 3000.f;
	FVector End = Camera->GetComponentLocation() + Camera->GetForwardVector() * 8000.f;
	ECollisionChannel TraceChannel = ECollisionChannel::ECC_Visibility;
	bool bIsCatched = GetWorld()->LineTraceSingleByChannel(Hit, Start, End, TraceChannel);

	if (bIsCatched && Hit.GetActor()->ActorHasTag("Monster"))
	{
		ABaseMonster* Monster = Cast<ABaseMonster>(Hit.GetActor());
		if (IsValid(Monster))
		{
			Monster->OnPlayerSight();
		}
	}

	AActor* HitActor = Hit.GetActor();
	if (bIsCatched && IsValid(HitActor))
	{
		if (HitActor->ActorHasTag("Trap"))
		{
			PlayerHud->SetTrapAimFrameVisibility(true);
			bHelp = true;

			if (HitActor->ActorHasTag("CollaborationTrap"))
			{
				HitRayTrapBase = Cast<ATrapBase>(HitActor);

				if (IsValid(HitRayTrapBase.Get()))
				{
					if (IsValid(PreviousHitRayTrapBase.Get()) && PreviousHitRayTrapBase != HitRayTrapBase)
					{
						PreviousHitRayTrapBase->ChangeCollaborationPartsRestoreOriginalMaterial(this);
					}

					PreviousHitRayTrapBase = HitRayTrapBase;
					//UE_LOG(LogTrap, Warning, TEXT("HitRayTrapBase name : %s"), *HitRayTrapBase->GetName());
					HitRayTrapBase->HitRayAndChangeCollaborationPartsMaterial(this);
				}
			}
		}
		else
		{
			PlayerHud->SetTrapAimFrameVisibility(false);

			if (IsValid(HitRayTrapBase.Get()))
			{
				HitRayTrapBase->ChangeCollaborationPartsRestoreOriginalMaterial(this);
				HitRayTrapBase = nullptr;
			}
		}
	}
	else if (bHelp)
	{
		bHelp = false;
		PlayerHud->SetTrapAimFrameVisibility(false);

		if (IsValid(HitRayTrapBase.Get()))
		{
			HitRayTrapBase->ChangeCollaborationPartsRestoreOriginalMaterial(this);
			HitRayTrapBase = nullptr;
		}
	}

	FindMagneticPillar();

	// F키 출력
	if (!TrapperGameState->bAbleActiveF)
	{
		if (bIsInteractUIVisible)
		{
			PlayerHud->SetInteractFrameVisibility(false);
			bIsInteractUIVisible = false;
		}

		return;
	}

	Start = GetActorLocation();
	End = Start + GetActorForwardVector() * InteractDistance;

	FCollisionShape Sphere = FCollisionShape::MakeSphere(InteractRadius);
	FHitResult HitResult;
	bool HasHit = GetWorld()->SweepSingleByChannel(
		HitResult,
		Start, End,
		FQuat::Identity,
		ECC_GameTraceChannel9,
		Sphere
	);

	if (HasHit && !bIsInteractUIVisible)
	{
		AInteract* InteractActor = Cast<AInteract>(HitResult.GetActor());
		if (InteractActor && InteractActor->ActorHasTag("Interact"))
		{
			PlayerHud->SetInteractFrameVisibility(true);
			bIsInteractUIVisible = true;
		}
	}

	if (!HasHit && bIsInteractUIVisible)
	{
		PlayerHud->SetInteractFrameVisibility(false);
		bIsInteractUIVisible = false;
	}

}

void ATrapperPlayer::MulticastRPCPlayInstallAnim_Implementation()
{
	StartInstallAnim();
	GetWorld()->GetTimerManager().SetTimer(AnimHandle, this, &ATrapperPlayer::EndInstallAnimTimer, 1.14f, false);
}

void ATrapperPlayer::ServerRPCPlayInstallAnim_Implementation()
{
	MulticastRPCPlayInstallAnim();
}


void ATrapperPlayer::ServerRPCSkip_Implementation()
{
	SkipCurrentStage();
}

void ATrapperPlayer::ServerRPCSkipGaugeChange_Implementation(float Value)
{
	SkipGauge = Value;
	OnRep_SkipGauge();
}

#pragma endregion Interact


/// ------------------------------------------------------------------------------
///										Trap
/// ------------------------------------------------------------------------------

// 함정 관련

#pragma region Trap

void ATrapperPlayer::EnterInstallationMode()
{
	UE_LOG(LogTemp, Warning, TEXT("MaintenanceCount %d"), TrapperGameState->MaintenanceCount);
	// 함정 튜토리얼
	if (TrapperGameState->MaintenanceCount == 0)
	{
		if (HasAuthority())
		{
			TrapperGameState->OnQuestExecute.Broadcast(12, true);
		}
		else
		{
			TrapperGameState->OnQuestExecute.Broadcast(12, false);
		}
	}

	bInstallationMode = true;
	BlockBow();
}

void ATrapperPlayer::ReleaseInstallationMode()
{
	bInstallationMode = false;
	UnblockBow();
}

uint8 ATrapperPlayer::IsTrapSelectMode() const
{
	return TrapMechanics->IsTrapSelectMode();
}

void ATrapperPlayer::StartInstallAnim()
{
	BlockBow();
	SetPreviewStartAnim(true);

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && TrapInstallMontage)
	{
		AnimInstance->Montage_Play(TrapInstallMontage, 1.0);
		AnimInstance->Montage_JumpToSection(FName("StartInstall"), TrapInstallMontage);
		//UE_LOG(LogTemp, Warning, TEXT("Start Anim"));
	}
}

void ATrapperPlayer::EndInstallAnim()
{
	UnblockBow();
	SetPreviewStartAnim(false);

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && TrapInstallMontage)
	{
		AnimInstance->Montage_Play(TrapInstallMontage, 1.0);
		AnimInstance->Montage_JumpToSection(FName("EndInstall"), TrapInstallMontage);
		//UE_LOG(LogTemp, Warning, TEXT("End Anim"));

	}
}

void ATrapperPlayer::UpgradeTrapInstallCost(float CostIcreaseDecreaseRate)
{
	MulticastRPCUpgradeTrapInstallCost(CostIcreaseDecreaseRate);
}

void ATrapperPlayer::UpgradeTrapDamage(float Rate)
{
	MulticastRPCUpgradeTrapDamage(Rate);
}

void ATrapperPlayer::EndInstallAnimTimer()
{
	EndInstallAnim();

	if (InteractionSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, InteractionSound, GetActorLocation());
	}
}

void ATrapperPlayer::MulticastRPCUpgradeTrapInstallCost_Implementation(float Rate)
{
	TrapMechanics->UpgradeCost(Rate);
	if (PlayerController && IsValid(PlayerController->TrapSelectHUDRef))
	{
		PlayerController->TrapSelectHUDRef->UpgradeCost(Rate);
	}
}

void ATrapperPlayer::MulticastRPCUpgradeTrapDamage_Implementation(float Rate)
{
	if (PlayerController && IsValid(PlayerController->TrapSelectHUDRef))
	{
		PlayerController->TrapSelectHUDRef->UpgradeDamage(Rate);
	}
}

bool ATrapperPlayer::IsInstallSpikeTrap() const
{
	if (SpikeNum <= 0)
	{
		return false;
	}

	return true;
}

bool ATrapperPlayer::IsInstallGunpowderBarrelTrap() const
{
	if (GunpowderBarrelNum <= 0)
	{
		return false;
	}

	return true;
}

void ATrapperPlayer::ShowTrapFailInstallHUD(ETrapFailInstallType Type)
{
	if (Type == ETrapFailInstallType::Money)
	{
		PlayerHud->SetTrapFailInstallText(L"재화가 부족합니다.");
	}
	else if (Type == ETrapFailInstallType::Num)
	{
		PlayerHud->SetTrapFailInstallText(L"설치 가능 수량을 초과했습니다.");
	}
	else if (Type == ETrapFailInstallType::Location)
	{
		PlayerHud->SetTrapFailInstallText(L"설치 불가능한 위치입니다.");
	}
	else if (Type == ETrapFailInstallType::NearLocation)
	{
		PlayerHud->SetTrapFailInstallText(L"설치 위치가 너무 가깝습니다.");
	}
	else if (Type == ETrapFailInstallType::Range)
	{
		PlayerHud->SetTrapFailInstallText(L"다른 함정의 공격 범위안에는 함정을 설치할 수 없습니다.");
	}
}

void ATrapperPlayer::SetGunpowderBarrelNum(uint8 Num)
{
	MaxGunpowderBarrelNum = Num;
	GunpowderBarrelNum = Num;
}

void ATrapperPlayer::ClientRPCUpdateTrapHudNum_Implementation(ETrapType Type, bool bInstall)
{
	//UE_LOG(LogTrap, Warning, TEXT("ClientRPCUpdateTrapHudNum_Implementation"));
	uint8 Num = 0;
	if (Type == ETrapType::Spike)
	{
		if (bInstall)
		{
			--SpikeNum;
		}
		else
		{
			++SpikeNum;
		}
		Num = SpikeNum;
		FString NumString = FString::Printf(TEXT("%d/3"), Num);
		PlayerController->TrapSelectHUDRef->SpikeText->SetText(FText::FromString(NumString));

	}
	else if (Type == ETrapType::GunpowderBarrel)
	{
		if (bInstall)
		{
			--GunpowderBarrelNum;

		}
		else
		{
			++GunpowderBarrelNum;
		}
		Num = GunpowderBarrelNum;
		FString NumString = FString::Printf(TEXT("%d"), Num);
		PlayerController->TrapSelectHUDRef->BarrelRemainingCountText->SetText(FText::FromString(NumString));
	}
}

#pragma endregion Trap


/// ------------------------------------------------------------------------------
///										 Bow
/// ------------------------------------------------------------------------------

// 활 관련

#pragma region Bow

void ATrapperPlayer::SetCameraAim(float Value)
{
	Camera->SetFieldOfView(FMath::Lerp(InitialFOV, AimFov, Value));
	SpringArm->SocketOffset = FMath::Lerp(InitialCameraArmOffset, AimCameraArmOffset, Value);
}

void ATrapperPlayer::SetCameraShoot(float Value)
{
	Camera->SetFieldOfView(FMath::Lerp(AimFov, ShootFov, Value));
}

void ATrapperPlayer::BlockBow()
{
	if (IsLocallyControlled())
	{
		//UE_LOG(LogTemp, Warning, TEXT("BlockBow"));
		BowMechanics->EnterBlocked();
	}
}

void ATrapperPlayer::UnblockBow()
{
	if (!IsValid(PlayerController) || !IsValid(PlayerController->GetSystemMenuUMG()))
	{
		BowMechanics->ReleaseBlocked();
		return;
	}

	if (!bInstallationMode
		&& IsLocallyControlled()
		&& !MapMechanics->IsOpenedWorldMap()
		&& !UpgradeMechanics->IsOnUpgradeMode()
		&& !PlayerController->GetSystemMenuUMG()->IsVisible()
		)
	{
		//UE_LOG(LogTemp, Warning, TEXT("UnblockBow"));

		BowMechanics->ReleaseBlocked();
	}
}

#pragma endregion Bow


/// ------------------------------------------------------------------------------
///										 UI
/// ------------------------------------------------------------------------------

// UI 관련

#pragma region UI

void ATrapperPlayer::CreatePlayerHUD()
{
	if (!IsLocallyControlled())
	{
		return;
	}

	PlayerHud = Cast<ATrapperPlayerController>(GetController())->PlayerHudRef;
}

bool ATrapperPlayer::IsPingSelectMode() const
{
	return PingMechanics->IsPingSelectMode();
}

#pragma endregion UI

/// ------------------------------------------------------------------------------
///										 ETC
/// ------------------------------------------------------------------------------

void ATrapperPlayer::MulticastShowMonsterPath_Implementation(bool Value)
{
	if (IsValid(MapMechanics) && IsLocallyControlled())
	{
		MapMechanics->ShowMonsterPath(Value);
	}
}

void ATrapperPlayer::ShowCursor()
{
	PlayerController->SetShowMouseCursor(true);
	FInputModeGameAndUI InputMode;
	//InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::LockAlways);
	InputMode.SetHideCursorDuringCapture(false);
	PlayerController->SetInputMode(InputMode);
	GetWorld()->GetGameViewport()->SetMouseCaptureMode(EMouseCaptureMode::CapturePermanently_IncludingInitialMouseDown);
}

void ATrapperPlayer::HideCursor()
{
	PlayerController->bShowMouseCursor = false;
	FInputModeGameOnly InputMode;
	PlayerController->SetInputMode(InputMode);
	GetWorld()->GetGameViewport()->SetMouseCaptureMode(EMouseCaptureMode::CapturePermanently_IncludingInitialMouseDown);
}

void ATrapperPlayer::SetCursorToCenter()
{
	if (GEngine && GEngine->GameViewport)
	{
		// 현재 게임 화면의 크기를 가져옴
		FVector2D ViewportSize;
		GEngine->GameViewport->GetViewportSize(ViewportSize);

		// 화면의 중앙 좌표 계산
		FVector2D CenterOfViewport = FVector2D(ViewportSize.X / 2, ViewportSize.Y / 2);

		// 마우스 커서 위치를 중앙으로 설정
		FViewport* Viewport = GEngine->GameViewport->Viewport;
		if (Viewport)
		{
			Viewport->SetMouse(CenterOfViewport.X, CenterOfViewport.Y);
		}
	}
}

FVector2D ATrapperPlayer::GetCursorPosition()
{
	FVector2D MousePosition;
	GEngine->GameViewport->GetMousePosition(MousePosition);
	return MousePosition;
}

float ATrapperPlayer::GetViewportScale()
{
	return UWidgetLayoutLibrary::GetViewportScale(GetWorld());
}

void ATrapperPlayer::S2SSetAwaken(bool bIsServer)
{
	MulticastRPCSetAwaken(bIsServer);
	BowMechanics->SetAwaken();
}

void ATrapperPlayer::MulticastRPCSetAwaken_Implementation(bool bIsServer)
{
	bIsAwaken = true;

	if (!IsLocallyControlled())
	{
		return;
	}

	if (bIsServer && IsValid(AwakeEffect1P) && IsValid(AwakeStartEffect1P))
	{
		UNiagaraFunctionLibrary::SpawnSystemAttached(
			AwakeEffect1P, RootComponent,
			NAME_None,
			FVector(0, 0, -90), FRotator(0.f),
			EAttachLocation::Type::KeepRelativeOffset, true, true);

		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(), AwakeStartEffect1P, GetActorLocation());
	}
	else if (!bIsServer && IsValid(AwakeEffect2P) && IsValid(AwakeStartEffect2P))
	{
		UNiagaraFunctionLibrary::SpawnSystemAttached(
			AwakeEffect2P, RootComponent,
			NAME_None,
			FVector(0, 0, -90), FRotator(0.f),
			EAttachLocation::Type::KeepRelativeOffset, true, true);

		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(), AwakeStartEffect2P, GetActorLocation());
	}

	if (IsValid(AwakenStartSound))
	{
		AudioComponent->SetSound(AwakenStartSound);
		AudioComponent->Play();
	}

	if (IsValid(AwakenLoopSound))
	{
		AwakenAudioComponent->SetSound(AwakenLoopSound);
		AwakenAudioComponent->Play();
	}
}

void ATrapperPlayer::MulticastRPCStopAwakenSound_Implementation()
{
	AwakenAudioComponent->Stop();
}
