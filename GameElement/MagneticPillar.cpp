#include "MagneticPillar.h"

#include "Components/PostProcessComponent.h"
#include "Components/SphereComponent.h"
#include "Components/AudioComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Particles/ParticleSystemComponent.h"

#include "Kismet/KismetMaterialLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"
#include "Net/UnrealNetwork.h"

#include "Character/TrapperPlayer.h"
#include "Framework/TrapperGameState.h"
#include "Bow/Arrow.h"
#include "Monster/ADC.h"
#include "Ping/MarkerComponent/MapMarkerComponent.h"
#include "Ping/PingMechanics.h"

AMagneticPillar::AMagneticPillar()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
	Tags.Add(FName("MagneticPillar"));

	Foundation = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Foundation"));
	RootComponent = Foundation;

	RuneStoneShell = CreateDefaultSubobject<USceneComponent>(TEXT("RuneStoneShell"));
	RuneStoneShell->SetupAttachment(RootComponent);

	RuneStone = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RuneStone"));
	RuneStone->SetupAttachment(RuneStoneShell);
	RuneStone->SetRelativeLocation(FVector(0, 0, InitHeight));

	TriggerArea1 = CreateDefaultSubobject<USphereComponent>(TEXT("TriggerArea1"));
	TriggerArea1->SetupAttachment(Foundation);
	TriggerArea1->SetSphereRadius(900);

	AudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("AudioComponent"));
	AudioComponent->SetupAttachment(RuneStoneShell);

	MarkerComponent = CreateDefaultSubobject<UMapMarkerComponent>(TEXT("MarkerComponent"));
	MarkerComponent->ObjectType = EMapObjectType::Pillar;
	AddOwnedComponent(MarkerComponent);

	EffectSpawnPoint0 = CreateDefaultSubobject<USceneComponent>(TEXT("EffectSpawnPoint0"));
	EffectSpawnPoint0->SetupAttachment(RuneStoneShell);

	CoolTimeEffectSpawnPoint = CreateDefaultSubobject<USceneComponent>(TEXT("CoolTimeEffectSpawnPoint"));
	CoolTimeEffectSpawnPoint->SetupAttachment(RuneStoneShell);

	ShineOutline = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Particle"));
	ShineOutline->bAutoActivate = false;
	ShineOutline->SetupAttachment(RootComponent);

	RuneStone->OnComponentHit.AddDynamic(this, &AMagneticPillar::OnArrowHit);
	TriggerArea1->OnComponentBeginOverlap.AddDynamic(this, &AMagneticPillar::OnOverlapBeginMonster);
	TriggerArea1->OnComponentEndOverlap.AddDynamic(this, &AMagneticPillar::OnOverlapEndMonster);

	static ConstructorHelpers::FObjectFinder<UParticleSystem> ParticleAsset(TEXT("/Script/Engine.ParticleSystem'/Game/Blueprints/VFX/Particles/P_ky_laser01.P_ky_laser01'"));
	if(ParticleAsset.Succeeded())
	{
		ShineOutline->SetTemplate(ParticleAsset.Object);
 	}
}

void AMagneticPillar::BeginPlay()
{
	Super::BeginPlay();
	RuneStone->SetRenderCustomDepth(false);
	ShineOutline->SetWorldLocation(RuneStone->GetComponentLocation());
	SpawnEffects();

	RuneStone->SetCanEverAffectNavigation(false);

	GameState = GetWorld()->GetGameState<ATrapperGameState>();

	if(HasAuthority())
	{
		TimeOffset = FMath::FRandRange(0, 1.f / HeightFrequency);
		float InitYaw = FMath::FRandRange(0, 360.f);

		BouncingSpeed.X = FMath::FRandRange(MinBouncingSpeed, MaxBouncingSpeed);
		BouncingSpeed.Y = FMath::FRandRange(MinBouncingSpeed, MaxBouncingSpeed);
		BouncingSpeed.Z = FMath::FRandRange(MinBouncingSpeed, MaxBouncingSpeed);

		BouncingAmplitude.X = FMath::FRandRange(MaxBouncingAmplitute * -1, MaxBouncingAmplitute);
		BouncingAmplitude.Y = FMath::FRandRange(MaxBouncingAmplitute * -1, MaxBouncingAmplitute);
		BouncingAmplitude.Z = FMath::FRandRange(MaxBouncingAmplitute * -1, MaxBouncingAmplitute);

		PitchBouncingAngle = FMath::FRandRange(MinBouncingAngle, MaxBouncingAngle);
		RollBouncingAngle = FMath::FRandRange(MinBouncingAngle, MaxBouncingAngle);
		PitchBouncingAngleSpeed = FMath::FRandRange(MinBouncingAngleSpeed, MaxBouncingAngleSpeed);
		RollBouncingAngleSpeed = FMath::FRandRange(MinBouncingAngleSpeed, MaxBouncingAngleSpeed);

		RuneStone->SetRelativeRotation(FRotator(0, InitYaw, 0));

		GetWorld()->GetTimerManager().SetTimer(
			TransformUpdaterTimer, this, &AMagneticPillar::UpdateTransform, TransformDeltaSeconds, true);

		if(IsValid(GameState))
		{
			GameState->S2SRegistMagneticPillar(this);
			if(ActivationState == EMagneticPillarActivationState::E_Locked)
			{
				GetWorld()->GetTimerManager().SetTimerForNextTick(this, &AMagneticPillar::LockNextTick);
			}
		}
	}
}

