// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster/CDC.h"

#include "Animation/AnimMontage.h" 
#include "Components/CapsuleComponent.h"
#include "Engine/DamageEvents.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "Framework/TrapperGameState.h"
#include "Monster/Weapon/Sword.h"
#include "Data/MonsterDataTables.h"

ACDC::ACDC()
{
	DeadbodyHeadSpawnPoint = CreateDefaultSubobject<USceneComponent>(TEXT("HeadSpawn"));
	DeadbodyHeadSpawnPoint->SetupAttachment(RootComponent);

	DeadbodySpineSpawnPoint = CreateDefaultSubobject<USceneComponent>(TEXT("SpineSpawnPoint"));
	DeadbodySpineSpawnPoint->SetupAttachment(RootComponent);

	DeadbodyLungSpawnPoint = CreateDefaultSubobject<USceneComponent>(TEXT("LungSpawnPoint"));
	DeadbodyLungSpawnPoint->SetupAttachment(RootComponent);

	DeadbodyHipSpawnPoint = CreateDefaultSubobject<USceneComponent>(TEXT("HipSpawnPoint"));
	DeadbodyHipSpawnPoint->SetupAttachment(RootComponent);

	DeadbodyRightArmSpawnPoint = CreateDefaultSubobject<USceneComponent>(TEXT("RightArmSpawnPoint"));
	DeadbodyRightArmSpawnPoint->SetupAttachment(RootComponent);

	DeadbodyLeftArmSpawnPoint = CreateDefaultSubobject<USceneComponent>(TEXT("LeftArmSpawnPoint"));
	DeadbodyLeftArmSpawnPoint->SetupAttachment(RootComponent);

	DeadbodyRightHandSpawnPoint = CreateDefaultSubobject<USceneComponent>(TEXT("RightHandSpawnPoint"));
	DeadbodyRightHandSpawnPoint->SetupAttachment(RootComponent);

	DeadbodyLeftHandSpawnPoint = CreateDefaultSubobject<USceneComponent>(TEXT("LeftHandSpawnPoint"));
	DeadbodyLeftHandSpawnPoint->SetupAttachment(RootComponent);

	DeadbodyLeftThighSpawnPoint = CreateDefaultSubobject<USceneComponent>(TEXT("LeftThighSpawnPoint"));
	DeadbodyLeftThighSpawnPoint->SetupAttachment(RootComponent);

	DeadbodyRightLegSpawnPoint = CreateDefaultSubobject<USceneComponent>(TEXT("RightLegSpawnPoint"));
	DeadbodyRightLegSpawnPoint->SetupAttachment(RootComponent);

	DeadbodyLeftLegSpawnPoint = CreateDefaultSubobject<USceneComponent>(TEXT("LeftLegSpawnPoint"));
	DeadbodyLeftLegSpawnPoint->SetupAttachment(RootComponent);


}

void ACDC::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		Sword = GetWorld()->SpawnActor<ASword>(SwordClass);
		Sword->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, TEXT("Sword"));
		Sword->SetOwner(this);
	}

	///SetData
	//HP
	if (MonsterInfoTable[0].HP)
	{
		if (!this->ActorHasTag("Tutorial"))
		{
			MaxHP = MonsterInfoTable[0].HP;
			CurrentHP = MaxHP;
		}
		
	}
	//Attack Damage
	if (MonsterInfoTable[0].Attack)
	{
		AttackDamage = MonsterInfoTable[0].Attack;
	}
	//Speed
	if (MonsterInfoTable[0].MoveSpeed)
	{
		MonsterSpeed = MonsterInfoTable[0].MoveSpeed;
		GetCharacterMovement()->MaxWalkSpeed = MonsterSpeed;
	}
	if (MonsterInfoTable[0].ChaseSpeed)
	{
		MonsterDetectSpeed = MonsterInfoTable[0].ChaseSpeed;
	}
	if (MonsterInfoTable[0].ChaseSpeed)
	{
		MonsterEffectSpeed = MonsterInfoTable[0].EffectSpeed;
	}
	//DetectTime
	if(MonsterInfoTable[0].LocationTimer)
	{
		DetectTime = MonsterInfoTable[0].LocationTimer;
	}
	//Item
	if (MonsterInfoTable[0].SpawnAmount)
	{
		ItemMinDropNumber = MonsterInfoTable[0].SpawnAmount - 1;
		ItemMaxDropNumber = MonsterInfoTable[0].SpawnAmount;
	}
	//Debuffe Timer
	if (MonsterInfoTable[0].SpawnAmount)
	{
		DebuffeTime = MonsterInfoTable[0].EffectTimer * 10;
	}
	//Pillar Speed
	if (MonsterInfoTable[0].PillarSpeed)
	{
		MonsterPillarSpeed = MonsterInfoTable[0].PillarSpeed;
	}
	//Pillar StunTime
	if (MonsterInfoTable[0].PillarStunTime)
	{
		MonsterPillarTime = MonsterInfoTable[0].PillarStunTime;
		//UE_LOG(LogTemp, Warning, TEXT("MonsterPillrTime = %f"), MonsterInfoTable[0].PillarStunTime);
	}
}

