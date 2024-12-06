// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster/Animation/AnimNotify_DownLoopEnd.h"
#include "Monster/BaseMonster.h"

#include "Monster/Animation/BaseMonsterAnimInstance.h"

void UAnimNotify_DownLoopEnd::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (IsValid(MeshComp))
	{
		TObjectPtr<ABaseMonster> HitMonster = Cast<ABaseMonster>(MeshComp->GetOwner());

		if (IsValid(HitMonster))
		{
			if (HitMonster->HasAuthority())
			{
				TObjectPtr<UBaseMonsterAnimInstance> BaseAnimInstance = Cast<UBaseMonsterAnimInstance>(HitMonster->AnimInstance);
				if (BaseAnimInstance->bIsFalling == false)
				{
					UAnimMontage* CurrentMontage = BaseAnimInstance->GetCurrentActiveMontage();
					FName NextSectionName(TEXT("DownEnd"));
					BaseAnimInstance->Montage_JumpToSection(NextSectionName, CurrentMontage);
					//UE_LOG(LogTemp, Warning, TEXT("Check Falling"));
				}
				//조건이 충족되면 End를 부른다
				//HitMonster->ServerRPCDownEnd();
			}
		}
	}
}
 