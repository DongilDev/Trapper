// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster/Animation/Effect/AnimNotify_PlayDieEffect.h"
#include "Monster/BaseMonster.h"

void UAnimNotify_PlayDieEffect::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);

	if (IsValid(MeshComp))
	{
		TObjectPtr<ABaseMonster> DieMonster = Cast<ABaseMonster>(MeshComp->GetOwner());

		if (DieMonster != nullptr)
		{
			if (DieMonster->DieEffect != nullptr)
			{
				DieMonster->ServerRPCPlayDieEffect();
			}
		}
		
	}
}


