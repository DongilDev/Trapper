// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster/Debuffer.h"
#include "Components/SphereComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "Data/MonsterDataTables.h"

ADebuffer::ADebuffer()
{
	PillerMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PillerMesh"));
	PillerMesh->SetupAttachment(RootComponent);

	MonsterMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MonsterMesh"));
	MonsterMesh->SetupAttachment(RootComponent);

	BufferRadius = CreateDefaultSubobject<USphereComponent>(TEXT("BufferRadius"));
	BufferRadius->SetupAttachment(RootComponent);

	DebuffePosition = CreateDefaultSubobject<USceneComponent>(TEXT("DebuffePosition"));
	DebuffePosition->SetupAttachment(RootComponent);

	MovePosition = CreateDefaultSubobject<USceneComponent>(TEXT("MovePosition"));
	MovePosition->SetupAttachment(RootComponent);

    
	//Add Dynamic
	BufferRadius->OnComponentBeginOverlap.AddDynamic(this, &ADebuffer::OnOverlapBeginMonsters);
	BufferRadius->OnComponentEndOverlap.AddDynamic(this, &ADebuffer::OnOVerlapEndMonsters);
}

void ADebuffer::BeginPlay()
{
	Super::BeginPlay();

	if (IsValid(DebuffeEffect))
	{
		UNiagaraFunctionLibrary::SpawnSystemAttached(
			DebuffeEffect,
			DebuffePosition,
			NAME_None,
			FVector(0.f, 0.f, 0.f),
			FRotator::ZeroRotator,
			EAttachLocation::KeepRelativeOffset,
			true);
	}

	if (IsValid(MoveEffect))
	{
		UNiagaraFunctionLibrary::SpawnSystemAttached(
			MoveEffect,
			MovePosition,
			NAME_None,
			FVector(0.f, 0.f, 0.f),
			FRotator::ZeroRotator,
			EAttachLocation::KeepRelativeOffset,
			true);
	}

	///SetData
	//HP
	if (MonsterInfoTable[3].HP)
	{
		MaxHP = MonsterInfoTable[3].HP;
		//UE_LOG(LogTemp, Warning, TEXT("Check HP : %f"), MonsterInfoTable[3].HP);
		CurrentHP = MaxHP;
	}
	//Attack Damage
	if (MonsterInfoTable[3].Attack)
	{
		//AttackDamage = MonsterInfoTable[2].Attack;
	}
	//Speed
	if (MonsterInfoTable[3].MoveSpeed)
	{
		MonsterSpeed = MonsterInfoTable[3].MoveSpeed;
		GetCharacterMovement()->MaxWalkSpeed = MonsterSpeed;
	}
	if (MonsterInfoTable[3].ChaseSpeed)
	{
		MonsterDetectSpeed = MonsterInfoTable[3].ChaseSpeed;
	}
	if (MonsterInfoTable[3].ChaseSpeed)
	{
		MonsterEffectSpeed = MonsterInfoTable[3].EffectSpeed;
	}
	//DetectTime
	if (MonsterInfoTable[3].LocationTimer)
	{
		DetectTime = MonsterInfoTable[3].LocationTimer;
	}
	//Item
	if (MonsterInfoTable[3].SpawnAmount)
	{
		ItemMinDropNumber = MonsterInfoTable[3].SpawnAmount - 1;
		ItemMaxDropNumber = MonsterInfoTable[3].SpawnAmount;
	}
	//Debuffe Timer
	if (MonsterInfoTable[3].SpawnAmount)
	{
		DebuffeTime = MonsterInfoTable[3].EffectTimer * 10;
	}
	//Pillar Speed
	if (MonsterInfoTable[3].PillarSpeed)
	{
		MonsterPillarSpeed = MonsterInfoTable[3].PillarSpeed;
	}
	//Pillar StunTime
	if (MonsterInfoTable[3].PillarStunTime)
	{
		MonsterPillarTime = MonsterInfoTable[3].PillarStunTime;
	}
}

void ADebuffer::ServerRPCGoToTowerState_Implementation()
{
	MulticastRPCGoToTowerState();
}

void ADebuffer::MulticastRPCGoToTowerState_Implementation()
{
	//PillerMesh->SetVisibility(false, true);
	//MonsterMesh->SetVisibility(false, true);

	MovePosition->SetVisibility(true, true);
	DebuffePosition->SetVisibility(false, true);

	//UE_LOG(LogTemp, Warning, TEXT("Debuffe"));

	
}

void ADebuffer::ServerRPCDebuffeState_Implementation()
{
	MulticastRPCDebuffeState();
}

void ADebuffer::MulticastRPCDebuffeState_Implementation()
{
	//PillerMesh->SetVisibility(true, true);
	//MonsterMesh->SetVisibility(true, true);

	MovePosition->SetVisibility(false, true);
	DebuffePosition->SetVisibility(true, true);

	UE_LOG(LogTemp, Warning, TEXT("Debuffe Start"));

}

void ADebuffer::OnOverlapBeginMonsters(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//UE_LOG(LogTemp, Warning, TEXT("BeginOVerlap Work?"));

	if (bIsDebuffeStart == true)
	{
		if (OtherActor->ActorHasTag("Monster"))
		{
			TObjectPtr<ABaseMonster> OwnerMonster = Cast<ABaseMonster>(OtherActor);
			if (OwnerMonster->bIsDebuffeTimerStart == false && OwnerMonster->bIsDebuffe == false)
			{
				OwnerMonster->bIsDebuffeTimerStart = true;
				OwnerMonster->ServerRPCEffectActive();
				//UE_LOG(LogTemp, Warning, TEXT("Monster Debuffe!!"));
				//UE_LOG(LogTemp, Warning, TEXT("%s"), *OtherActor->GetName())
			}
		}
	}
}

void ADebuffer::OnOVerlapEndMonsters(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	//UE_LOG(LogTemp, Warning, TEXT("EndOverlap Work?"));
}
