// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster/Animation/AnimNotify_RockAttackEndCheck.h"
#include "Monster/BaseMonster.h"

void UAnimNotify_RockAttackEndCheck::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	TObjectPtr<class ABaseMonster> AttackMonster = Cast<ABaseMonster>(MeshComp->GetOwner());

	if (IsValid(AttackMonster))
	{
		AttackMonster->MonsterState = EMonsterStateType::GoToTower;
		//AttackMonster->bIsSkillStart = false;
		//AttackMonster->AnimInstance->StopAllMontages(0.0f);
		//UE_LOG(LogTemp, Warning, TEXT("GoToTower : TrapAttackEndCheck"));

		//AttackMonster->bIsAttackEnd = true;
		//UE_LOG(LogTemp, Warning, TEXT("Check Animation End"));
	}
}
