// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster/AI/Service/BTService_LookPlayer.h"
#include "Kismet/KismetMathLibrary.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Character/TrapperPlayer.h"
#include "Monster/BaseMonster.h"
#include "Monster/AI/BaseMonsterAIController.h"

void UBTService_LookPlayer::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	ABaseMonster* Monster = Cast<ABaseMonster>(OwnerComp.GetAIOwner()->GetPawn());
	
	if (IsValid(Monster))
	{
		//플레이어가 감지되면
		if (Monster->bIsSightDetect)
		{
			TObjectPtr<ATrapperPlayer> DetectPlayer = Monster->DetectPlayer;
			
			//감지된 플레이어에 대해
			if (IsValid(DetectPlayer))
			{
				FRotator LookRot = UKismetMathLibrary::FindLookAtRotation(
					Monster->GetActorLocation(),
					DetectPlayer->GetActorLocation()
				);

				FRotator TargetLook = FMath::RInterpTo(Monster->GetActorRotation(),
					LookRot,
					DeltaSeconds,
					15.0f);

				Monster->SetActorRotation(TargetLook);
			}
		}
	}
	
}
