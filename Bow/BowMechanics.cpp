

#include "BowMechanics.h"

#include "TrapperProject.h"

#include "Animation/AnimMontage.h"
#include "Animation/AnimInstance.h"
#include "EnhancedInputComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "EngineUtils.h"
#include "Camera/CameraComponent.h"
#include "Components/BoxComponent.h"
#include "Sound/SoundCue.h"
#include "Components/AudioComponent.h"

#include "Character/TrapperPlayer.h"
#include "Character/TrapperPlayerAnimInstance.h"
#include "Framework/TrapperGameState.h"
#include "Widget/PlayerHUD.h"
#include "Bow.h"
#include "Arrow.h"



UBowMachanics::UBowMachanics()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
}


void UBowMachanics::BeginPlay()
{
	Super::BeginPlay();

	//초기 설정
	CharacterRef = Cast<ATrapperPlayer>(GetOwner());
	GameState = GetWorld()->GetGameState<ATrapperGameState>();
	AnimInstance = Cast<UTrapperPlayerAnimInstance>(CharacterRef->GetMesh()->GetAnimInstance());

	if(IsValid(AnimInstance))
	{
		AnimInstance->OnTakeArrow.AddUObject(this, &UBowMachanics::OnTakeArrowNotify);
		AnimInstance->OnDrawBow.AddUObject(this, &UBowMachanics::OnDrawBowNotify);
		AnimInstance->OnReloadEnd.AddUObject(this, &UBowMachanics::OnReloadEndNotify);
	}

	// 인풋 바인딩
	SetupInputComponent();

	// 활, 화살 장착
	if(CharacterRef->HasAuthority())
	{
		AttachBow();
		ServerRPCSpawnArrow(ArrowType);
	}

	GetWorld()->GetTimerManager().SetTimer(
		ArrowPleaseStartTimer, this, &UBowMachanics::ArrowPleaseStart, 5, false);
}


void UBowMachanics::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	GetWorld()->GetTimerManager().ClearAllTimersForObject(this);
}

void UBowMachanics::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UBowMachanics, bIsHandlingBow);
	DOREPLIFETIME(UBowMachanics, bIsDrawingBow);
	DOREPLIFETIME(UBowMachanics, bIsCombatMode);
	DOREPLIFETIME(UBowMachanics, BowRef);
	DOREPLIFETIME(UBowMachanics, ArrowRef);
	DOREPLIFETIME(UBowMachanics, bIsAwaken);
}

void UBowMachanics::ApplyDefaultArrowData()
{
	if(bIsAwaken)
	{
		ArrowRef->MinSpeed = ArrowMinSpeed;
		ArrowRef->MaxSpeed = ArrowMaxSpeed;
		ArrowRef->MinDamage = 500;
		ArrowRef->MaxDamage = 500;
		ArrowRef->bIsAwaken = true;
	}
	else
	{
		ArrowRef->MinSpeed = ArrowMinSpeed;
		ArrowRef->MaxSpeed = ArrowMaxSpeed;
		ArrowRef->MinDamage = NormalArrowMinDamage;
		ArrowRef->MaxDamage = NormalArrowMaxDamage;
	}
}

void UBowMachanics::ApplyMagneticArrowData()
{
	if(bIsAwaken)
	{
		ArrowRef->MinSpeed = ArrowMinSpeed;
		ArrowRef->MaxSpeed = ArrowMaxSpeed;
		ArrowRef->MinDamage = 500;
		ArrowRef->MaxDamage = 500;
		ArrowRef->bIsAwaken = true;
	}
	else
	{
		ArrowRef->MinSpeed = ArrowMinSpeed;
		ArrowRef->MaxSpeed = ArrowMaxSpeed;
		ArrowRef->MinDamage = MagneticArrowMinDamage;
		ArrowRef->MaxDamage = MagneticArrowMaxDamage;
	}
}

void UBowMachanics::SetAwaken()
{
	bIsAwaken = true;
	ServerRPCDestroyArrow();
	ServerRPCSpawnArrow(ArrowType);
}