void ACDC::AttackHitCheck()
{
	//UE_LOG(LogTemp, Warning, TEXT("AttaackHitCheck"));

	if (!HasAuthority())
	{
		return;
	}

	if (IsValid(Sword))
	{
		//UE_LOG(LogTemp, Warning, TEXT("Sword Valid"));
		FHitResult OutHitResult;
		FCollisionQueryParams Params(SCENE_QUERY_STAT(Attack), false, this);
		const FVector Start = Sword->SwordMesh->GetSocketLocation("Start");
		const FVector End = Sword->SwordMesh->GetSocketLocation("End");

		//UE_LOG(LogTemp, Warning, TEXT("AttackHitCheck?"));

		bool HitDetected = GetWorld()->SweepSingleByChannel
		(
			OutHitResult,
			Start,
			End,
			FQuat::Identity,
			ECC_GameTraceChannel4,
			Sword->SwordMesh->GetCollisionShape(),
			Params
		);

		//UE_LOG(LogTemp, Warning, TEXT("Hit results : %s"), *OutHitResult.GetActor()->GetName());

		if (HitDetected)
		{
			if (!OutHitResult.GetActor()->ActorHasTag("Monster"))
			{
				//UE_LOG(LogTemp, Warning, TEXT("OutHitResult : %s"), *OutHitResult.GetActor()->GetName());
				FDamageEvent DamageEvent;
				OutHitResult.GetActor()->TakeDamage(AttackDamage, DamageEvent, nullptr, Sword);
				ServerRPCAttackHitSoundPlay();
			}
		
		}
	} 

//#if ENABLE_DRAW_DEBUG
//	FVector CapsuleOrigin = Start + (End - Start) * 0.5f;
//	float CapsuleHeight = AttackRange * 0.5f;
//	FColor DrawColor = HitDetected ? FColor::Red : FColor::Green;
//
//	DrawDebugBox(GetWorld(), CapsuleOrigin, CapsuleHeight / 2, AttackRadius, FRotationMatrix::MakeFromZ(GetActorForwardVector()).ToQuat(), DrawColor, false, 5.0f);
//#endif
}

float ACDC::TakeDamage(float DamageAmout, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
	Super::TakeDamage(DamageAmout, DamageEvent, EventInstigator, DamageCauser);
	//UE_LOG(LogTemp, Warning, TEXT("Damage Causer : %s"), *DamageCauser->GetName());

	return 0.0f;
}

void ACDC::AttackCDCMontage()
{
	ServerRPCAttack();
}

void ACDC::ChangeCDCDamage()
{
	float DamageVairiation = TrapperGameState->MonsterAttackDamage;

	AttackDamage = AttackDamage - (AttackDamage * DamageVairiation);
}

