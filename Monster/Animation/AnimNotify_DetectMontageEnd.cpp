// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster/Animation/AnimNotify_DetectMontageEnd.h"
#include "Monster/BaseMonster.h"

void UAnimNotify_DetectMontageEnd::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (IsValid(MeshComp))
	{
		TObjectPtr<ABaseMonster> DetectMonster = Cast<ABaseMonster>(MeshComp->GetOwner());

		if (DetectMonster)
		{
			DetectMonster->MonsterState = EMonsterStateType::IsDetect;

		}
	}
}
