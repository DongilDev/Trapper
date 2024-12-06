// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "TrapperPlayerMovementComponent.generated.h"

class AMagneticPillar;

UENUM(BlueprintType)
enum ECustomMovementMode
{
	CMOVE_None				UMETA(Hidden),
	CMOVE_MagneticMove		UMETA(DisplayName = "MagneticMove"),
	CMOVE_MAX				UMETA(Hidden),
};

UCLASS()
class TRAPPERPROJECT_API UTrapperPlayerMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

	class FSavedMove_Trapper : public FSavedMove_Character
	{
	public:
		virtual bool CanCombineWith(const FSavedMovePtr& NewMove, ACharacter* InCharacter, float MaxDelta) const override;
		virtual void Clear() override;
		virtual uint8 GetCompressedFlags() const override;
		virtual void SetMoveFor(ACharacter* C, float InDeltaTime, FVector const& NewAccel, class FNetworkPredictionData_Client_Character& ClientData) override;
		virtual void PrepMoveFor(ACharacter* C) override;

	private:
		uint8 Saved_bWantsToMagneticMove : 1;
		uint8 Saved_bWantsToMagneticCast : 1;
	};

	class FTrapperNetworkPredictionData_Client_Character : public FNetworkPredictionData_Client_Character
	{
		typedef FNetworkPredictionData_Client_Character Super;
	public:
		FTrapperNetworkPredictionData_Client_Character(const UCharacterMovementComponent& ClientMovement);
		virtual FSavedMovePtr AllocateNewMove() override;
	};

public:
	UTrapperPlayerMovementComponent();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void InitializeComponent() override;

protected:
	virtual void ProcessLanded(const FHitResult& Hit, float remainingTime, int32 Iterations) override;
	virtual void OnMovementUpdated(float DeltaSeconds, const FVector& OldLocation, const FVector& OldVelocity) override;
	virtual void UpdateCharacterStateBeforeMovement(float DeltaSeconds) override;
	virtual void PhysCustom(float deltaTime, int32 Iterations) override;
	virtual void OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode) override;

	virtual class FNetworkPredictionData_Client* GetPredictionData_Client() const override;
	virtual void UpdateFromCompressedFlags(uint8 Flags) override;

public:
	TObjectPtr<class ATrapperGameState> GameState;

	TObjectPtr<AMagneticPillar> GetTarget() { return TargetMagneticPillar; }
	FVector GetTargetLocation() const { return TargetPosition; }
	void SetTarget(TObjectPtr<AMagneticPillar> Target);
	void ClearTarget();

	bool HitTarget();

	UFUNCTION(Server, Reliable) void ServerSetTargetPos(const FVector& TargetPos);
	UFUNCTION(Client, Reliable) void ClientSetTargetPos(const FVector& TargetPos);
	UFUNCTION(NetMulticast, Reliable) void MulticastMagneticMoveEndCheck();

	bool GetMagneticMovingState() { return Safe_bWantsToMagneticMove; }
	bool IsMagneticCast() { return Safe_bWantsToMagneticCast; }

	bool CanMagneticMoving() { return bCanMagneticMoving; }

	void PreEnterMagneticMove();
	void EnterMagneticMove(EMovementMode PrevMode, ECustomMovementMode PrevCustomMode);
	void ExitMagneticMove();
	void PhysMagneticMove(float deltaTime, int32 Iterations);

	void StopMagneticMove();
private:
	uint8 Safe_bWantsToMagneticMove : 1;
	uint8 Safe_bWantsToMagneticCast : 1;

public:
	uint8 bWantsToMagneticMove : 1;
	uint8 bWantsToMagneticCast : 1;
	uint8 bCanMagneticMoving : 1;

	UPROPERTY(ReplicatedUsing = OnRep_GravityChange)
	uint8 bProxyGravityChange : 1;

	FTimerHandle MagneticMoveEndCheckHandle;

	UPROPERTY(Replicated) uint8 bCanChangeTarget : 1;

	float MagneticDistance = 5000.f;
	float TargetHitDistance = 200.f;

	float CastImpulse = 7000.f;
	float CastDelay = 0.67f;

	float JumpImpulse = 8000.f;
	float OriginalGravity = 1.f;
	float ArrivedJumpGravity = 3.f;

	TObjectPtr<AMagneticPillar> TargetMagneticPillar;
	TObjectPtr<class ATrapperPlayer> PlayerRef;

	UPROPERTY(Replicated) FVector TargetPosition;

	/// ------------------------------------

	float DefaultGravityScale;
	float SlowReductionRate;

	float InitWalkSpeed;
	float InitFlySpeed;
	uint8 bUseMagneticAfterSpeed : 1;
	float MagneticMoveAfterSpeed;

private:
	UFUNCTION() void OnRep_GravityChange();
	UFUNCTION() void BackToOriginalGravity();

	FTimerHandle MoveSpeedHandle;
	void AdjustMoveSpeed();
public:
	bool IsCustomMovementMode(ECustomMovementMode InCustomMovementMode) const;
	void LogMovement();

};
