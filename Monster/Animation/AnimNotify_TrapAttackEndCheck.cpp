// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster/Animation/AnimNotify_TrapAttackEndCheck.h"
#include "Monster/BaseMonster.h"
#include "Monster/Boss/Boss_Phase2.h"

void UAnimNotify_TrapAttackEndCheck::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	TObjectPtr<class ABaseMonster> AttackMonster = Cast<ABaseMonster>(MeshComp->GetOwner());

	if (IsValid(AttackMonster))
	{
		if(AttackMonster->bIsTowerDetected != true)
		{
			AttackMonster->MonsterState = EMonsterStateType::GoToTower;
			AttackMonster->bIsSkillStart = false;
		}
		else
		{
			AttackMonster->MonsterState = EMonsterStateType::IsTowerAttack;
			AttackMonster->bIsSkillStart = false;
		}
		
		//AttackMonster->AnimInstance->StopAllMontages(0.0f);
		//UE_LOG(LogTemp, Warning, TEXT("GoToTower : TrapAttackEndCheck"));

		//AttackMonster->bIsAttackEnd = true;
		//UE_LOG(LogTemp, Warning, TEXT("Check Animation End"));
	}
}
