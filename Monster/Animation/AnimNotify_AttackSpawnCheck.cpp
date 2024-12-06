// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster/Animation/AnimNotify_AttackSpawnCheck.h"
#include "Monster/ADC.h"
#include "Monster/Boss/Boss_Phase1.h"
#include "Monster/Boss/Boss_Phase2.h"

void UAnimNotify_AttackSpawnCheck::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (IsValid(MeshComp))
	{
		TObjectPtr<AADC> AttackPawn = Cast<AADC>(MeshComp->GetOwner());

		if (AttackPawn)
		{
			AttackPawn->SpawnMagicBall();
		}
		
		TObjectPtr<ABoss_Phase1> AttackBoss1 = Cast<ABoss_Phase1>(MeshComp->GetOwner());

		if (AttackBoss1)
		{
			AttackBoss1->SpawnSword();
		}

		TObjectPtr<ABoss_Phase2> AttackBoss2 = Cast<ABoss_Phase2>(MeshComp->GetOwner());
		
		if (AttackBoss2)
		{
			AttackBoss2->SpawnSword();
			
		}

		
	}
}
