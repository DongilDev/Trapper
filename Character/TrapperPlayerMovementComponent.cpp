// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/TrapperPlayerMovementComponent.h"
#include "Character/TrapperPlayer.h"
#include "GameElement/MagneticPillar.h"
#include "GameElement/Spline.h"
#include "Components/SplineComponent.h"
#include "GameFramework/GameStateBase.h"
#include "Framework/TrapperGameState.h"
#include "TimerManager.h"
#include "Widget/PlayerHUD.h"
#include "Net/UnrealNetwork.h"
#include "EngineUtils.h"

UTrapperPlayerMovementComponent::UTrapperPlayerMovementComponent()
	: TargetMagneticPillar(nullptr)
	, bCanMagneticMoving(false)
	, bCanChangeTarget(true)
	, bWantsToMagneticMove(false)
	, bWantsToMagneticCast(false)
{

}

void UTrapperPlayerMovementComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UTrapperPlayerMovementComponent, bCanChangeTarget);
	DOREPLIFETIME(UTrapperPlayerMovementComponent, TargetPosition);

	DOREPLIFETIME_CONDITION(UTrapperPlayerMovementComponent, bProxyGravityChange, COND_SimulatedOnly);
}

void UTrapperPlayerMovementComponent::InitializeComponent()
{
	Super::InitializeComponent();

	PlayerRef = Cast<ATrapperPlayer>(GetOwner());
}

/// =======================================================
///							OVERRIDE
/// =======================================================


void UTrapperPlayerMovementComponent::ProcessLanded(const FHitResult& Hit, float remainingTime, int32 Iterations)
{
	BackToOriginalGravity();
	bProxyGravityChange = false;
	Super::ProcessLanded(Hit, remainingTime, Iterations);
}

void UTrapperPlayerMovementComponent::OnMovementUpdated(float DeltaSeconds, const FVector& OldLocation, const FVector& OldVelocity)
{
	Super::OnMovementUpdated(DeltaSeconds, OldLocation, OldVelocity);

	Safe_bWantsToMagneticCast = bWantsToMagneticCast;
	Safe_bWantsToMagneticMove = bWantsToMagneticMove;
}

void UTrapperPlayerMovementComponent::UpdateCharacterStateBeforeMovement(float DeltaSeconds)
{
	if (Safe_bWantsToMagneticMove && Safe_bWantsToMagneticCast)
	{
		SetMovementMode(MOVE_Custom, CMOVE_MagneticMove);
	}
	else if (Safe_bWantsToMagneticCast)
	{
		PreEnterMagneticMove();
	}

	if (IsCustomMovementMode(CMOVE_MagneticMove) && !Safe_bWantsToMagneticMove && !Safe_bWantsToMagneticCast)
	{
		SetMovementMode(MOVE_Falling);
	}

	Super::UpdateCharacterStateBeforeMovement(DeltaSeconds);
}

void UTrapperPlayerMovementComponent::PhysCustom(float deltaTime, int32 Iterations)
{
	Super::PhysCustom(deltaTime, deltaTime);

	switch (CustomMovementMode)
	{
	case CMOVE_MagneticMove:
		PhysMagneticMove(deltaTime, Iterations);
		break;
	default:
		UE_LOG(LogTemp, Fatal, TEXT("Invalid Movement Mode"));
	}
}

void UTrapperPlayerMovementComponent::OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode)
{
	Super::OnMovementModeChanged(PreviousMovementMode, PreviousCustomMode);

	if (PreviousMovementMode == MOVE_Custom && PreviousCustomMode == CMOVE_MagneticMove) ExitMagneticMove();
	if (IsCustomMovementMode(CMOVE_MagneticMove)) EnterMagneticMove(PreviousMovementMode, (ECustomMovementMode)PreviousCustomMode);
}

FNetworkPredictionData_Client* UTrapperPlayerMovementComponent::GetPredictionData_Client() const
{
	check(PawnOwner != nullptr);

	if (ClientPredictionData == nullptr)
	{
		UTrapperPlayerMovementComponent* MutableThis = const_cast<UTrapperPlayerMovementComponent*>(this);

		MutableThis->ClientPredictionData = new FTrapperNetworkPredictionData_Client_Character(*this);
		MutableThis->ClientPredictionData->MaxSmoothNetUpdateDist = 92.f;
		MutableThis->ClientPredictionData->NoSmoothNetUpdateDist = 140.f;
	}

	return ClientPredictionData;
}

void UTrapperPlayerMovementComponent::UpdateFromCompressedFlags(uint8 Flags)
{
	Super::UpdateFromCompressedFlags(Flags);

	Safe_bWantsToMagneticCast = (Flags & FSavedMove_Character::FLAG_Custom_0) != 0;
	Safe_bWantsToMagneticMove = (Flags & FSavedMove_Character::FLAG_Custom_1) != 0;
}