void UBowMachanics::SetupInputComponent()
{
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if(!PlayerController)
	{
		return;
	}

	UEnhancedInputComponent* UIC = Cast<UEnhancedInputComponent>(PlayerController->InputComponent);
	if(IsValid(UIC))
	{
		UIC->BindAction(FireAction, ETriggerEvent::Started, this, &UBowMachanics::DrawBegin);
		UIC->BindAction(FireAction, ETriggerEvent::Completed, this, &UBowMachanics::FireArrowBegin);
		//UIC->BindAction(SwitchArrowAction, ETriggerEvent::Completed, this, &UBowMachanics::SwitchArrowType);
	}
}


///   ---------------------------------------------------------------------------------------------------
///							          발사 가능 상태
///   ---------------------------------------------------------------------------------------------------

void UBowMachanics::EnterBlocked()
{
	//UE_LOG(LogBow, Error, TEXT("Block, Combat : %d, Drawing : %d, Handling : %d"), bIsCombatMode, bIsDrawingBow, bIsHandlingBow);
	bIsBlocked = true;
	CombatModeEnd();

	if(bIsDrawingBow)
	{
		DrawEnd();
		bIsInputTriggerOnGoing = false;
	}
	
	if(bIsHandlingBow)
	{
		FireArrowEnd();
	}

	if(!IsValid(ArrowRef))
	{
		ServerRPCSpawnArrow(ArrowType);
	}
}

void UBowMachanics::ReleaseBlocked()
{
// 	GetWorld()->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateLambda([this]()
// 		{
// 			this->bIsBlocked = false;
// 		}));
	GetWorld()->GetTimerManager().SetTimerForNextTick(this, &UBowMachanics::ReleaseBlockReal);
}

void UBowMachanics::ReleaseBlockReal()
{
	bIsBlocked = false;
	//UE_LOG(LogBow, Warning, TEXT("Release"));
}


///   ---------------------------------------------------------------------------------------------------
///							                     활
///   ---------------------------------------------------------------------------------------------------


void UBowMachanics::AttachBow()
{
	BowRef = Cast<ABow>(GetWorld()->SpawnActor<ABow>(BowClass));

	if(IsValid(BowRef) && IsValid(CharacterRef))
	{
		FAttachmentTransformRules AttachRules(
			EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, true);

		BowRef->AttachToComponent(CharacterRef->GetMesh(), AttachRules, BowSocketName);
		BowRef->SetOwner(CharacterRef);
	}
}


///   ---------------------------------------------------------------------------------------------------
///							                      화살
///   ---------------------------------------------------------------------------------------------------

void UBowMachanics::SwitchArrowType(const FInputActionValue& Value)
{
	if(!CharacterRef->IsLocallyControlled() || bIsBlocked || !bCanSwitchArrow)
	{
		return;
	}

	bCanSwitchArrow = false;
	GetWorld()->GetTimerManager().SetTimer(
		SwitchArrowTimer, this, &UBowMachanics::SwitchArrowCoolDownTimer, 0.2, false);

	if(ArrowType == EArrowType::Default)	ArrowType = EArrowType::Magnetic;
	else									ArrowType = EArrowType::Default;

	ServerRPCDestroyArrow();
	ServerRPCSpawnArrow(ArrowType);

	if(IsValid(CharacterRef->PlayerHud))
	{
		if(ArrowType == EArrowType::Default)
		{
			CharacterRef->PlayerHud->SetArrowControlIcon(false);
			CharacterRef->PlayerHud->SetMagneticFrameVisibility(false);
		}
		else
		{
			CharacterRef->PlayerHud->SetArrowControlIcon(true);
			CharacterRef->PlayerHud->SetMagneticFrameVisibility(true);
		}
	}

	// 자성화살 튜토리얼
	if(GameState->CurrentGameProgress != EGameProgress::Tutorial)
	{
		return;
	}

	if(CharacterRef->HasAuthority())
	{
		GameState->OnQuestExecute.Broadcast(3, true);
	}
	else
	{
		GameState->OnQuestExecute.Broadcast(3, false);
	}
}

