// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster/Boss/Boss_Phase1.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/BoxComponent.h"
#include "Components/WidgetComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "EngineUtils.h"

#include "Monster/Weapon/BossChair.h"
#include "Monster/Weapon/BossSword.h"
#include "Monster/Weapon/BossThrowSword.h"
#include "Data/MonsterDataTables.h"
#include "Trap/TrapBase.h"
#include "GameElement/MagneticPillar.h"
#include "Monster/Boss/BossItem.h"
#include "Widget/BossHealthBar.h"
#include "Character/TrapperPlayer.h"
#include "GameElement/Arch.h"


ABoss_Phase1::ABoss_Phase1()
	: TrapSkillCoolTime(10)
	, ADCAttackCoolTime(10)
	, bIsTrapAttackState(true)
	, bIsADCAttackState(true)
{
	SwordSpawn = CreateDefaultSubobject<USceneComponent>(TEXT("Sword Spawn"));
	SwordSpawn->SetupAttachment(RootComponent);

	TrapAttackSight = CreateDefaultSubobject<UBoxComponent>(TEXT("TrapAttackSight"));
	TrapAttackSight->SetupAttachment(RootComponent);

	ChairSpawn = CreateDefaultSubobject<USceneComponent>(TEXT("Chair Spawn"));
	ChairSpawn->SetupAttachment(RootComponent);

	SwordItemSpawn = CreateDefaultSubobject<USceneComponent>(TEXT("SwordItem Spawn"));
	SwordItemSpawn->SetupAttachment(RootComponent);

	MoveEffectPosition = CreateDefaultSubobject<USceneComponent>(TEXT("MoveEffectPosition"));
	MoveEffectPosition->SetupAttachment(RootComponent);

	HitEffectPosition = CreateDefaultSubobject<USceneComponent>(TEXT("HitEffectPosition"));
	HitEffectPosition->SetupAttachment(RootComponent);

	//Add Dynamic
	TrapAttackSight->OnComponentBeginOverlap.AddDynamic(this, &ABoss_Phase1::OnOverlapBeginTrapSight);
	TrapAttackSight->OnComponentEndOverlap.AddDynamic(this, &ABoss_Phase1::OnOverlapEndTrapSight);
}