void ACDC::CDCDeadByTrap()
{
	if (IsValid(DeadbodyHead) && IsValid(DeadbodySpine) && IsValid(DeadbodyLung)
		&& IsValid(DeadbodyHip) && IsValid(DeadbodyRightArm) && IsValid(DeadbodyLeftArm)
		&& IsValid(DeadbodyRightHand) && IsValid(DeadbodyLeftHand) && IsValid(DeadbodyLeftThigh)
		&& IsValid(DeadbodyRightLeg) && IsValid(DeadbodyLeftLeg))
	{
		FVector HeadSpawnLocation = DeadbodyHeadSpawnPoint->GetComponentLocation();
		FRotator HeadSpawnRotation = DeadbodyHeadSpawnPoint->GetComponentRotation();

		FVector SpineSpawnLocation = DeadbodySpineSpawnPoint->GetComponentLocation();
		FRotator SpineSpawnRotation = DeadbodySpineSpawnPoint->GetComponentRotation();

		FVector LungSpawnLocation = DeadbodyLungSpawnPoint->GetComponentLocation();
		FRotator LungSpawnRotation = DeadbodyLungSpawnPoint->GetComponentRotation();

		FVector HipSpawnLocation = DeadbodyHipSpawnPoint->GetComponentLocation();
		FRotator HipSpawnRotation = DeadbodyHipSpawnPoint->GetComponentRotation();

		FVector RightArmSpawnLocation = DeadbodyRightArmSpawnPoint->GetComponentLocation();
		FRotator RightArmSpawnRotation = DeadbodyRightArmSpawnPoint->GetComponentRotation();

		FVector LeftArmSpawnLocation = DeadbodyLeftArmSpawnPoint->GetComponentLocation();
		FRotator LeftArmSpawnRotation = DeadbodyLeftArmSpawnPoint->GetComponentRotation();

		FVector RightHandSpawnLocation = DeadbodyRightHandSpawnPoint->GetComponentLocation();
		FRotator RightHandSpawnRotation = DeadbodyRightHandSpawnPoint->GetComponentRotation();

		FVector LeftHandSpawnLocation = DeadbodyLeftHandSpawnPoint->GetComponentLocation();
		FRotator LeftHandSpawnRotation = DeadbodyLeftHandSpawnPoint->GetComponentRotation();

		FVector LeftThighSpawnLocation = DeadbodyLeftThighSpawnPoint->GetComponentLocation();
		FRotator LeftThighSpawnRotation = DeadbodyLeftThighSpawnPoint->GetComponentRotation();

		FVector RightLegSpawnLocation = DeadbodyRightLegSpawnPoint->GetComponentLocation();
		FRotator RightLegSpawnRotation = DeadbodyRightLegSpawnPoint->GetComponentRotation();

		FVector LeftLegSpawnLocation = DeadbodyLeftLegSpawnPoint->GetComponentLocation();
		FRotator LeftLegSpawnRotation = DeadbodyLeftLegSpawnPoint->GetComponentRotation();

		TObjectPtr<ADeadbodyParts> HeadPart = GetWorld()->SpawnActor<ADeadbodyParts>(DeadbodyHead, HeadSpawnLocation, HeadSpawnRotation);
		TObjectPtr<ADeadbodyParts> SpinePart = GetWorld()->SpawnActor<ADeadbodyParts>(DeadbodySpine, SpineSpawnLocation, SpineSpawnRotation);
		TObjectPtr<ADeadbodyParts> LungPart = GetWorld()->SpawnActor<ADeadbodyParts>(DeadbodyLung, LungSpawnLocation, LungSpawnRotation);
		TObjectPtr<ADeadbodyParts> HipPart = GetWorld()->SpawnActor<ADeadbodyParts>(DeadbodyHip, HipSpawnLocation, HipSpawnRotation);
		TObjectPtr<ADeadbodyParts> RightArmPart = GetWorld()->SpawnActor<ADeadbodyParts>(DeadbodyRightArm, RightArmSpawnLocation, RightArmSpawnRotation);
		TObjectPtr<ADeadbodyParts> LeftArmPart = GetWorld()->SpawnActor<ADeadbodyParts>(DeadbodyLeftArm, LeftArmSpawnLocation, LeftArmSpawnRotation);
		TObjectPtr<ADeadbodyParts> RightHandPart = GetWorld()->SpawnActor<ADeadbodyParts>(DeadbodyRightHand, RightHandSpawnLocation, RightHandSpawnRotation);
		TObjectPtr<ADeadbodyParts> LeftHandPart = GetWorld()->SpawnActor<ADeadbodyParts>(DeadbodyLeftHand, LeftHandSpawnLocation, LeftHandSpawnRotation);
		TObjectPtr<ADeadbodyParts> LeftThighPart = GetWorld()->SpawnActor<ADeadbodyParts>(DeadbodyLeftThigh, LeftThighSpawnLocation, LeftThighSpawnRotation);
		TObjectPtr<ADeadbodyParts> RightLegPart = GetWorld()->SpawnActor<ADeadbodyParts>(DeadbodyRightLeg, RightLegSpawnLocation, RightLegSpawnRotation);
		TObjectPtr<ADeadbodyParts> LeftLegPart = GetWorld()->SpawnActor<ADeadbodyParts>(DeadbodyLeftLeg, LeftLegSpawnLocation, LeftLegSpawnRotation);

	}
}

void ACDC::ServerRPCAttackHitSoundPlay_Implementation()
{
	MulticastRPCAttackHitSoundPlay();
}

void ACDC::MulticastRPCAttackHitSoundPlay_Implementation()
{
	if (CDCAttackHitSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, CDCAttackHitSound, GetActorLocation());
	}
}

void ACDC::ServerRPCAttack_Implementation()
{
	MulticastRPCAttack();
}

void ACDC::MulticastRPCAttack_Implementation()
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