void UBowMachanics::SwitchArrowSocket(EArrowSocketType type)
{
	if(!IsValid(CharacterRef)
		|| !IsValid(ArrowRef)
		|| type == ArrowSocketType)
		return;

	ArrowSocketType = type;

	FDetachmentTransformRules DetachRules = FDetachmentTransformRules(
		EDetachmentRule::KeepWorld, EDetachmentRule::KeepWorld, EDetachmentRule::KeepWorld, false);

	FAttachmentTransformRules AttachRules(
		EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, true);

	ArrowRef->DetachFromActor(DetachRules);

	switch(type)
	{
	case EArrowSocketType::RightHand:
		ArrowRef->AttachToComponent(CharacterRef->GetMesh(), AttachRules, ArrowSocketName);
		break;

	case EArrowSocketType::Nocked:
		ArrowRef->AttachToComponent(CharacterRef->GetMesh(), AttachRules, NockedArrowSocketName);
		break;
	}
}

void UBowMachanics::ClientFireArrow(float drawTime, FTransform ArrowTransform)
{
	AArrow* Temp = nullptr;

	// 화살 스폰
	if(ArrowType == EArrowType::Default && !bIsAwaken)
	{
		Temp = Cast<AArrow>(GetWorld()->SpawnActorDeferred<AArrow>(ClientDefaultArrowClass, ArrowTransform));
	}
	else if(ArrowType == EArrowType::Magnetic && !bIsAwaken)
	{
		Temp = Cast<AArrow>(GetWorld()->SpawnActorDeferred<AArrow>(ClientMagneticArrowClass, ArrowTransform));
	}
	else if(ArrowType == EArrowType::Default && bIsAwaken)
	{
		Temp = Cast<AArrow>(GetWorld()->SpawnActorDeferred<AArrow>(ClientDefaultAwakenArrowClass, ArrowTransform));
		Temp->bIsAwaken = true;
	}
	else if(ArrowType == EArrowType::Magnetic && bIsAwaken)
	{
		Temp = Cast<AArrow>(GetWorld()->SpawnActorDeferred<AArrow>(ClientMagneticAwakenArrowClass, ArrowTransform));
		Temp->bIsAwaken = true;
	}

	if(IsValid(Temp))
	{
		Temp->CollisionBox->SetCollisionProfileName("NoCollision");
		Temp->SetOwner(CharacterRef);
		Temp->FinishSpawning(ArrowTransform);

		float Strength = UKismetMathLibrary::NormalizeToRange(drawTime, 0.0, MaxDrawTime);
		Strength = FMath::Clamp(Strength, 0.0, 1.0);
		Temp->FireArrow(CalculateAimDirection(), Strength);
	}
}

FVector UBowMachanics::CalculateAimDirection()
{
	UCameraComponent* Camera = CharacterRef->GetComponentByClass<UCameraComponent>();
	if(!IsValid(Camera) || !IsValid(ArrowRef))
		return FVector();

	FHitResult Hit;
	FVector Start = Camera->GetComponentLocation() + Camera->GetForwardVector() * FVector::Distance(CharacterRef->GetActorLocation(), Camera->GetComponentLocation());
	FVector End = Camera->GetComponentLocation() + Camera->GetForwardVector() * 10000.f;
	ECollisionChannel TraceChannel = ECollisionChannel::ECC_Visibility;
	bool bIsCatched = GetWorld()->LineTraceSingleByChannel(Hit, Start, End, TraceChannel);

	FVector DirectionFrom = ArrowRef->GetActorLocation();
	FVector DirectionTo = Hit.TraceEnd;
	if(bIsCatched)
	{
		DirectionTo = Hit.Location;
	}

	return (DirectionTo - DirectionFrom).GetSafeNormal();
}


///   -------------------------------------------------------------------------------------
///							          타이머 및 상태값 델리게이트
///   -------------------------------------------------------------------------------------

void UBowMachanics::IncrementDrawTime()
{
	DrawTime += DrawIncrementTime * GameState->ChargeSpeed;
	if(GameState->bIsRemovedArrowCharge)
	{
		DrawTime = MaxDrawTime;
	}

	if(IsValid(CharacterRef) && IsValid(CharacterRef->PlayerHud))
	{
		 CharacterRef->PlayerHud->SetDrawFrameScale(DrawTime, MaxDrawTime);
	}
}


///   -------------------------------------------------------------------------------------
///							          애니메이션 델리게이트
///   -------------------------------------------------------------------------------------