/// =======================================================
///							Target
/// =======================================================

void UTrapperPlayerMovementComponent::SetTarget(TObjectPtr<AMagneticPillar> Target)
{
	if (!IsValid(Target) || (!PlayerRef->HasAuthority() && !bCanChangeTarget) || Target->IsLocked())
	{
		return;
	}

	bCanMagneticMoving = true;
	PlayerRef->PlayerHud->SetShiftFrameVisibility(true);

	// 타겟 포지션 계산
	TargetMagneticPillar = Target;
	TargetPosition = Target->GetActorLocation();

	UStaticMeshComponent* StaticMeshComponent = Target->FindComponentByClass<UStaticMeshComponent>();
	FVector BoxSize;
	if (StaticMeshComponent)
	{
		BoxSize = StaticMeshComponent->Bounds.GetBox().GetSize();
	}
	//UCapsuleComponent* CapsuleComponent = GetOwner()->FindComponentByClass<UCapsuleComponent>();
	TargetPosition.Z += 700.f; // + CapsuleComponent->GetScaledCapsuleHalfHeight();

	//DrawDebugSphere(GetWorld(), TargetPosition, 100.f, 16, FColor::Blue, false, 3.f);

	// 효과
	TargetMagneticPillar->SetOutline(true);
}

void UTrapperPlayerMovementComponent::ClearTarget()
{
	// 방금까지 타겟이었던 자성기둥의 아웃라인을 해제해주고 nullptr
	if (!PlayerRef->HasAuthority() && !bCanChangeTarget)
	{
		return;
	}

	if (GetTarget()) //&& !bIsMagneticMovingCast)
	{
		TargetMagneticPillar->SetOutline(false);
		TargetMagneticPillar = nullptr;
		bCanMagneticMoving = false;
		PlayerRef->PlayerHud->SetShiftFrameVisibility(false);
	}
}

bool UTrapperPlayerMovementComponent::HitTarget()
{
	FVector TraceDirection = TargetPosition - UpdatedComponent->GetComponentLocation();
	FVector Start = UpdatedComponent->GetComponentLocation();
	FVector End = Start + TraceDirection.GetSafeNormal() * TargetHitDistance;
	//DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, 3);

	FHitResult Result;
	bool HasHit = GetWorld()->LineTraceSingleByChannel(
		Result,
		Start,
		End,
		ECC_GameTraceChannel1
	);

	if (HasHit) return true;
	else return false;
}

void UTrapperPlayerMovementComponent::ServerSetTargetPos_Implementation(const FVector& TargetPos)
{
	TargetPosition = TargetPos;
	bCanChangeTarget = false;
}

void UTrapperPlayerMovementComponent::ClientSetTargetPos_Implementation(const FVector& TargetPos)
{
	TargetPosition = TargetPos;
}

void UTrapperPlayerMovementComponent::MulticastMagneticMoveEndCheck_Implementation()
{
	if (!IsValid(PlayerRef) || PlayerRef->HasAuthority()) return;

	UE_LOG(LogTemp, Warning, TEXT("Check"));

	bCanChangeTarget = true;

	if (bWantsToMagneticMove || bWantsToMagneticCast)
	{
		bWantsToMagneticMove = false;
		bWantsToMagneticCast = false;
	}

	PlayerRef->FinishMagneticMovingSetting();
}

/// =======================================================
///						 Move Logic
/// =======================================================

void UTrapperPlayerMovementComponent::PreEnterMagneticMove()
{
	PlayerRef->StartManeticMovingSetting();
	BackToOriginalGravity();
	bProxyGravityChange = false;

	// Cast Impulse
	Velocity = CastImpulse * (FVector::UpVector * 0.1f);
	SetMovementMode(MOVE_Falling);

	// Set Target RPC
	if (PlayerRef->HasAuthority() && PlayerRef->IsLocallyControlled())
	{
		ClientSetTargetPos(TargetPosition);
	}
	else if (!PlayerRef->HasAuthority() && PlayerRef->IsLocallyControlled())
	{
		ServerSetTargetPos(TargetPosition);
	}

	// Rotation in Target Direction
	FVector Direction = (TargetPosition - UpdatedComponent->GetComponentLocation()).GetSafeNormal();
	Direction.Z = 0.0f;
	FQuat Rot = Direction.ToOrientationQuat();
	FHitResult Hit;
	SafeMoveUpdatedComponent(FVector::ZeroVector, Rot, false, Hit);
}

