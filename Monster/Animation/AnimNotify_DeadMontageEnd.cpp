// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster/Animation/AnimNotify_DeadMontageEnd.h"
#include "Monster/BaseMonster.h"
#include "Character/TrapperPlayer.h"

void UAnimNotify_DeadMontageEnd::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (IsValid(MeshComp))
	{
		TObjectPtr<ABaseMonster> DeadMonster = Cast<ABaseMonster>(MeshComp->GetOwner());
		if (DeadMonster && DeadMonster->MonsterState != EMonsterStateType::IsDestroy)
		{
			//DeadMonster->MonsterDieCheck();
			DeadMonster->MonsterState = EMonsterStateType::IsDestroy;
			UE_LOG(LogTemp, Warning, TEXT("DieMontage End"));
		}

		
	}
}