void UBowMachanics::OnTakeArrowNotify()
{
	if(CharacterRef->IsLocallyControlled())
	{
		ServerRPCSpawnArrow(ArrowType);
	}
}

void UBowMachanics::OnDrawBowNotify()
{
	BowRef->DrawState = EBowDrawState::E_Draw;
}

void UBowMachanics::OnReloadEndNotify()
{
	if(CharacterRef->IsLocallyControlled())
	{
		//UE_LOG(LogBow, Log, TEXT("OnReloadEnd Notify"));
		FireArrowEnd();
	}
}

void UBowMachanics::OnReloadEndDelegate(UAnimMontage* Montage, bool bInterrupted)
{
// 	if(CharacterRef->IsLocallyControlled())
// 	{
// 		UE_LOG(LogBow, Log, TEXT("OnReloadEnd Notify"));
// 		FireArrowEnd();
// 	}
}

///   ---------------------------------------------------------------------------------------------------
///							          조준 모드
///   ---------------------------------------------------------------------------------------------------

void UBowMachanics::CombatModeBegin()
{
	//UE_LOG(LogBow, Log, TEXT("Begin Combat"));
	
	bIsCombatMode = true;
	ServerRPCChangeCombatMode(true);

	if(IsValid(CharacterRef))
	{
		CharacterRef->BowCameraTimeLine.SetPlayRate(1.f / CharacterRef->BowCameraLerpDuration);
		CharacterRef->BowCameraTimeLine.Play();
	}
}

void UBowMachanics::CombatModeEnd()
{

	if(!bIsCombatMode)
	{
		//UE_LOG(LogBow, Log, TEXT("End Combat, return"));
		return;
	}
	//UE_LOG(LogBow, Log, TEXT("End Combat"));

	if(IsValid(CharacterRef))
	{
		if(bIsBlocked)
			CharacterRef->BowCameraTimeLine.SetPlayRate(1.f / CharacterRef->BowCameraLerpDuration);
		else
			CharacterRef->BowCameraTimeLine.SetPlayRate(1.f / CharacterRef->BowCameraLerpDurationReverse);

		CharacterRef->BowCameraTimeLine.Reverse();
	}

	bIsCombatMode = false;
	ServerRPCChangeCombatMode(false);
}

///   ---------------------------------------------------------------------------------------------------
///							          당기기
///   ---------------------------------------------------------------------------------------------------

void UBowMachanics::DrawBegin()
{
	if(!CharacterRef->IsLocallyControlled() || !bPleasePleasePlease)
	{
		return;
	}

	bIsInputTriggerOnGoing = true;

	if(bIsHandlingBow || bIsBlocked)
	{
		return;
	}
	
	//UE_LOG(LogBow, Log, TEXT("Begin Draw"));

	// 사운드
	if(DrawSound && CharacterRef->AudioComponent && !bIsAwaken)
	{
		CharacterRef->AudioComponent->SetSound(DrawSound);
		CharacterRef->AudioComponent->Play();
	}
	else if(DrawAwakenSound && CharacterRef->AudioComponent && bIsAwaken)
	{
		CharacterRef->AudioComponent->SetSound(DrawAwakenSound);
		CharacterRef->AudioComponent->Play();
	}

	// UI
	CharacterRef->PlayerHud->SetDrawFrameVisibility(true);

	// 카메라 줌인, 자세 변경
	CombatModeBegin();

	// 상태 변수 변경
	bIsHandlingBow = true;
	bIsDrawingBow = true;
	BowRef->DrawState = EBowDrawState::E_Draw;
	SwitchArrowSocket(EArrowSocketType::RightHand);

	ServerRPCChangeHandlingBow(true);
	ServerRPCChangeDrawingBow(true);
	ServerRPCChangeDrawState(EBowDrawState::E_Draw);
	ServerRPCSwitchArrowSocket(EArrowSocketType::RightHand);

	// 당기기 타이머 설정
	GetWorld()->GetTimerManager().SetTimer(DrawBowTimer, this, &UBowMachanics::IncrementDrawTime, DrawIncrementTime, true);

	// 컴뱃모드 시간 초기화
	GetWorld()->GetTimerManager().ClearTimer(CombatTimer);

	// 카메라, 컨트롤 변경
	if(IsValid(CharacterRef))
	{
		CharacterRef->CharacterControlTypeCheck();
		//CharacterRef->CameraFixEnd(FInputActionValue());
	}
}


