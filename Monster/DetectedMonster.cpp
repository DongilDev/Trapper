// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster/DetectedMonster.h"

#include "Animation/AnimMontage.h"
#include "Components/CapsuleComponent.h"
#include "Components/BoxComponent.h"
#include "Engine/DamageEvents.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "Framework/TrapperGameState.h"
#include "Data/MonsterDataTables.h"
#include "Monster/DeadbodyParts.h"

ADetectedMonster::ADetectedMonster()
{
	AttackTriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("AttackTriggerBox"));
	AttackTriggerBox->SetupAttachment(RootComponent);

	DeadbodyHeadSpawnPoint = CreateDefaultSubobject<USceneComponent>(TEXT("HeadSpawn"));
	DeadbodyHeadSpawnPoint->SetupAttachment(RootComponent);

	DeadbodyThoraxSpawnPoint = CreateDefaultSubobject<USceneComponent>(TEXT("ThoraxSpawn"));
	DeadbodyThoraxSpawnPoint->SetupAttachment(RootComponent);

	DeadbodyLegSpawnPoint = CreateDefaultSubobject<USceneComponent>(TEXT("LegSpawn"));
	DeadbodyLegSpawnPoint->SetupAttachment(RootComponent);
}

void ADetectedMonster::BeginPlay()
{
	Super::BeginPlay();

	///SetData
	//HP
	if (MonsterInfoTable[2].HP)
	{
		MaxHP = MonsterInfoTable[2].HP;
		CurrentHP = MaxHP;
	}
	//Attack Damage
	if (MonsterInfoTable[2].Attack)
	{
		AttackDamage = MonsterInfoTable[2].Attack;
	}
	//Speed
	if (MonsterInfoTable[2].MoveSpeed)
	{
		MonsterSpeed = MonsterInfoTable[2].MoveSpeed;
		GetCharacterMovement()->MaxWalkSpeed = MonsterSpeed;
	}
	if (MonsterInfoTable[2].ChaseSpeed)
	{
		MonsterDetectSpeed = MonsterInfoTable[2].ChaseSpeed;
	}
	if (MonsterInfoTable[2].ChaseSpeed)
	{
		MonsterEffectSpeed = MonsterInfoTable[2].EffectSpeed;
	}
	//DetectTime
	if (MonsterInfoTable[2].LocationTimer)
	{
		DetectTime = MonsterInfoTable[2].LocationTimer;
	}
	//Item
	if (MonsterInfoTable[2].SpawnAmount)
	{
		ItemMinDropNumber = MonsterInfoTable[2].SpawnAmount - 1;
		ItemMaxDropNumber = MonsterInfoTable[2].SpawnAmount;
	}
	//Debuffe Timer
	if (MonsterInfoTable[2].SpawnAmount)
	{
		DebuffeTime = MonsterInfoTable[2].EffectTimer * 10;
	}
	//Pillar Speed
	if (MonsterInfoTable[2].PillarSpeed)
	{
		MonsterPillarSpeed = MonsterInfoTable[2].PillarSpeed;
	}
	//Pillar StunTime
	if (MonsterInfoTable[2].PillarStunTime)
	{
		MonsterPillarTime = MonsterInfoTable[2].PillarStunTime;
	}
}