void AMagneticPillar::EndPlay(EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	GetWorld()->GetTimerManager().ClearAllTimersForObject(this);
}

void AMagneticPillar::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AMagneticPillar, ActivationState);
}

void AMagneticPillar::SetOutline(bool Value)
{
	RuneStone->SetRenderCustomDepth(Value);

	if(!ShineOutline) return;

	if(Value)
	{
		ShineOutline->Activate();
	}
	else
	{
		ShineOutline->Deactivate();
	}
}

bool AMagneticPillar::IsActivated() const
{
	return (ActivationState == EMagneticPillarActivationState::E_1PActivated
		|| ActivationState == EMagneticPillarActivationState::E_2PActivated);
}

bool AMagneticPillar::IsLocked() const
{
	return (ActivationState == EMagneticPillarActivationState::E_Locked);
}

void AMagneticPillar::S2SSetLock(bool bLock)
{
	if(bLock)
	{
		if(ActivationState == EMagneticPillarActivationState::E_Locked)
		{
			return;
		}
		else if(ActivationState == EMagneticPillarActivationState::E_None)
		{
			S2SLock();
		}
		else if(ActivationState == EMagneticPillarActivationState::E_CoolTime)
		{
			MulticastRPCDeactivateEffect(EMagneticPillarActivationState::E_CoolTime);
			GetWorld()->GetTimerManager().ClearTimer(CooldownTimer);
			S2SLock();
		}
		else
		{
			bIsPendingLock = true;
		}
	}
	else
	{
		if(ActivationState == EMagneticPillarActivationState::E_Locked)
		{
			ActivationState = EMagneticPillarActivationState::E_None;
			MulticastRPCSetIcon(true);
			MulticastRPCDeactivateEffect(EMagneticPillarActivationState::E_Locked);
		}
	}
}

void AMagneticPillar::Break()
{
	GetWorld()->GetTimerManager().ClearAllTimersForObject(this);
	SetLifeSpan(11);
	ActivationState = EMagneticPillarActivationState::E_Broken;
	MulticastRPCBreak();

	GameState->PillarBroken(this);
}

void AMagneticPillar::S2SLock()
{
	bIsPendingLock = false;
	ActivationState = EMagneticPillarActivationState::E_Locked;
	GameState->PillarDeactivated(this);
	MulticastRPCSetIcon(false);
	MulticastRPCActivateEffect(EMagneticPillarActivationState::E_Locked);
}