void UBowMachanics::DrawEnd()
{	
	//UE_LOG(LogBow, Log, TEXT("End Draw"));

	// 타이머 초기화	
	GetWorld()->GetTimerManager().ClearTimer(DrawBowTimer);
	DrawTime = 0.0f;

	// 상태 변수 변경
	bIsDrawingBow = false;
	BowRef->DrawState = EBowDrawState::E_Idle;

	ServerRPCChangeDrawingBow(false);
	ServerRPCChangeDrawState(EBowDrawState::E_Idle);

	if(IsValid(CharacterRef))
	{
		// 카메라 제어 변경
		CharacterRef->CharacterControlTypeCheck();

		// HUD
		CharacterRef->PlayerHud->SetDrawFrameScale(0, MaxDrawTime);
		CharacterRef->PlayerHud->SetDrawFrameVisibility(false);
	}
}


///   ---------------------------------------------------------------------------------------------------
///							          화살 발사
/// 
///			키를 뗐을 때 FireArrowBegin() -> 몽타주 재생 -> 재생이 끝나면 발사 끝
///   ---------------------------------------------------------------------------------------------------

void UBowMachanics::FireArrowBegin()
{
	if(!CharacterRef->IsLocallyControlled())
	{
		return;
	}

	bIsInputTriggerOnGoing = false;

	if(!bIsDrawingBow || bIsBlocked)
	{
		return;
	}

	//UE_LOG(LogBow, Log, TEXT("Begin Fire"));

	// 사운드
	if(IsValid(FireSound) && IsValid(CharacterRef->AudioComponent) && !bIsAwaken)
	{
		CharacterRef->AudioComponent->SetSound(FireSound);
		CharacterRef->AudioComponent->Play();
	}
	else if(IsValid(FireAwakenSound) && IsValid(CharacterRef->AudioComponent) && bIsAwaken)
	{
		CharacterRef->AudioComponent->SetSound(FireAwakenSound);
		CharacterRef->AudioComponent->Play();
	}

	// 카메라 무빙
	if(IsValid(CharacterRef))
	{
		CharacterRef->BowCameraShootTimeLine.SetPlayRate(1.f / CharacterRef->BowCameraLerpDurationShoot);
		CharacterRef->BowCameraShootTimeLine.PlayFromStart();
	}

	// 화살 발사, 몽타주 재생
	FTransform ArrowTransform;
	if(IsValid(ArrowRef))
	{
		ArrowTransform = ArrowRef->GetActorTransform();
	}
	ServerRPCFireArrow(DrawTime, ArrowTransform);

	if(!CharacterRef->HasAuthority())
	{
		ClientFireArrow(DrawTime, ArrowTransform);
	}

	// 상태 변수 변경
	DrawEnd();
}


void UBowMachanics::FireArrowEnd()
{
	//UE_LOG(LogBow, Log, TEXT("End Fire"));

	// 상태 변수 변경
	bIsHandlingBow = false;
	SwitchArrowSocket(EArrowSocketType::Nocked);
	BowRef->DrawState = EBowDrawState::E_Idle;

	ServerRPCChangeHandlingBow(false);
	ServerRPCSwitchArrowSocket(EArrowSocketType::Nocked);
	ServerRPCChangeDrawState(EBowDrawState::E_Idle);

	// 전투모드 타이머 시작
	GetWorld()->GetTimerManager().SetTimer(CombatTimer, this, &UBowMachanics::CombatModeEnd, CombatModeDuration, false);

	// 장전 중에 클릭하고 있다면 바로 다시 발사 가능하게 함
	if(bIsInputTriggerOnGoing && !bIsBlocked)
	{
		DrawBegin();
	}
}



void UBowMachanics::ArrowPleaseStart()
{
	bPleasePleasePlease = true;
}

///   ---------------------------------------------------------------------------------------------------
///							              RPC
///   ---------------------------------------------------------------------------------------------------

