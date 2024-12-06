// Fill out your copyright notice in the Description page of Project Settings.


#include "GameElement/Arch.h"
#include "TrapperProject.h"
#include "Trap/GuillotinePendulumTrap.h"
#include "Trap/TrabSnabEnviroment/TrapSnapTrigger.h"


AArch::AArch()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AArch::CallFragment()
{
	SpawnFragment();

	if (!HasAuthority())
	{
		return;
	}

	GetWorld()->GetTimerManager().SetTimerForNextTick(this, &AArch::DestoryFragment);

	UE_LOG(LogTrap, Warning, TEXT("TrapArray Num : %d"), TrapArray.Num());

	TArray<TWeakObjectPtr<ATrapBase>> CopyTrapArray = TrapArray;
	for (const auto& Trap : CopyTrapArray)
	{
		if (!Trap.IsValid())
		{
			continue;
		}

		UE_LOG(LogTrap, Warning, TEXT("Trap Name : %s"), *Trap->GetName());
		if (Trap->ActorHasTag("GuillotinePendulumTrap"))
		{
			UE_LOG(LogTrap, Warning, TEXT("OnAttackedByBoss"));
			Trap->OnAttackedByBoss();
			Trap->SetLifeSpan(5);
		}
		else
		{
			UE_LOG(LogTrap, Warning, TEXT("Demolish"));
			Trap->Demolish();
			Trap->Destroy();
		}
	}

	if (TrapSnapTrigger)
	{
		UE_LOG(LogTrap, Warning, TEXT("TrapSnapTrigger destroy"));
		TrapSnapTrigger->Destroy();
	}

	if (CellingTrigger)
	{
		UE_LOG(LogTrap, Warning, TEXT("CellingTrigger destroy"));
		CellingTrigger->Destroy();
	}
}


