// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster/Animation/AnimNotify_HitMontageEnd.h"
#include "Monster/BaseMonster.h"
#include "Monster/Debuffer.h"

void UAnimNotify_HitMontageEnd::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);
	
	if (IsValid(MeshComp))
	{
		TObjectPtr<ABaseMonster> HitMonster = Cast<ABaseMonster>(MeshComp->GetOwner());
		TObjectPtr<ADebuffer> HitDebufferMonster = Cast<ADebuffer>(MeshComp->GetOwner());
		
		if (IsValid(HitMonster))
		{

			//Debuffer일 경우는 Detect를 하지 않는다.
			if (HitDebufferMonster)
			{
				HitDebufferMonster->MonsterState = EMonsterStateType::GoToTower;
				//UE_LOG(LogTemp, Warning, TEXT("GoToTower : HitMontage Excetion"));
			}

			else
			{
				if (HitMonster->bISArrowShoot == true)
				{
					if (HitMonster->bIsHeadShoot == true)
					{
						HitMonster->OnHeadShootHit();
						HitMonster->bIsHeadShoot = false;
					}
					else
					{
						HitMonster->OnNotHeadShootHit();

					}

					HitMonster->bISArrowShoot = false;

					if (HitMonster->MonsterState != EMonsterStateType::IsStun &&
						HitMonster->MonsterState != EMonsterStateType::IsDead &&
						HitMonster->MonsterState != EMonsterStateType::IsDetect &&
						HitMonster->MonsterState != EMonsterStateType::IsDetectStart)
					{
						HitMonster->ServerRPCDetectStart();
						HitMonster->MonsterState = EMonsterStateType::IsDetectStart;
					}
				}

				else
				{
					HitMonster->MonsterState = EMonsterStateType::GoToTower;
					UE_LOG(LogTemp, Warning, TEXT("Go To Tower : Hit Montage End"));
				}
			}
		}

	}
}
