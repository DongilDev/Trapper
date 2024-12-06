// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster/Animation/AnimNotify_DestroyRock.h"

#include "EngineUtils.h"
#include "Kismet/GameplayStatics.h"

#include "Monster/Boss/Boss_Phase1.h"
#include "Monster/Boss/Boss_Phase2.h"
#include "Trap/TrapBase.h"
#include "GameElement/MagneticPillar.h"
#include "Character/TrapperPlayer.h"
#include "GameElement/Arch.h"

void UAnimNotify_DestroyRock::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);
	
	AnimMeshComp = MeshComp;

	//FindArch();

	TObjectPtr<ABoss_Phase1> AttackMonster1 = Cast<ABoss_Phase1>(MeshComp->GetOwner());
	TObjectPtr<ABoss_Phase2> AttackMonster2 = Cast<ABoss_Phase2>(MeshComp->GetOwner());
	
	if (IsValid(AttackMonster1))
	{
		AttackMonster1->ServerRPCCameraShaking();
		
		//Rock Destory
		if (IsValid(AttackMonster1->Boss1Arch0) && AttackMonster1->ArchNum == 0)
		{
			AttackMonster1->Boss1Arch0->CallFragment();
		}

		if (IsValid(AttackMonster1->Boss1Arch1) && AttackMonster1->ArchNum == 1)
		{
			AttackMonster1->Boss1Arch1->CallFragment();
		}

		/*if (AttackMonster1->DetectTrapList.Num() != 0)
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
		}*/
	}

	else if (IsValid(AttackMonster2))
	{
		AttackMonster2->ServerRPCCameraShaking();

		//Rock Destory
		if (IsValid(AttackMonster2->Boss2Arch0) && AttackMonster2->ArchNum == 0)
		{
			AttackMonster2->Boss2Arch0->CallFragment();
		}

		if (IsValid(AttackMonster2->Boss2Arch1) && AttackMonster2->ArchNum == 1)
		{
			AttackMonster2->Boss2Arch1->CallFragment();
		}

		/*if (AttackMonster2->DetectTrapList.Num() != 0)
		{
			AttackMonster2->bSafeRanged = true;
			for (const auto& Trap : AttackMonster2->DetectTrapList)
			{
				if (!IsValid(Trap))
				{
					continue;
				}
				Cast<ATrapBase>(Trap)->OnAttackedByBoss();
				DestroyTrapList.Add(Trap);
				UE_LOG(LogTemp, Warning, TEXT("TrapDistory!"));
			}

			AttackMonster2->bSafeRanged = false;

			for (const auto& DestoryTrap : DestroyTrapList)
			{
				if (AttackMonster2->DetectTrapList.Find(DestoryTrap))
				{
					AttackMonster2->DetectTrapList.Remove(DestoryTrap);
				}

			}
			DestroyTrapList.Reset();
		}

		if (AttackMonster2->DetectMagneticList.Num() != 0)
		{
			AttackMonster2->bSafeRanged = true;
			for (const auto& Pillar : AttackMonster2->DetectMagneticList)
			{
				if (!IsValid(Pillar))
				{
					continue;
				}

				Cast<AMagneticPillar>(Pillar)->Break();
				DestroyPillarList.Add(Pillar);
				UE_LOG(LogTemp, Warning, TEXT("PillarDistory!"));
			}

			AttackMonster2->bSafeRanged = false;

			for (const auto& DestoryPillar : DestroyPillarList)
			{
				if (AttackMonster2->DetectMagneticList.Find(DestoryPillar))
				{
					AttackMonster2->DetectMagneticList.Remove(DestoryPillar);
				}

			}
			DestroyPillarList.Reset();
		}*/

	}
}

void UAnimNotify_DestroyRock::FindArch()
{
	//for (TActorIterator<AActor> MyActor(AnimMeshComp->GetOwner()->GetWorld()); MyActor; ++MyActor)
	//{
	//	if (MyActor->ActorHasTag("Arch0"))
	//	{
	//		//AActor CurrentActor = MyActor;
	//		Arch0 = Cast<AArch>(*MyActor);
	//		UE_LOG(LogTemp, Warning, TEXT("Find Arch0"));
	//	}

	//	if (MyActor->ActorHasTag("Arch1"))
	//	{
	//		//AActor* CurrentActor = *MyActor;
	//		Arch1 = Cast<AArch>(*MyActor);
	//		UE_LOG(LogTemp, Warning, TEXT("Find Arch1"));
	//	}
	//}
}
