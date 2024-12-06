// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster/Animation/AnimNotify_DownMontageStart.h"
#include "Monster/BaseMonster.h"
#include "Monster/Debuffer.h"

void UAnimNotify_DownMontageStart::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (IsValid(MeshComp))
	{
		TObjectPtr<ABaseMonster> HitMonster = Cast<ABaseMonster>(MeshComp->GetOwner());
		TObjectPtr<ADebuffer> HitDebufferMonster = Cast<ADebuffer>(MeshComp->GetOwner());

		if (IsValid(HitMonster))
		{
			if (HitMonster->HasAuthority())
			{
				if (HitDebufferMonster)
				{
					HitDebufferMonster->MonsterState = EMonsterStateType::GoToTower;
					//UE_LOG(LogTemp, Warning, TEXT("GoToTower : DownMontageExcetion"));
				}

				else
				{
					HitMonster->ServerRPCDownLoop();
				}
			}
		}
	}
}