void AMagneticPillar::UpdateTransform()
{
	// 서희 상태변화 보간
	if(LastActivationState == EMagneticPillarActivationState::E_Locked
		&& ActivationState == EMagneticPillarActivationState::E_None)
	{
		bIsLerpingActivationState = true;
		GetWorld()->GetTimerManager().SetTimer(
			LerpActivationStateTimer, this, &AMagneticPillar::LerpActivationState, TransformDeltaSeconds, true);
	}

	else if(ActivationState == EMagneticPillarActivationState::E_None && !bIsLerpingActivationState)
	{
		float t = GetWorld()->GetTimeSeconds() + TimeOffset;
		float Height = InitHeight + HeightAmplitude * FMath::Sin(HeightFrequency * t * 6.283185);
		RuneStoneShell->SetRelativeLocation(FVector(0, 0, Height));
		
		float Yaw = RuneStoneShell->GetRelativeRotation().Yaw;
		Yaw += RotationSpeed * TransformDeltaSeconds;
		RuneStoneShell->SetRelativeRotation(FRotator(0, Yaw, 0));
	}

	else if(ActivationState == EMagneticPillarActivationState::E_Locked)
	{
		FVector CurrPos = RuneStoneShell->GetRelativeLocation();
		FVector Pos = FMath::Lerp(CurrPos, FVector(0,0,DeactivatedHeight), TransformDeltaSeconds * 1);
		RuneStoneShell->SetRelativeLocation(Pos);
		//TODO::서희 회전도 천천히 멈추기
	}
	
	else if(ActivationState == EMagneticPillarActivationState::E_1PReady
		|| ActivationState == EMagneticPillarActivationState::E_2PReady)
	{
		FVector CurrPos = RuneStone->GetRelativeLocation();
		FVector Target = BouncingStartPos + BouncingAmplitude;
		
		FVector Pos;
		Pos.X = FMath::Lerp(CurrPos.X, Target.X, BouncingSpeed.X * TransformDeltaSeconds);
		Pos.Y = FMath::Lerp(CurrPos.Y, Target.Y, BouncingSpeed.Y * TransformDeltaSeconds);
		Pos.Z = FMath::Lerp(CurrPos.Z, Target.Z, BouncingSpeed.Z * TransformDeltaSeconds);
		RuneStone->SetRelativeLocation(Pos);

		FRotator CurrRotation = RuneStone->GetRelativeRotation();
		FRotator Rot;
		Rot.Pitch = FMath::Lerp(CurrRotation.Pitch, PitchBouncingAngle, PitchBouncingAngleSpeed * TransformDeltaSeconds);
		Rot.Yaw = CurrRotation.Yaw;
		Rot.Roll = FMath::Lerp(CurrRotation.Roll, RollBouncingAngle, RollBouncingAngleSpeed * TransformDeltaSeconds);
		RuneStone->SetRelativeRotation(Rot);

		if(FMath::Abs(Pos.X - CurrPos.X) < 0.1)
		{
			float Inverse = 1;
			if(0 < BouncingAmplitude.X)
			{
				Inverse = -1;
			}
			BouncingAmplitude.X = FMath::FRandRange(MinBouncingAmplitute, MaxBouncingAmplitute) * Inverse;
			BouncingSpeed.X = FMath::FRandRange(MinBouncingSpeed, MaxBouncingSpeed);
		}

		if(FMath::Abs(Pos.Y - CurrPos.Y) < 0.1)
		{
			float Inverse = 1;
			if(0 < BouncingAmplitude.Y)
			{
				Inverse = -1;
			}
			BouncingAmplitude.Y = FMath::FRandRange(MinBouncingAmplitute, MaxBouncingAmplitute) * Inverse;
			BouncingSpeed.Y = FMath::FRandRange(MinBouncingSpeed, MaxBouncingSpeed);
		}

		if(FMath::Abs(Pos.Z - CurrPos.Z) < 0.1)
		{
			float Inverse = 1;
			if(0 < BouncingAmplitude.Z)
			{
				Inverse = -1;
			}
			BouncingAmplitude.Z = FMath::FRandRange(MinBouncingAmplitute, MaxBouncingAmplitute) * Inverse;
			BouncingSpeed.Z = FMath::FRandRange(MinBouncingSpeed, MaxBouncingSpeed);
		}	

		if(FMath::Abs(Rot.Roll - CurrRotation.Roll) < 0.5)
		{
			float Inverse = 1;
			if(0 < RollBouncingAngle)
			{
				Inverse = -1;
			}
			
			RollBouncingAngle = FMath::FRandRange(MinBouncingAngle, MaxBouncingAngle) * Inverse;
			RollBouncingAngleSpeed = FMath::FRandRange(MinBouncingAngleSpeed, MaxBouncingAngleSpeed); 
		}
		
		if(FMath::Abs(Rot.Pitch - CurrRotation.Pitch) < 0.5)
		{
			float Inverse = 1;
			if(0 < PitchBouncingAngle)
			{
				Inverse = -1;
			}

			PitchBouncingAngle = FMath::FRandRange(MinBouncingAngle, MaxBouncingAngle) * Inverse;
			PitchBouncingAngleSpeed = FMath::FRandRange(MinBouncingAngleSpeed, MaxBouncingAngleSpeed);
		}
	}

	LastActivationState = ActivationState;
}

