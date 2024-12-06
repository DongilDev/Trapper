// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster/Animation/AnimNotify_IsComboStart.h"

#include "Monster/BaseMonster.h"
#include "Monster/Boss/Boss_Phase2.h"
#include "Monster/Animation/BaseMonsterAnimInstance.h"

void UAnimNotify_IsComboStart::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	TObjectPtr<ABoss_Phase2> AttackMonster = Cast<ABoss_Phase2>(MeshComp->GetOwner());

	if (IsValid(AttackMonster))
	{
		if (AttackMonster->HasAuthority())
		{
			//Combo가 실행될 수 있는가?
			if (AttackMonster->bIsCDCComboState == true)
			{
				TObjectPtr<UBaseMonsterAnimInstance> BaseAnimInstance = Cast<UBaseMonsterAnimInstance>(AttackMonster->AnimInstance);

				if (IsValid(BaseAnimInstance))
				{
					UAnimMontage* CurrentMontage = BaseAnimInstance->GetCurrentActiveMontage();
					FName NextSectionName(TEXT("ComboStart"));
					BaseAnimInstance->Montage_JumpToSection(NextSectionName, CurrentMontage);
					UE_LOG(LogTemp, Warning, TEXT("ComboStart"));
				}		
			}
		}
		

	
	}
}