void UTrapperPlayerMovementComponent::EnterMagneticMove(EMovementMode PrevMode, ECustomMovementMode PrevCustomMode)
{
	PlayerRef->StartManeticMovingSetting();
	BackToOriginalGravity();
	bProxyGravityChange = false;

	// Set Target RPC
	if (PlayerRef->HasAuthority() && PlayerRef->IsLocallyControlled())
	{
		ClientSetTargetPos(TargetPosition);
	}
	else if (!PlayerRef->HasAuthority() && PlayerRef->IsLocallyControlled())
	{
		ServerSetTargetPos(TargetPosition);
	}

	// Rotation in Target Direction
	FVector Direction = (TargetPosition - UpdatedComponent->GetComponentLocation()).GetSafeNormal();
	Direction.Z = 0.0f;
	FQuat Rot = Direction.ToOrientationQuat();
	FHitResult Hit;
	SafeMoveUpdatedComponent(FVector::ZeroVector, Rot, false, Hit);
}

void UTrapperPlayerMovementComponent::ExitMagneticMove()
{
	PlayerRef->FinishMagneticMovingSetting();

	// Jump After Arrived & Gravity Set
	GravityScale *= ArrivedJumpGravity;
	bProxyGravityChange = true;
	Velocity = JumpImpulse * (FVector::UpVector * 0.1f);

	// Set Movement & Flag 
	bWantsToMagneticMove = false;
	bWantsToMagneticCast = false;

	// Change to enable Target Setting
	if (PlayerRef->HasAuthority() && !PlayerRef->IsLocallyControlled())
	{
		MulticastMagneticMoveEndCheck();
	}

	// 기본 이동속도에 3초간 중첩 적용
	if (bUseMagneticAfterSpeed)
	{
		MaxWalkSpeed *= MagneticMoveAfterSpeed;

		GetWorld()->GetTimerManager().SetTimer(MoveSpeedHandle, this,
			&UTrapperPlayerMovementComponent::AdjustMoveSpeed, 1.0f, false, 3.f);
	}
}

void UTrapperPlayerMovementComponent::PhysMagneticMove(float deltaTime, int32 Iterations)
{
	if (deltaTime < MIN_TICK_TIME)
	{
		return;
	}

	bJustTeleported = false;
	float remainingTime = deltaTime;

	// Perform the move
	while ((remainingTime >= MIN_TICK_TIME) && (Iterations < MaxSimulationIterations) && CharacterOwner && (CharacterOwner->Controller || bRunPhysicsWithNoController || (CharacterOwner->GetLocalRole() == ROLE_SimulatedProxy)))
	{
		Iterations++;
		bJustTeleported = false;
		const float timeTick = GetSimulationTimeStep(remainingTime, Iterations);
		remainingTime -= timeTick;

		// Calc Velocity
		FVector CurrentLocation = UpdatedComponent->GetComponentLocation();
		FVector DirectionToTarget = (TargetPosition - CurrentLocation).GetSafeNormal();
		float DistanceToTarget = FVector::Dist(CurrentLocation, TargetPosition);
		Velocity = DirectionToTarget * MaxFlySpeed;

		FVector OldLocation = UpdatedComponent->GetComponentLocation();
		FQuat OldRotation = UpdatedComponent->GetComponentRotation().Quaternion();
		FHitResult Hit(1.f);
		FVector Adjusted = Velocity * timeTick;
		SafeMoveUpdatedComponent(Adjusted, OldRotation, true, Hit);

		if (FVector::Dist(OldLocation, TargetPosition) <= 100.f)
		{
			SetMovementMode(MOVE_Falling);
			StartNewPhysics(remainingTime, Iterations);
			return;
		}

		if (Hit.Time < 1.f)
		{
			HandleImpact(Hit, deltaTime, Adjusted);
			SlideAlongSurface(Adjusted, (1.f - Hit.Time), Hit.Normal, Hit, true);
		}

		if (!bJustTeleported && !HasAnimRootMotion() && !CurrentRootMotion.HasOverrideVelocity())
		{
			Velocity = (UpdatedComponent->GetComponentLocation() - OldLocation) / timeTick;
		}
	}

}

void UTrapperPlayerMovementComponent::StopMagneticMove()
{
	bWantsToMagneticMove = false;
	bWantsToMagneticCast = false;
}

/// =======================================================
///						 Saved Move
/// =======================================================

