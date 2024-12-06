// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster/AI/Task/BTTask_DestroyActor.h"

#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameElement/Spawner.h"

#include "Monster/PathTarget.h"
#include "Monster/DebufferPathTarget.h"
#include "Monster/ADC.h"
#include "Monster/CDC.h"
#include "Monster/DetectedMonster.h"
#include "Monster/Debuffer.h"
#include "Monster/BaseMonster.h"
#include "Monster/Boss/Boss_Phase1.h"
#include "Monster/Boss/Boss_Phase2.h"
#include "Framework/TrapperGameMode.h"
#include "Framework/TrapperGameState.h"
#include "Monster/AI/BaseMonsterAIController.h"
#include "Monster/AI/DebufferMonsterAIController.h"
#include "Monster/Weapon/BossChair.h"
#include "Monster/Weapon/BossSword.h"
#include "Monster/Weapon/Sword.h"
#include "Monster/Weapon/Wand.h"
#include "TrapperProject.h"

EBTNodeResult::Type UBTTask_DestroyActor::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMenory)
{
	ATrapperGameMode* TrapperGameMode = Cast<ATrapperGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	ABaseMonsterAIController* AIC = Cast<ABaseMonsterAIController>(OwnerComp.GetAIOwner());
	
	if (IsValid(AIC))
	{
		APawn* Pawn = AIC->GetPawn();
		if (Pawn)
		{
			//UE_LOG(LogTemp, Warning, TEXT("Destory Actor"));
			//TObjectPtr<ABaseMonster> Monster = Cast<ABaseMonster>(Pawn);
			ABaseMonster* Monster = Cast<ABaseMonster>(Pawn);

			/*for (int i = 0; i < TrapperGameMode->Spanwer->SpawnMonsterList.Num(); i++)
			{
				UE_LOG(LogTemp, Warning, TEXT("MonsterList : %s"), *TrapperGameMode->Spanwer->SpawnMonsterList[i].GetName());
			}*/
			
			//UE_LOG(LogTemp, Warning, TEXT("Remove Monster: %s"), *Monster->GetName());

			if (IsValid(Monster))
			{
				/// Todo : RPC로 다 던져버리기 근데 이걸로도 뭔가 뭔가던데
				/// 결국 오브젝트 풀은 죽었다. 
				//Monster->SetActorLocation(Monster->StartPoint);
				//Monster->TeleportTo(Monster->StartPoint, Monster->GetActorRotation());
				//Monster->ServerRPCTeleportToDie();
				Monster->bIsSpawn = false;

				//Path Clear
				for (int i = 0; i < AIC->PathTargets.Num(); i++)
				{
					//UE_LOG(LogTemp, Warning, TEXT("Remove Success"));
					AIC->PathTargets[i]->CheckTarget.Remove(Monster);
				}

				//CDCsword Delete
				if (Cast<ACDC>(Monster))
				{
					//보스가 가지고 있는거 삭제
					Cast<ACDC>(Monster)->Sword->Destroy();
				}

				if (Cast<AADC>(Monster))
				{
					//보스가 가지고 있는거 삭제
					Cast<AADC>(Monster)->Wand->Destroy();
				}

				//DebufferClear
				if (Cast<ADebuffer>(Monster))
				{
					//Path Clear
					TObjectPtr<ADebufferMonsterAIController> DebufferAI = Cast<ADebufferMonsterAIController>(AIC);

					if (IsValid(DebufferAI))
					{
						for (int i = 0; i < DebufferAI->DebufferTargets.Num(); i++)
						{

							DebufferAI->DebufferTargets[i]->CheckTarget.Remove(Cast<ADebuffer>(Monster));
						}
					}
					//UE_LOG(LogTemp, Warning, TEXT("CDC Die"));
				}
				if (Cast<ABoss_Phase1>(Monster))
				{
					//보스가 가지고 있는거 삭제
					Cast<ABoss_Phase1>(Monster)->BossChair->Destroy();
					Cast<ABoss_Phase1>(Monster)->BossSword->Destroy();

				}

				if (Cast<ABoss_Phase2>(Monster))
				{
					//보스가 가지고 있는거 삭제
					Cast<ABoss_Phase2>(Monster)->BossSword->Destroy();

					//보스 죽은거 보냄
					if (Monster->HasAuthority())
					{
						GetWorld()->GetGameState<ATrapperGameState>()->OnQuestExecute.Broadcast(97, true);
					}
					//UE_LOG(LogTemp, Warning, TEXT("CDC Die"));
				}

				if (IsValid(TrapperGameMode->Spanwer))
				{
					if (TrapperGameMode->Spanwer->SpawnMonsterList.Contains(Monster))
					{
						TrapperGameMode->Spanwer->SpawnMonsterList.Remove(Monster);
					}
				}

				//UE_LOG(LogTemp, Warning, TEXT("Boss Die!"));

				GetWorld()->GetGameState<ATrapperGameState>()->ChangeMonsterCount(-1);
				Monster->Destroy();
				//UE_LOG(LogTrap, Warning, TEXT("BTTast_DestroyActor"));
				  
				

				//Clear는 필요 없어짐!
				/*Monster->CurrentHP = Monster->MaxHP;
				Monster->CurrentTarget = 0;
				Monster->MonsterState = EMonsterStateType::GoToTower;
				Monster->GetCharacterMovement()->MaxWalkSpeed = Monster->MonsterSpeed;
				Monster->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);*/
				//Monster->GetMesh()->SetVisibility(false);

			//	if (IsValid(TrapperGameMode->Spanwer))
			//	{
			//		
			//		if (Monster->HasAuthority())
			//		{
			//			//UE_LOG(LogTemp, Warning, TEXT("Check Die : %s"), *Monster->GetName());
			//			//Objectpool
			//			if (Cast<AADC>(Monster))
			//			{
			//				if (TrapperGameMode->Spanwer->ADCList.Find(Cast<AADC>(Monster)) == INDEX_NONE)
			//				{
			//					TrapperGameMode->Spanwer->ADCList.Add(Cast<AADC>(Monster));
			//					GetWorld()->GetGameState<ATrapperGameState>()->ChangeMonsterCount(-1);
			//				}

			//				//UE_LOG(LogTemp, Warning, TEXT("ADC Die"));
			//			}
			//			if (Cast<ACDC>(Monster))
			//			{
			//				if (TrapperGameMode->Spanwer->CDCList.Find(Cast<ACDC>(Monster)) == INDEX_NONE)
			//				{
			//					TrapperGameMode->Spanwer->CDCList.Add(Cast<ACDC>(Monster));
			//					GetWorld()->GetGameState<ATrapperGameState>()->ChangeMonsterCount(-1);
			//				}

			//				//UE_LOG(LogTemp, Warning, TEXT("CDCList Size : %d"), TrapperGameMode->Spanwer->CDCList.Num());
			//			}
			//			if (Cast<ADetectedMonster>(Monster))
			//			{
			//				if (TrapperGameMode->Spanwer->DetectedList.Find(Cast<ADetectedMonster>(Monster)) == INDEX_NONE)
			//				{
			//					TrapperGameMode->Spanwer->DetectedList.Add(Cast<ADetectedMonster>(Monster));
			//					GetWorld()->GetGameState<ATrapperGameState>()->ChangeMonsterCount(-1);
			//				}

			//				//UE_LOG(LogTemp, Warning, TEXT("CDC Die"));
			//			}
			//			if (Cast<ADebuffer>(Monster))
			//			{
			//				if (TrapperGameMode->Spanwer->DebufferList.Find(Cast<ADebuffer>(Monster)) == INDEX_NONE)
			//				{
			//					TrapperGameMode->Spanwer->DebufferList.Add(Cast<ADebuffer>(Monster));
			//					GetWorld()->GetGameState<ATrapperGameState>()->ChangeMonsterCount(-1);
			//				}

			//				Cast<ADebuffer>(Monster)->bIsDebuffeStart = false;
			//				//Path Clear
			//				TObjectPtr<ADebufferMonsterAIController> DebufferAI = Cast<ADebufferMonsterAIController>(AIC);

			//				if (IsValid(DebufferAI))
			//				{
			//					for (int i = 0; i < DebufferAI->DebufferTargets.Num(); i++)
			//					{

			//						DebufferAI->DebufferTargets[i]->CheckTarget.Remove(Cast<ADebuffer>(Monster));
			//					}
			//				}
			//				//UE_LOG(LogTemp, Warning, TEXT("CDC Die"));
			//			}

			//			if (Cast<ABoss_Phase2>(Monster))
			//			{
			//				if (TrapperGameMode->Spanwer->BossPhase2List.Find(Cast<ABoss_Phase2>(Monster)) == INDEX_NONE)
			//				{
			//					TrapperGameMode->Spanwer->BossPhase2List.Add(Cast<ABoss_Phase2>(Monster));
			//					GetWorld()->GetGameState<ATrapperGameState>()->ChangeMonsterCount(-1);
			//				}
			//			
			//				//보스 죽은거 보냄
			//				if (Monster->HasAuthority())
			//				{
			//					GetWorld()->GetGameState<ATrapperGameState>()->OnQuestExecute.Broadcast(97, true);
			//				}
			//				//UE_LOG(LogTemp, Warning, TEXT("CDC Die"));
			//			}


			//			if (TrapperGameMode->Spanwer->SpawnMonsterList.Find(Monster) != INDEX_NONE)
			//			{
			//				TrapperGameMode->Spanwer->SpawnMonsterList.Remove(Monster);
			//				//GetWorld()->GetGameState<ATrapperGameState>()->ChangeMonsterCount(-1);
			//			}
			//		}

			//		
			//	}
			//	//Set MonsterAnimation As Idle
			//	//Monster->ServerRPCIdle();
			//}
				//그저 Destory...
				return EBTNodeResult::Succeeded;
			}
			
		}
	}

	return EBTNodeResult::Failed;
}
