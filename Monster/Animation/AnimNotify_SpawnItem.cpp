// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster/Animation/AnimNotify_SpawnItem.h"
#include "Monster/Boss/Boss_Phase1.h"
#include "Monster/Weapon/BossSword.h"

void UAnimNotify_SpawnItem::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{

	Super::Notify(MeshComp, Animation, EventReference);

	if (IsValid(MeshComp))
	{
		TObjectPtr<ABoss_Phase1>BossMonster = Cast<ABoss_Phase1>(MeshComp->GetOwner());

		if (IsValid(BossMonster))
		{
			if (BossMonster->HasAuthority())
			{

				if (BossMonster->bIsSwordHandle == true)
				{
					BossMonster->BossSword->SetActorHiddenInGame(false);
				}

				if (BossMonster->bIsSwordSpawn == true)
				{
					BossMonster->SpawnSwordItem();
					//BossMonster->BossSword->SetActorHiddenInGame(false);
					BossMonster->bIsSwordHandle = true;
					BossMonster->bIsSwordSpawn = false;
				}
				
			}
		}
	}
}