bool UTrapperPlayerMovementComponent::FSavedMove_Trapper::CanCombineWith(const FSavedMovePtr& NewMove, ACharacter* InCharacter, float MaxDelta) const
{
	FSavedMove_Trapper* NewTrapperMove = static_cast<FSavedMove_Trapper*>(NewMove.Get());

	if (Saved_bWantsToMagneticCast != NewTrapperMove->Saved_bWantsToMagneticCast)
	{
		return false;
	}

	if (Saved_bWantsToMagneticMove != NewTrapperMove->Saved_bWantsToMagneticMove)
	{
		return false;
	}

	return FSavedMove_Character::CanCombineWith(NewMove, InCharacter, MaxDelta);
}

void UTrapperPlayerMovementComponent::FSavedMove_Trapper::Clear()
{
	FSavedMove_Character::Clear();

	Saved_bWantsToMagneticCast = 0;
	Saved_bWantsToMagneticMove = 0;
}

uint8 UTrapperPlayerMovementComponent::FSavedMove_Trapper::GetCompressedFlags() const
{
	uint8 Result = FSavedMove_Character::GetCompressedFlags();

	if (Saved_bWantsToMagneticCast)
	{
		Result |= FLAG_Custom_0;
	}

	if (Saved_bWantsToMagneticMove)
	{
		Result |= FLAG_Custom_1;
	}

	return Result;
}

void UTrapperPlayerMovementComponent::FSavedMove_Trapper::SetMoveFor(ACharacter* C, float InDeltaTime, FVector const& NewAccel, FNetworkPredictionData_Client_Character& ClientData)
{
	FSavedMove_Character::SetMoveFor(C, InDeltaTime, NewAccel, ClientData);

	UTrapperPlayerMovementComponent* CharacterMovement = Cast<UTrapperPlayerMovementComponent>(C->GetCharacterMovement());
	Saved_bWantsToMagneticCast = CharacterMovement->Safe_bWantsToMagneticCast;
	Saved_bWantsToMagneticMove = CharacterMovement->Safe_bWantsToMagneticMove;
}

void UTrapperPlayerMovementComponent::FSavedMove_Trapper::PrepMoveFor(ACharacter* C)
{
	FSavedMove_Character::PrepMoveFor(C);

	UTrapperPlayerMovementComponent* CharacterMovement = Cast<UTrapperPlayerMovementComponent>(C->GetCharacterMovement());
	CharacterMovement->Safe_bWantsToMagneticCast = Saved_bWantsToMagneticCast;
	CharacterMovement->Safe_bWantsToMagneticMove = Saved_bWantsToMagneticMove;
}

UTrapperPlayerMovementComponent::FTrapperNetworkPredictionData_Client_Character::FTrapperNetworkPredictionData_Client_Character(const UCharacterMovementComponent& ClientMovement)
	: Super(ClientMovement)
{
}

FSavedMovePtr UTrapperPlayerMovementComponent::FTrapperNetworkPredictionData_Client_Character::AllocateNewMove()
{
	return FSavedMovePtr(new FSavedMove_Trapper());
}

/// =======================================================
///						 Helper
/// =======================================================

void UTrapperPlayerMovementComponent::OnRep_GravityChange()
{
	if (bProxyGravityChange)
	{
		GravityScale *= ArrivedJumpGravity;
		//UE_LOG(LogTemp, Warning, TEXT("Apply Jump Gravity"));
	}
	else
	{
		BackToOriginalGravity();
		//UE_LOG(LogTemp, Warning, TEXT("Back to Original Jump Gravity"));
	}
}

void UTrapperPlayerMovementComponent::BackToOriginalGravity()
{
	GravityScale = OriginalGravity;
}

void UTrapperPlayerMovementComponent::AdjustMoveSpeed()
{
	PlayerRef->AdjustMoveSpeed();
}

bool UTrapperPlayerMovementComponent::IsCustomMovementMode(ECustomMovementMode InCustomMovementMode) const
{
	return MovementMode == MOVE_Custom && CustomMovementMode == InCustomMovementMode;
}

void UTrapperPlayerMovementComponent::LogMovement()
{
	FString MovementModeString;
	switch (MovementMode)
	{
	case MOVE_None: MovementModeString = TEXT("None"); break;
	case MOVE_Walking: MovementModeString = TEXT("Walking"); break;
	case MOVE_NavWalking: MovementModeString = TEXT("NavWalking"); break;
	case MOVE_Falling: MovementModeString = TEXT("Falling"); break;
	case MOVE_Swimming: MovementModeString = TEXT("Swimming"); break;
	case MOVE_Flying: MovementModeString = TEXT("Flying"); break;
	case MOVE_Custom: MovementModeString = FString::Printf(TEXT("Custom(%d)"), CustomMovementMode); break;
	default: MovementModeString = TEXT("Unknown"); break;
	}

	UE_LOG(LogTemp, Warning, TEXT("Movement Mode: %s"), *MovementModeString);
}