void AMagneticPillar::LerpActivationState()
{
	// 지금 위치
	float CurrHeight = RuneStoneShell->GetRelativeLocation().Z;

	// 보간될 위치
	float t = GetWorld()->GetTimeSeconds() + TimeOffset;
	float Height = InitHeight + HeightAmplitude * FMath::Sin(HeightFrequency * t * 6.283185);
	
	// 보간
	float ResultHeight = FMath::Lerp(CurrHeight, Height, TransformDeltaSeconds * 1);
	RuneStoneShell->SetRelativeLocation(FVector(0, 0, ResultHeight));

	// 회전 속도 천천히 증가
	float CurrRotationSpeed = FMath::Lerp(RotationSpeed, 0, FMath::Abs(ResultHeight - Height) / (InitHeight - DeactivatedHeight + HeightAmplitude));
	float Yaw = RuneStoneShell->GetRelativeRotation().Yaw;
	Yaw += CurrRotationSpeed * TransformDeltaSeconds;
	RuneStoneShell->SetRelativeRotation(FRotator(0, Yaw, 0));

	// 비슷해지면 보간 끝
	if( FMath::Abs(ResultHeight - Height) < 3)
	{
		GetWorld()->GetTimerManager().ClearTimer(LerpActivationStateTimer);
		bIsLerpingActivationState = false;
	}
}

void AMagneticPillar::LockNextTick()
{
	S2SLock();
}

UPingMechanics* AMagneticPillar::GetPingMechanicsRef()
{
	APawn* Pawn =  GetWorld()->GetFirstPlayerController()->GetPawn();
	if(IsValid(Pawn))
	{
		ATrapperPlayer* PC = Cast<ATrapperPlayer>(Pawn);
		if(PC)
		{
			return PC->PingMechanics;
		}
	}
	return nullptr;
}

