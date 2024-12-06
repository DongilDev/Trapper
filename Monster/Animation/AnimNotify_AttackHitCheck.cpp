// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster/Animation/AnimNotify_AttackHitCheck.h"

#include "Monster/Animation/AnimationAttackInterface.h"

void UAnimNotify_AttackHitCheck::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (IsValid(MeshComp))
	{
		IAnimationAttackInterface* AttackPawn = Cast<IAnimationAttackInterface>(MeshComp->GetOwner());

		if (AttackPawn)
		{
			AttackPawn->AttackHitCheck();
		}
		 
	}
}