void UBowMachanics::ServerRPCSpawnArrow_Implementation(EArrowType Type)
{
	if(ArrowRef)
	{
		return;
	}

	FTransform SpawnTransform = FTransform(CharacterRef->GetActorLocation());

	// 화살 스폰
	if(Type == EArrowType::Default && !bIsAwaken)
	{
		ArrowRef = Cast<AArrow>(GetWorld()->SpawnActorDeferred<AArrow>(DefaultArrowClass, SpawnTransform));
		ApplyDefaultArrowData();
	}
	else if(Type == EArrowType::Magnetic && !bIsAwaken)
	{
		ArrowRef = Cast<AArrow>(GetWorld()->SpawnActorDeferred<AArrow>(MagneticArrowClass, SpawnTransform));
		ApplyMagneticArrowData();
	}
	else if(Type == EArrowType::Default && bIsAwaken)
	{
		ArrowRef = Cast<AArrow>(GetWorld()->SpawnActorDeferred<AArrow>(DefaultAwakenArrowClass, SpawnTransform));
		ApplyDefaultArrowData();
	}
	else if(Type == EArrowType::Magnetic && bIsAwaken)
	{
		ArrowRef = Cast<AArrow>(GetWorld()->SpawnActorDeferred<AArrow>(MagneticAwakenArrowClass, SpawnTransform));
		ApplyMagneticArrowData();
	}

	ArrowRef->CollisionBox->SetCollisionProfileName("NoCollision");
	ArrowRef->SetOwner(CharacterRef);
	ArrowRef->FinishSpawning(SpawnTransform);

	// 화살 소켓에 연결
	if(CharacterRef && CharacterRef->GetMesh() && ArrowRef)
	{
		FAttachmentTransformRules AttachRules(
			EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, true);

		SwitchArrowSocket(EArrowSocketType::Nocked);
	}
}

void UBowMachanics::ServerRPCDestroyArrow_Implementation()
{
	if(IsValid(ArrowRef))
	{
		ArrowRef->Destroy();
		ArrowRef = nullptr;
		ArrowSocketType = EArrowSocketType::None;
	}
}

void UBowMachanics::ServerRPCSwitchArrowSocket_Implementation(EArrowSocketType type)
{
	SwitchArrowSocket(type);
}

void UBowMachanics::ServerRPCFireArrow_Implementation(float drawTime, FTransform ArrowTransform)
{
	if(IsValid(ArrowRef))
	{
		FDetachmentTransformRules rules = FDetachmentTransformRules(
			EDetachmentRule::KeepWorld, EDetachmentRule::KeepWorld, EDetachmentRule::KeepWorld, false);
		ArrowRef->DetachFromActor(rules);
		ArrowRef->SetActorTransform(ArrowTransform);

		float Strength = UKismetMathLibrary::NormalizeToRange(drawTime, 0.0, MaxDrawTime);
		Strength = FMath::Clamp(Strength, 0.0, 1.0);

		if(Strength >= 0.99)
		{
			ArrowRef->bIsFullCharged = true;
		}

		ArrowRef->FireArrow(CalculateAimDirection(), Strength);
		ArrowRef = nullptr;
		ArrowSocketType = EArrowSocketType::None;
	}

	// 애니메이션
	MulticastRPCPlayFireMontage();
}

void UBowMachanics::MulticastRPCPlayFireMontage_Implementation()
{
	if(IsValid(AnimInstance) && IsValid(FireMontage))
	{
		AnimInstance->Montage_Play(FireMontage);
		//AnimInstance->OnMontageEnded.AddDynamic(this, &UBowMachanics::OnReloadEndDelegate);
	}
}

void UBowMachanics::ServerRPCChangeDrawState_Implementation(EBowDrawState State)
{
	BowRef->DrawState = State;
}

void UBowMachanics::ServerRPCChangeCombatMode_Implementation(bool IsCombatMode)
{
	bIsCombatMode = IsCombatMode;
}

void UBowMachanics::ServerRPCChangeDrawingBow_Implementation(bool IsDrawing)
{
	bIsDrawingBow = IsDrawing;
}

void UBowMachanics::ServerRPCChangeHandlingBow_Implementation(bool IsHandling)
{
	bIsHandlingBow = IsHandling;
}

