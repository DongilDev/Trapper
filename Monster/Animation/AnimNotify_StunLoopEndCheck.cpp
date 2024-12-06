// Fill out your copyright notice in the Description page of Project Settings.
#include "Monster/Animation/AnimNotify_StunLoopEndCheck.h"
#include "Monster/BaseMonster.h"


void UAnimNotify_StunLoopEndCheck::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (IsValid(MeshComp))
	{
		TObjectPtr<ABaseMonster>HitMonster = Cast<ABaseMonster>(MeshComp->GetOwner());

		if (IsValid(HitMonster))
		{
			if (HitMonster->HasAuthority())
			{
				//조건이 맞아떨어지면
				//HitMonster->ServerRPCStunEnd();
			}
		}
		
	
	}
}