void ADetectedMonster::AttackHitCheck()
{
	if (!HasAuthority())
	{
		return;
	}
	FHitResult HitResult;
	FCollisionQueryParams Params(SCENE_QUERY_STAT(Attack), false, this);
	
	float AttackRange = 300.f;
	float AttackRadius = 80.f;

	bool bResult = GetWorld()->SweepSingleByChannel(
		HitResult,
		GetActorLocation(),
		GetActorLocation() + GetActorForwardVector() * AttackRange,
		FQuat::Identity,
		ECC_GameTraceChannel4,
		AttackTriggerBox->GetCollisionShape(),
		Params);

//#if ENABLE_DRAW_DEBUG
//	FVector TraceVec = GetActorForwardVector() * AttackRange;
//	FVector Center = GetActorLocation() + TraceVec * 0.5f;
//	float HalfHeight = AttackRange * 0.5f + AttackRadius;
//	FQuat CapsuleRot = FRotationMatrix::MakeFromZ(TraceVec).ToQuat();
//	FColor DrawColor = bResult ? FColor::Green : FColor::Red;
//	float DebugLifeTime = 5.0f;
//
//	DrawDebugCapsule(GetWorld(),
//		Center,
//		HalfHeight,
//		AttackRadius,
//		CapsuleRot,
//		DrawColor,
//		false,
//		DebugLifeTime);
//
//#endif

	if (bResult)
	{
		if (!HitResult.GetActor()->ActorHasTag("Monster"))
		{
			FDamageEvent DamageEvent;
			HitResult.GetActor()->TakeDamage(50.0f, DamageEvent, nullptr, this);
			ServerRPCAttackHitSoundPlay();
		}
		
	}
}

void ADetectedMonster::DetectedDeadByTrap()
{
	if (IsValid(DeadbodyHead) && IsValid(DeadbodyThorax) && IsValid(DeadbodyLeg))
	{
		FVector HeadSpawnLocation = DeadbodyHeadSpawnPoint->GetComponentLocation();
		FRotator HeadSpawnRotation = DeadbodyHeadSpawnPoint->GetComponentRotation();

		FVector ThoraxSpawnLocation = DeadbodyThoraxSpawnPoint->GetComponentLocation();
		FRotator ThoraxSpawnRotation = DeadbodyThoraxSpawnPoint->GetComponentRotation();

		FVector LegSpawnLocation = DeadbodyLegSpawnPoint->GetComponentLocation();
		FRotator LegSpawnRotation = DeadbodyLegSpawnPoint->GetComponentRotation();

		TObjectPtr<ADeadbodyParts> HeadPart = GetWorld()->SpawnActor<ADeadbodyParts>(DeadbodyHead, HeadSpawnLocation, HeadSpawnRotation);
		TObjectPtr<ADeadbodyParts> ThoraxPart = GetWorld()->SpawnActor<ADeadbodyParts>(DeadbodyThorax, ThoraxSpawnLocation, ThoraxSpawnRotation);
		TObjectPtr<ADeadbodyParts> LegPart = GetWorld()->SpawnActor<ADeadbodyParts>(DeadbodyLeg, LegSpawnLocation, LegSpawnRotation);
		
	}
}

void ADetectedMonster::AttackDetectedMontage()
{
	ServerRPCAttack();
}

void ADetectedMonster::ChangeDetectedDamage()
{
	float DamageVairiation = TrapperGameState->MonsterAttackDamage;

	AttackDamage = AttackDamage - (AttackDamage * DamageVairiation);
}

void ADetectedMonster::ServerRPCAttackHitSoundPlay_Implementation()
{
	MulticastRPCAttackHitSoundPlay();
}

void ADetectedMonster::MulticastRPCAttackHitSoundPlay_Implementation()
{
	if (DetectAttackHitSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, DetectAttackHitSound, GetActorLocation());
	}
}

void ADetectedMonster::ServerRPCAttack_Implementation()
{
	MulticastRPCAttack();
}

void ADetectedMonster::MulticastRPCAttack_Implementation()
{
	float RandomNum = FMath::FRandRange(1.0, 3.0);

	if (!IsValid(AnimInstance))
	{
		return;
	}

	if (RandomNum < 2)
	{
		if (IsValid(AttackkActionMontage1))
		{
			AnimInstance->Montage_Play(AttackkActionMontage1);
		}
		
	}

	else
	{
		if (IsValid(AttackkActionMontage2))
		{
			AnimInstance->Montage_Play(AttackkActionMontage2);
		}	
	}
}