void ABoss_Phase1::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		BossChair = GetWorld()->SpawnActor<ABossChair>(BossChairClass);
		BossChair->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, TEXT("Chair"));
		BossChair->SetOwner(this);

		BossSword = GetWorld()->SpawnActor<ABossSword>(BossSwordClass);
		BossSword->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, TEXT("Sword"));
		BossSword->SetOwner(this);
		BossSword->SetActorHiddenInGame(true);
	}

	//Effect
	if (IsValid(MoveEffect))
	{
		UNiagaraFunctionLibrary::SpawnSystemAttached(
			MoveEffect,
			MoveEffectPosition,
			NAME_None,
			FVector(0.f, 0.f, 0.f),
			FRotator::ZeroRotator,
			EAttachLocation::KeepRelativeOffset,
			true
		);
	}

	MoveEffectPosition->SetVisibility(true, true);

	if (IsValid(HitEffect))
	{
		UNiagaraFunctionLibrary::SpawnSystemAttached(
			HitEffect,
			HitEffectPosition,
			NAME_None,
			FVector(0.f, 0.f, 0.f),
			FRotator::ZeroRotator,
			EAttachLocation::KeepRelativeOffset,
			true
		);
	}

	HitEffectPosition->SetVisibility(false, true);

	TrapperGameMode = Cast<ATrapperGameMode>(UGameplayStatics::GetGameMode(GetWorld()));

	///SetData
	//HP
	if (MonsterInfoTable[4].HP)
	{
		MaxHP = MonsterInfoTable[4].HP;
		CurrentHP = MaxHP;
	}
	
	//Speed
	if (MonsterInfoTable[4].MoveSpeed)
	{
		MonsterSpeed = MonsterInfoTable[4].MoveSpeed;
		GetCharacterMovement()->MaxWalkSpeed = MonsterSpeed;
	}
	if (MonsterInfoTable[4].ChaseSpeed)
	{
		MonsterDetectSpeed = MonsterInfoTable[4].ChaseSpeed;
	}
	if (MonsterInfoTable[4].ChaseSpeed) 
	{
		MonsterEffectSpeed = MonsterInfoTable[4].EffectSpeed;
	}

	if (MonsterInfoTable[4].Attack)
	{
		TrapAttackDamage = MonsterInfoTable[4].Attack;
	}

	//DetectTime
	if (MonsterInfoTable[4].LocationTimer)
	{
		DetectTime = MonsterInfoTable[4].LocationTimer;
	}
	//Item
	if (MonsterInfoTable[4].SpawnAmount)
	{
		ItemMinDropNumber = MonsterInfoTable[4].SpawnAmount - 1;
		ItemMaxDropNumber = MonsterInfoTable[4].SpawnAmount;
	}

	//TrapSkillCoolTime
	if (MonsterInfoTable[4].TrapCoolTime)
	{
		TrapSkillCoolTime = MonsterInfoTable[4].TrapCoolTime;
	}

	//ADCSkillCoolTime
	if (MonsterInfoTable[4].ADCCoolTime)
	{
		ADCAttackCoolTime = MonsterInfoTable[4].ADCCoolTime;
		UE_LOG(LogTemp, Warning, TEXT("ADCCoolTime : %f"), ADCAttackCoolTime);
	}

	//Pillar Speed
	if (MonsterInfoTable[4].PillarSpeed)
	{
		MonsterPillarSpeed = MonsterInfoTable[4].PillarSpeed;
	}
	//Pillar StunTime
	if (MonsterInfoTable[4].PillarStunTime)
	{
		MonsterPillarTime = MonsterInfoTable[4].PillarStunTime;
	}

	bIsTrapAttackState = true;
	bIsADCAttackState = true;

	if (HasAuthority())
	{
		for (TActorIterator<AActor> MyActor(this->GetWorld()); MyActor; ++MyActor)
		{
			if (MyActor->ActorHasTag("Arch0"))
			{
				//AActor CurrentActor = MyActor;
				Boss1Arch0 = Cast<AArch>(*MyActor);
				UE_LOG(LogTemp, Warning, TEXT("Find Arch0"));
			}

			if (MyActor->ActorHasTag("Arch1"))
			{
				//AActor* CurrentActor = *MyActor;
				Boss1Arch1 = Cast<AArch>(*MyActor);
				UE_LOG(LogTemp, Warning, TEXT("Find Arch1"));
			}
		}
	}

}

float ABoss_Phase1::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	if (DamageCauser->ActorHasTag("Trap"))
	{
		ServerRPCHitEffect();
	}

	return 0.0f;
}

void ABoss_Phase1::SpawnSword()
{
	if (!HasAuthority())
	{
		return;
	}

	FVector SpawnLocation = SwordSpawn->GetComponentLocation();
	FRotator SpawnRotation = SwordSpawn->GetComponentRotation();

	auto BossADCAttack = GetWorld()->SpawnActor<ABossThrowSword>(ThrowSword, SpawnLocation, SpawnRotation);
	BossADCAttack->TargetCharacter = Cast<ACharacter>(DetectPlayer);
	
	//Attack Damage
	if (MonsterInfoTable[4].Attack)
	{
		BossADCAttack->Damage = MonsterInfoTable[4].Attack;
	}
	
	BossADCAttack->SetOwner(this);
}

void ABoss_Phase1::SpawnChairItem()    
{
	if (!HasAuthority())
	{
		return;
	}

	FVector SpawnLocation = ChairSpawn->GetComponentLocation();
	FRotator SpawnRotation = ChairSpawn->GetComponentRotation();

	TObjectPtr<ABossItem> BossChairItem = GetWorld()->SpawnActor<ABossItem>(ChairItem, SpawnLocation, SpawnRotation);
}