void AMagneticPillar::OnArrowHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if(!HasAuthority()
		|| !OtherActor->ActorHasTag("MagneticArrow")
		|| ActivationState == EMagneticPillarActivationState::E_Locked)
	{
		return;
	}

	// 에임 피드백
	ATrapperPlayer* player = Cast<ATrapperPlayer>(OtherActor->GetOwner());
	AArrow* arrow = Cast<AArrow>(OtherActor);
	bool Is1P = true;
	UPingMechanics* pingComp = GetPingMechanicsRef();

	if(player && arrow)
	{
		player->ShowHitFeedbackHUD(false, arrow->IsFullCharged());
		Is1P = arrow->IsSpawnedBy1P();

		if(!arrow->IsActivated())
			return;

		arrow->DeactivateArrow();
	}

	// 활성화
	switch(ActivationState)
	{
	case EMagneticPillarActivationState::E_None:
		if(Is1P)
		{
			ActivationState = EMagneticPillarActivationState::E_1PReady;
			GetWorld()->GetTimerManager().SetTimer(
				ActivationWaitTimer, this, &AMagneticPillar::DeactivateWait, ActivationWaitTime, false);
			MulticastRPCActivateEffect(EMagneticPillarActivationState::E_1PReady);
			BouncingStartPos = RuneStone->GetRelativeLocation();
			
			if(IsValid(pingComp))
			{
				pingComp->S2SSpawnAutoGeneratedPing(this->GetUniqueID(), RuneStone->GetComponentLocation() + FVector(0,0,50), true);
			}
		}
		else
		{
			ActivationState = EMagneticPillarActivationState::E_2PReady;
			GetWorld()->GetTimerManager().SetTimer(
				ActivationWaitTimer, this, &AMagneticPillar::DeactivateWait, ActivationWaitTime, false);
			MulticastRPCActivateEffect(EMagneticPillarActivationState::E_2PReady);
			BouncingStartPos = RuneStone->GetRelativeLocation();

			if(IsValid(pingComp))
			{
				pingComp->S2SSpawnAutoGeneratedPing(this->GetUniqueID(), RuneStone->GetComponentLocation() + FVector(0, 0, 50), false);
			}
		}

		break;

	case EMagneticPillarActivationState::E_1PReady:
		if(Is1P)
		{
			GetWorld()->GetTimerManager().ClearTimer(ActivationWaitTimer);
			GetWorld()->GetTimerManager().SetTimer(
				ActivationWaitTimer, this, &AMagneticPillar::DeactivateWait, ActivationWaitTime, false);
		}
		else
		{
			GameState->OnQuestExecute.Broadcast(10, true);

			ActivationState = EMagneticPillarActivationState::E_2PActivated;

			GetWorld()->GetTimerManager().ClearTimer(ActivationWaitTimer);
			GetWorld()->GetTimerManager().SetTimer(
				ActivationTimer, this, &AMagneticPillar::Deactivate, ActivationTime, false);

			MulticastRPCDeactivateEffect(EMagneticPillarActivationState::E_1PReady);
			MulticastRPCActivateEffect(EMagneticPillarActivationState::E_2PActivated);
			MulticastRPCActivateEffect(EMagneticPillarActivationState::E_CoolTime);

			if(IsValid(pingComp))
			{
				pingComp->S2SDestroyAutoGeneratedPing(this->GetUniqueID());
			}

			for(const auto& Actor : OverappingActors)
			{
				//상태를 업데이트해줌
				UE_LOG(LogTemp, Warning, TEXT("Pillar Actived"));
				Cast<ABaseMonster>(Actor)->MonsterState = EMonsterStateType::IsPillarActived;
				Cast<ABaseMonster>(Actor)->ActivedPillarLocation = this->GetActorLocation();
			}
		}
		break;

	case EMagneticPillarActivationState::E_2PReady:
		if(Is1P)
		{
			GameState->OnQuestExecute.Broadcast(10, true);

			ActivationState = EMagneticPillarActivationState::E_1PActivated;

			GetWorld()->GetTimerManager().ClearTimer(ActivationWaitTimer);
			GetWorld()->GetTimerManager().SetTimer(
				ActivationTimer, this, &AMagneticPillar::Deactivate, ActivationTime, false);

			MulticastRPCDeactivateEffect(EMagneticPillarActivationState::E_2PReady);
			MulticastRPCActivateEffect(EMagneticPillarActivationState::E_1PActivated);
			MulticastRPCActivateEffect(EMagneticPillarActivationState::E_CoolTime);

			if(IsValid(pingComp))
			{
				pingComp->S2SDestroyAutoGeneratedPing(this->GetUniqueID());
			}

			for(const auto& Actor : OverappingActors)
			{
				//상태를 업데이트해줌
				UE_LOG(LogTemp, Warning, TEXT("Pillar Actived"));
				Cast<ABaseMonster>(Actor)->MonsterState = EMonsterStateType::IsPillarActived;
				Cast<ABaseMonster>(Actor)->ActivedPillarLocation = this->GetActorLocation();
			}
		}
		else
		{
			GetWorld()->GetTimerManager().ClearTimer(ActivationWaitTimer);
			GetWorld()->GetTimerManager().SetTimer(
				ActivationWaitTimer, this, &AMagneticPillar::DeactivateWait, ActivationWaitTime, false);
		}
		break;
	}
}

void AMagneticPillar::OnOverlapBeginMonster(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(!HasAuthority()
		|| OtherActor->ActorHasTag("PathMonster")
		|| !OtherActor->ActorHasTag("Monster"))
	{
		return;
	}
	OverappingActors.Add(OtherActor);

	if(IsValid(ForePillar) && !OtherActor->ActorHasTag("Tutorial"))
	{
		ForePillar->S2SSetLock(true);
	}
}

void AMagneticPillar::OnOverlapEndMonster(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if(!HasAuthority()
		|| !IsValid(OtherActor)
		|| OtherActor->ActorHasTag("PathMonster")
		|| !OtherActor->ActorHasTag("Monster"))
	{
		return;
	}

	if (Cast<ABaseMonster>(OtherActor)->MonsterState != EMonsterStateType::IsTurretDetect)
	{
		Cast<ABaseMonster>(OtherActor)->MonsterState = EMonsterStateType::GoToTower;
		//UE_LOG(LogTemp, Warning, TEXT("GoToTower : MagneticPillar"));
	}
	
	OverappingActors.Remove(OtherActor);
}

