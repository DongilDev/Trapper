// Fill out your copyright notice in the Description page of Project Settings.

#include "Monster/Animation/AnimNotify_AttackMontageEnd.h"
#include "Monster/BaseMonster.h"

void UAnimNotify_AttackMontageEnd::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	TObjectPtr<class ABaseMonster> AttackMonster = Cast<ABaseMonster>(MeshComp->GetOwner());

	if (IsValid(AttackMonster))
	{
		if (AttackMonster->MonsterState == EMonsterStateType::IsAttackStart)
		{
			AttackMonster->MonsterState = EMonsterStateType::IsAttack;
		}

		if (AttackMonster->bIsAttackEnd == true)
		{
			UAnimInstance* AnimInstance = AttackMonster->GetMesh()->GetAnimInstance();
			AnimInstance->StopAllMontages(0.0f);

			AttackMonster->MonsterState = EMonsterStateType::GoToTower;
			//UE_LOG(LogTemp, Warning, TEXT("Go To Tower : AttackEnd"));
		}
	}

	
}