void ABoss_Phase1::SpawnSwordItem()
{
	if (!HasAuthority())
	{
		return;
	}

	FVector SpawnLocation = SwordItemSpawn->GetComponentLocation();
	FRotator SpawnRotation = SwordItemSpawn->GetComponentRotation();

	TObjectPtr<ABossItem> BossSwordItem = GetWorld()->SpawnActor<ABossItem>(SwordItem, SpawnLocation, SpawnRotation);
}



void ABoss_Phase1::OnOverlapBeginTrapSight(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	
	if (!HasAuthority())
	{
		return;
	}
	//UE_LOG(LogTemp, Warning, TEXT("Trap Attack Active"));

	if (MonsterState != EMonsterStateType::IsPillarActived &&
		MonsterState != EMonsterStateType::IsStun &&
		MonsterState != EMonsterStateType::IsDead &&
		MonsterState != EMonsterStateType::IsAttackStart)
	{
		if (OtherActor->ActorHasTag("BossAttackableTrap"))
		{
			TObjectPtr<ATrapBase> DetectTrap = Cast<ATrapBase>(OtherActor);

			if (IsValid(DetectTrap))
			{
				if (DetectTrap->IsWeckage() == false)
				{
					if (!DetectTrapList.Contains(DetectTrap))
					{

						DetectTrapList.Add(DetectTrap);
						UE_LOG(LogTemp, Warning, TEXT("Add Trap : %s"), *DetectTrap->GetName());
					}

					if (bIsTrapAttackState == true)
					{
						bIsAttackEnd = false;
						bIsTrapAttackState = false;
						MonsterState = EMonsterStateType::IsTrapAttack;
						UE_LOG(LogTemp, Warning, TEXT("TrapCheck"));
					}
				}
			}
		}

		if (OtherActor->ActorHasTag("MagneticPillar"))
		{
			TObjectPtr<AMagneticPillar> DetectPillar = Cast<AMagneticPillar>(OtherActor);
			if (IsValid(DetectPillar))
			{
				if (!DetectMagneticList.Contains(DetectPillar))
				{
					DetectMagneticList.Add(DetectPillar);
				}
				if (bIsTrapAttackState == true)
				{
					bIsAttackEnd = false;
					bIsTrapAttackState = false;
					MonsterState = EMonsterStateType::IsTrapAttack;
					//UE_LOG(LogTemp, Warning, TEXT("TrapCheck"));
				}

			}
		}

		if (OtherActor->ActorHasTag("Player"))
		{
			if (Cast<ATrapperPlayer>(OtherActor)->HasAuthority())
			{
				DamagedPlayer1P = Cast<ATrapperPlayer>(OtherActor);
			}
			else
			{
				DamagedPlayer2P = Cast<ATrapperPlayer>(OtherActor);
			}
			
		}

	}
}

void ABoss_Phase1::OnOverlapEndTrapSight(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (!HasAuthority())
	{
		return;
	}

	//UE_LOG(LogTemp, Warning, TEXT("Work?"));
	if (OtherActor->ActorHasTag("BossAttackableTrap"))
	{
		TObjectPtr<ATrapBase> DetectTrap = Cast<ATrapBase>(OtherActor);

		if (IsValid(DetectTrap))
		{
			if (bSafeRanged)
			{
				//UE_LOG(LogTemp, Warning, TEXT("bSafeRanged true good"));
			}
			else
			{
				DetectTrapList.Remove(DetectTrap);
			}
			//UE_LOG(LogTemp, Warning, TEXT("Remove Trap : %s"), *DetectTrap->GetName());
		}
	}

	if (OtherActor->ActorHasTag("MagneticPillar"))
	{
		TObjectPtr<AMagneticPillar> DetectPillar = Cast<AMagneticPillar>(OtherActor);

		if (IsValid(DetectPillar))
		{
			if(bSafeRanged)
			{
				//UE_LOG(LogTemp, Warning, TEXT("bSafeRanged true good"));
			}
			else
			{
				DetectMagneticList.Remove(DetectPillar);
			}
			
		}
	}

	if (OtherActor->ActorHasTag("Player"))
	{
		if (bSafeRanged)
		{
			//UE_LOG(LogTemp, Warning, TEXT("bSafeRanged true good"));
		}
		else
		{
			if (Cast<ATrapperPlayer>(OtherActor)->HasAuthority())
			{
				DamagedPlayer1P = nullptr;
			}
			else
			{
				DamagedPlayer2P = nullptr;
			}
		}
		
	}

}