void AMagneticPillar::SpawnEffects()
{
	if(IsValid(Hit1PNiagara))
	{
		Hit1PEffectRef = UNiagaraFunctionLibrary::SpawnSystemAttached(
			Hit1PNiagara, EffectSpawnPoint0,
			NAME_None,
			FVector(0.f), FRotator(0.f),
			EAttachLocation::Type::KeepRelativeOffset, false, false);
		Hit1PEffectRef->SetRelativeScale3D(FVector(2,2,2));
	}

	if(IsValid(Hit2PNiagara))
	{
		Hit2PEffectRef = UNiagaraFunctionLibrary::SpawnSystemAttached(
			Hit2PNiagara, EffectSpawnPoint0,
			NAME_None,
			FVector(0.f), FRotator(0.f),
			EAttachLocation::Type::KeepRelativeOffset, false, false);
		Hit2PEffectRef->SetRelativeScale3D(FVector(2, 2, 2));
	}

	if(IsValid(Activation1PNiagara))
	{
		Activation1PEffectRef = UNiagaraFunctionLibrary::SpawnSystemAttached(
			Activation1PNiagara, EffectSpawnPoint0,
			NAME_None,
			FVector(0.f), FRotator(0.f),
			EAttachLocation::Type::KeepRelativeOffset, false, false);

	}
	if(IsValid(Activation2PNiagara))
	{
		Activation2PEffectRef = UNiagaraFunctionLibrary::SpawnSystemAttached(
			Activation2PNiagara, EffectSpawnPoint0,
			NAME_None,
			FVector(0.f), FRotator(0.f),
			EAttachLocation::Type::KeepRelativeOffset, false, false);
	}

	if(IsValid(CoolTimeParticle))
	{
		CooldonwEffectRef = UGameplayStatics::SpawnEmitterAttached(
			CoolTimeParticle, CoolTimeEffectSpawnPoint,
			NAME_None, 
			FVector::ZeroVector, FRotator::ZeroRotator, 
			EAttachLocation::KeepRelativeOffset, false);
		
		if(IsValid(CooldonwEffectRef))
		{
			CooldonwEffectRef->Deactivate();
			CooldonwEffectRef->SetRelativeScale3D(FVector(1.6, 1.6, 1.6));
		}
	}

	if(IsValid(LockedNiagara))
	{
		LockedEffectRef = UNiagaraFunctionLibrary::SpawnSystemAttached(
			LockedNiagara, EffectSpawnPoint0,
			NAME_None,
			FVector(0.f), FRotator(0.f),
			EAttachLocation::Type::KeepRelativeOffset, false, false);
	}
}

void AMagneticPillar::DeactivateWait()
{
	if(!HasAuthority())
	{
		return;
	}

	if(ActivationState == EMagneticPillarActivationState::E_1PReady)
	{
		MulticastRPCDeactivateEffect(EMagneticPillarActivationState::E_1PReady);

		// 2P 실패 대사
		if(GameState && GameState->CurrentGameProgress == EGameProgress::Tutorial)
		{
			GameState->OnEventExecute.Broadcast(ETrapperEventCode::D_Soluna_Quest_2P_Fail);
		}
	}
	else if(ActivationState == EMagneticPillarActivationState::E_2PReady)
	{
		MulticastRPCDeactivateEffect(EMagneticPillarActivationState::E_2PReady);

		// 1P 실패 대사
		if(GameState && GameState->CurrentGameProgress == EGameProgress::Tutorial)
		{
			GameState->OnEventExecute.Broadcast(ETrapperEventCode::D_Soluna_Quest_1P_Fail);
		}
	}

	if(bIsPendingLock)
	{
		S2SLock();
	}
	else
	{
		ActivationState = EMagneticPillarActivationState::E_None;
	}
}

void AMagneticPillar::Deactivate()
{
	if(!HasAuthority())
	{
		return;
	}

	MulticastRPCDeactivateEffect(ActivationState);

	if(bIsPendingLock)
	{
		S2SLock();
		MulticastRPCDeactivateEffect(EMagneticPillarActivationState::E_CoolTime);
	}
	else
	{
		ActivationState = EMagneticPillarActivationState::E_CoolTime;
		GetWorld()->GetTimerManager().SetTimer(
			CooldownTimer, this, &AMagneticPillar::Cooldown, CoolTime, false);
	}
}

void AMagneticPillar::Cooldown()
{
	ActivationState = EMagneticPillarActivationState::E_None;
	MulticastRPCDeactivateEffect(EMagneticPillarActivationState::E_CoolTime);
}

