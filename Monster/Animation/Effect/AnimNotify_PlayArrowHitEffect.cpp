// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster/Animation/Effect/AnimNotify_PlayArrowHitEffect.h"
#include "Monster/BaseMonster.h"

void UAnimNotify_PlayArrowHitEffect::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{

	Super::Notify(MeshComp, Animation);

	if (IsValid(MeshComp))
	{
		TObjectPtr<ABaseMonster> HitMonster = Cast<ABaseMonster>(MeshComp->GetOwner());

		if (HitMonster != nullptr)
		{
			if (HitMonster->ArrowHitEffect != nullptr)
			{
				HitMonster->ServerRPCPlayArrowHitEffect();
			}
		}
	}
}