void ABoss_Phase1::ServerRPCTrapAttack_Implementation()
{
	if (HasAuthority())
	{
		MulticastRPCTrapAttack();
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &ABoss_Phase1::TrapAttackState, TrapSkillCoolTime, false);

	}
	
	//bIsSkillStart = true;
}

void ABoss_Phase1::MulticastRPCTrapAttack_Implementation()
{
	if(IsValid(AttackTrapMontage))
	{
		AnimInstance->Montage_Play(AttackTrapMontage, 1.5f);
	}
}


void ABoss_Phase1::ServerRPCPhaseChange_Implementation()
{
	MulticastRPCPhaseChange();
	
}

void ABoss_Phase1::MulticastRPCPhaseChange_Implementation()
{
	if (HasAuthority())
	{
		BossChair->SetActorHiddenInGame(true);
		bIsSwordSpawn = true;
	}

	SpawnChairItem();

	if (IsValid(PhaseChangeMontage) && bIsSkillStart != true)
	{
		AnimInstance->Montage_Play(PhaseChangeMontage); 
		MoveEffectPosition->SetVisibility(false, true);
		bIsSkillStart = true;
	}
}

void ABoss_Phase1::ServerRPCADCAttack_Implementation()
{
	MulticastRPCADCAttack();
	bIsAttackEnd = true;
}

void ABoss_Phase1::MulticastRPCADCAttack_Implementation()
{
	if (IsValid(AttackADCMontage))
	{
		
		AnimInstance->Montage_Play(AttackADCMontage);
		GetWorld()->GetTimerManager().SetTimer(ADCTimerHandle, this, &ABoss_Phase1::ADCAttackState, ADCAttackCoolTime, false);
	}
}

void ABoss_Phase1::ServerRPCCameraShaking_Implementation()
{
	MulticastRPCCameraShaking();
}

void ABoss_Phase1::MulticastRPCCameraShaking_Implementation()
{
	APawn* Pawn = GetWorld()->GetFirstPlayerController()->GetPawn();
	if (IsValid(Pawn))
	{
		ATrapperPlayer* TP = Cast<ATrapperPlayer>(Pawn);
		if (IsValid(TP))
		{
			TP->ShakeCamera(this->GetActorLocation(), ECameraShakeType::BossMonster);
		}
	}
}

void ABoss_Phase1::ServerRPCHitEffect_Implementation()
{
	if (HasAuthority())
	{
		MulticastRPCHitEffect();
	}
	
}

void ABoss_Phase1::MulticastRPCHitEffect_Implementation()
{
	MoveEffectPosition->SetVisibility(false, true);
	HitEffectPosition->SetVisibility(true, true);

	GetWorld()->GetTimerManager().SetTimer(HitTimerHandle, this, &ABoss_Phase1::OnOffEffect, 0.4, false);
}

void ABoss_Phase1::ServerRPCRockAttack_Implementation()
{
	if(HasAuthority())
	{
		MulticastRPCRockAttack();
	}
}

void ABoss_Phase1::MulticastRPCRockAttack_Implementation()
{
	if (!IsValid(AnimInstance))
	{
		return;
	}

	if (IsValid(AttackRockMontage))
	{
		AnimInstance->Montage_Play(AttackRockMontage, 1.5f);
	}
}
