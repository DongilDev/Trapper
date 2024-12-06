// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster/Animation/AnimNotify_ComboAttackEnd.h"

#include "Monster/Boss/Boss_Phase2.h"

void UAnimNotify_ComboAttackEnd::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	TObjectPtr<ABoss_Phase2> AttackMonster = Cast<ABoss_Phase2>(MeshComp->GetOwner());

	if (IsValid(AttackMonster))
	{
		
		UAnimInstance* AnimInstance = AttackMonster->GetMesh()->GetAnimInstance();
		AnimInstance->StopAllMontages(0.0f);

		AttackMonster->MonsterState = EMonsterStateType::GoToTower;
		AttackMonster->bIsCDCComboState = false;
		
	}
}
