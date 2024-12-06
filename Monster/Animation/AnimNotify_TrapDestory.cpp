// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster/Animation/AnimNotify_TrapDestory.h"
#include "Monster/BaseMonster.h"
#include "Monster/Boss/Boss_Phase1.h"
#include "Monster/Boss/Boss_Phase2.h"

#include "Trap/TrapBase.h"
#include "GameElement/MagneticPillar.h"
#include "Character/TrapperPlayer.h"

void UAnimNotify_TrapDestory::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	TObjectPtr<ABoss_Phase1> AttackMonster1 = Cast<ABoss_Phase1>(MeshComp->GetOwner());
	TObjectPtr<ABoss_Phase2> AttackMonster = Cast<ABoss_Phase2>(MeshComp->GetOwner());
	
	if (IsValid(AttackMonster))
	{
		AttackMonster->ServerRPCCameraShaking();

		if (AttackMonster->DetectTrapList.Num() != 0)
		{
			AttackMonster->bSafeRanged = true;
			for (const auto& Trap : AttackMonster->DetectTrapList)
			{
				if (!IsValid(Trap))
				{
					continue;
				}
				Cast<ATrapBase>(Trap)->OnAttackedByBoss();
				DestroyTrapList.Add(Trap);
			}
			AttackMonster->bSafeRanged = false;

			for (const auto& DestoryTrap : DestroyTrapList)
			{
				if (AttackMonster->DetectTrapList.Find(DestoryTrap))
				{
					AttackMonster->DetectTrapList.Remove(DestoryTrap);
				}
				
			}
			DestroyTrapList.Reset();
		}

		if (AttackMonster->DetectMagneticList.Num() != 0)
		{
			AttackMonster->bSafeRanged = true;
			for (const auto& Pillar : AttackMonster->DetectMagneticList)
			{
				if (!IsValid(Pillar))
				{
					continue;
				}

				Cast<AMagneticPillar>(Pillar)->Break();
				DestroyPillarList.Add(Pillar);
			}

			AttackMonster->bSafeRanged = false;

			for (const auto& DestoryPillar : DestroyPillarList)
			{
				if (AttackMonster->DetectMagneticList.Find(DestoryPillar))
				{
					AttackMonster->DetectMagneticList.Remove(DestoryPillar);
				}

			}
			DestroyPillarList.Reset();
		}
	}
	else if (IsValid(AttackMonster1))
	{
		AttackMonster1->ServerRPCCameraShaking();

		if (AttackMonster1->DetectTrapList.Num() != 0)
		{
			AttackMonster1->bSafeRanged = true;
			for (const auto& Trap : AttackMonster1->DetectTrapList)
			{
				if (!IsValid(Trap))
				{
					continue;
				}
				Cast<ATrapBase>(Trap)->OnAttackedByBoss();
				DestroyTrapList.Add(Trap);
				UE_LOG(LogTemp, Warning, TEXT("TrapDistory!"));
			}

			AttackMonster1->bSafeRanged = false;

			for (const auto& DestoryTrap : DestroyTrapList)
			{
				if (AttackMonster1->DetectTrapList.Find(DestoryTrap))
				{
					AttackMonster1->DetectTrapList.Remove(DestoryTrap);
				}

			}
			DestroyTrapList.Reset();
		}

		if (AttackMonster1->DetectMagneticList.Num() != 0)
		{
			AttackMonster1->bSafeRanged = true;
			for (const auto& Pillar : AttackMonster1->DetectMagneticList)
			{
				if (!IsValid(Pillar))
				{
					continue;
				}

				Cast<AMagneticPillar>(Pillar)->Break();
				DestroyPillarList.Add(Pillar);
				UE_LOG(LogTemp, Warning, TEXT("PillarDistory!"));
			}

			AttackMonster1->bSafeRanged = false;

			for (const auto& DestoryPillar : DestroyPillarList)
			{
				if (AttackMonster1->DetectMagneticList.Find(DestoryPillar))
				{
					AttackMonster1->DetectMagneticList.Remove(DestoryPillar);
				}

			}
			DestroyPillarList.Reset();
		}

	}
}