void AMagneticPillar::MulticastRPCActivateEffect_Implementation(EMagneticPillarActivationState type)
{
	switch(type)
	{
	case EMagneticPillarActivationState::E_1PReady:
		if(IsValid(Hit1PEffectRef))
		{
			Hit1PEffectRef->Activate();
		}
		if(HalfActivateSound && AudioComponent)
		{
			AudioComponent->SetSound(HalfActivateSound);
			AudioComponent->Play();
		}
		break;

	case EMagneticPillarActivationState::E_2PReady:
		if(IsValid(Hit2PEffectRef)) Hit2PEffectRef->Activate();
		if(HalfActivateSound && AudioComponent)
		{
			AudioComponent->SetSound(HalfActivateSound);
			AudioComponent->Play();
		}
		break;

	case EMagneticPillarActivationState::E_1PActivated:
		if(IsValid(Activation1PEffectRef)) Activation1PEffectRef->Activate();
		if(FullActivateSound && AudioComponent)
		{
			AudioComponent->SetSound(FullActivateSound);
			AudioComponent->Play();
		}
		break;

	case EMagneticPillarActivationState::E_2PActivated:
		if(IsValid(Activation2PEffectRef)) Activation2PEffectRef->Activate();
		if(FullActivateSound && AudioComponent)
		{
			AudioComponent->SetSound(FullActivateSound);
			AudioComponent->Play();
		}
		break;

	case EMagneticPillarActivationState::E_CoolTime:
		if(IsValid(CooldonwEffectRef)) CooldonwEffectRef->Activate();
		break;

	case EMagneticPillarActivationState::E_Locked:
		if(IsValid(LockedNiagara)) LockedEffectRef->Activate();
		break;
	}
}

void AMagneticPillar::MulticastRPCDeactivateEffect_Implementation(EMagneticPillarActivationState type)
{
	switch(type)
	{
	case EMagneticPillarActivationState::E_1PReady:
		if(IsValid(Hit1PEffectRef)) Hit1PEffectRef->Deactivate();
		break;

	case EMagneticPillarActivationState::E_2PReady:
		if(IsValid(Hit2PEffectRef)) Hit2PEffectRef->Deactivate();
		break;

	case EMagneticPillarActivationState::E_1PActivated:
		if(IsValid(Activation1PEffectRef)) Activation1PEffectRef->Deactivate();
		break;

	case EMagneticPillarActivationState::E_2PActivated:
		if(IsValid(Activation2PEffectRef)) Activation2PEffectRef->Deactivate();
		break;

	case EMagneticPillarActivationState::E_CoolTime:
		if(IsValid(CooldonwEffectRef)) 
		{
			CooldonwEffectRef->Deactivate();
		}
		break;

	case EMagneticPillarActivationState::E_Locked:
		if(IsValid(LockedNiagara)) LockedEffectRef->Deactivate();
		break;
	}
}

void AMagneticPillar::MulticastRPCSetIcon_Implementation(bool IsActivated)
{
	if(IsActivated)
	{
		MarkerComponent->SetType(EMapObjectType::Pillar);
	}
	else
	{
		MarkerComponent->SetType(EMapObjectType::LockedPillar);
	}
}

void AMagneticPillar::MulticastRPCBreak_Implementation()
{
	// 기존 runestone은 투명처리, 충돌 무효
	RuneStone->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RuneStone->SetVisibility(false);

	// 이펙트 삭제
	if(IsValid(Hit1PEffectRef))
	{
		Hit1PEffectRef->DestroyComponent();
	}
	if(IsValid(Hit2PEffectRef))
	{
		Hit2PEffectRef->DestroyComponent();
	}
	if(IsValid(Activation1PEffectRef))
	{
		Activation1PEffectRef->DestroyComponent();
	}
	if(IsValid(Activation2PEffectRef))
	{
		Activation2PEffectRef->DestroyComponent();
	}
	if(IsValid(CooldonwEffectRef))
	{
		CooldonwEffectRef->DestroyComponent();
	}
	if(IsValid(LockedEffectRef))
	{
		LockedEffectRef->DestroyComponent();
	}

	// 마커 삭제
	MarkerComponent->DestroyComponent();

	// 잔해 생성
	FTransform tf = RuneStone->GetComponentTransform();
	tf.SetScale3D({ 0.64,0.64,0.64 });
	AActor* frag = Cast<AActor>(GetWorld()->SpawnActorDeferred<AActor>(RunestoneFragmentClass, RuneStone->GetComponentTransform()));
	
	if(IsValid(frag))
	{
		frag->SetOwner(this);
		frag->FinishSpawning(tf);
		frag->SetLifeSpan(10);
	}
}

