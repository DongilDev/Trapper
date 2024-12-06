// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster/Animation/AnimNotify_SpawnMonsterCheck.h"

#include "Monster/Boss/Boss_Phase2.h"

void UAnimNotify_SpawnMonsterCheck::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	TObjectPtr<class ABoss_Phase2> AttackMonster = Cast<ABoss_Phase2>(MeshComp->GetOwner());

	if (IsValid(AttackMonster))
	{
		AttackMonster->SpawnMonsters();
	}
}
