// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster/AI/Service/MyBTService_CheckState.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "EngineUtils.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "TimerManager.h"

#include "Framework/TrapperPlayerController.h"
#include "Monster/BaseMonster.h"
#include "Character/TrapperPlayer.h"
#include "Monster/AI/BaseMonsterAIController.h"
#include "Monster/AI/DebufferMonsterAIController.h"
#include "Tower/Tower.h"
#include "Monster/Animation/BaseMonsterAnimInstance.h"
#include "Monster/PathTarget.h"
#include "Monster/DebufferPathTarget.h"
#include "Monster/Debuffer.h"
#include "Monster/Boss/Boss_Phase1.h"
#include "Monster/Boss/Boss_Phase2.h"
#include "Monster/PathMonster.h"
#include "Turret/Turret.h"


void UMyBTService_CheckState::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	TObjectPtr<ABaseMonster> OwnerMonster = Cast<ABaseMonster>(OwnerComp.GetAIOwner()->GetPawn());
	TObjectPtr<APathMonster> PathMonster = Cast<APathMonster>(OwnerMonster);
	TObjectPtr<ABoss_Phase1> Boss_Phase1 = Cast<ABoss_Phase1>(OwnerMonster);
	TObjectPtr<ABoss_Phase2> Boss_Phase2 = Cast<ABoss_Phase2>(OwnerMonster);

	if (IsValid(OwnerMonster))
	{

		if(OwnerMonster->bIsDead)
		{
			//필요한 변수들 AI에서 받아오기
			OwnerMonsterAI = Cast<ABaseMonsterAIController>(OwnerComp.GetAIOwner());
			
			
			if (IsValid(OwnerMonsterAI))
			{
				if (IsValid(OwnerMonsterAI->Tower))
				{
					this->Tower = OwnerMonsterAI->Tower;
				}

				if (OwnerMonsterAI->PathTargets.Num() > 0)
				{
					this->PathTargets = OwnerMonsterAI->PathTargets;
				}


				DebufferMonsterAIController = Cast<ADebufferMonsterAIController>(OwnerComp.GetAIOwner());

				if (IsValid(DebufferMonsterAIController))
				{
					if (DebufferMonsterAIController->DebufferTargets.Num() > 0)
					{
						this->DebufferTargets = DebufferMonsterAIController->DebufferTargets;
					}
				}
			}

			//Blackboard 설정
			UBlackboardComponent* BBC = OwnerComp.GetBlackboardComponent();

			//Monster가 플레이어를 추적하는지 확인하는 변수 업데이트
			//Monster가 플레이어를 공격하는지 확인하는 변수 업데이트

			//Monster의 5초동안의 이동거리를 계산하는 로직
			MonsterMovingCheckTime += DeltaSeconds;

			//UE_LOG(LogTemp, Warning, TEXT("MonsterMovingCheckTime : %f"), MonsterMovingCheckTime);
			MonsterStartLocation = PathTargets[0]->SubTargetLocation3;
			
			if (OwnerMonster->bIsSpawn == true)
			{
				if (MonsterMovingCheckTime >= 5.f)
				{
					MonsterCurrentLocation = OwnerMonster->GetActorLocation();

					MonsterMovingDistance = FVector::Dist(MonsterStartLocation, MonsterCurrentLocation);

					//UE_LOG(LogTemp, Warning, TEXT("MonsterMoving Dis : %f"), MonsterMovingDistance);

					if (MonsterBeforeMovingDistance - 500 <= MonsterMovingDistance && MonsterMovingDistance <= MonsterBeforeMovingDistance + 500)
					{
						bIsStuck = true;
					}

					if (bIsStuck == true)
					{
						StuckCount++;
					}
					else
					{
						bIsStuck = false; 
					}

					MonsterBeforeMovingDistance = MonsterMovingDistance;
					MonsterMovingCheckTime = 0;
					MonsterMovingDistance = 0;
				}
			}

			//Monster State 업데이트
			uint32 CurrentState = static_cast<uint32>(OwnerMonster->MonsterState);
			BBC->SetValueAsInt(TEXT("MonsterState"), CurrentState);
			BBC->SetValueAsFloat(TEXT("StunTime"), OwnerMonster->MonsterPillarTime);
			//UE_LOG(LogTemp, Warning, TEXT("MonsterState : %d"), CurrentState);

			//속도 한번에 변화시키기
			if (CurrentState == 0)
			{
				OwnerMonster->GetCharacterMovement()->MaxWalkSpeed = OwnerMonster->MonsterSpeed;
				//UE_LOG(LogTemp, Warning, TEXT("Check?"));
			}
			if (CurrentState == 1)
			{
				OwnerMonster->GetCharacterMovement()->MaxWalkSpeed = OwnerMonster->MonsterDetectSpeed;
			}
			if (OwnerMonster->bIsInOil == true)
			{
				OwnerMonster->GetCharacterMovement()->MaxWalkSpeed = OwnerMonster->MonsterSpeed * 0.5;
			}

			//DetectPlayer Update
			if (IsValid(OwnerMonster->DetectPlayer))
			{
				BBC->SetValueAsVector(TEXT("DetectPlayerLocation"), OwnerMonster->DetectPlayer->GetActorLocation());
			}
			/*if (CurrentState == 1 || CurrentState == 2)
			{
				
			}*/

			//DetectTurret Update
			if (CurrentState == 23)
			{
				if (IsValid(OwnerMonster->DetectTurret) && OwnerMonster->DetectTurret != nullptr /*&& !OwnerMonster->DetectTurret->IsPendingKill()*/)
				{
					BBC->SetValueAsVector(TEXT("DetectTurretLocation"), OwnerMonster->DetectTurret->TurretAttackPoint);
				}
			}

			//AttackPlayer Update
			if (IsValid(OwnerMonster->AttackPlayer))
			{
				BBC->SetValueAsVector(TEXT("AttackPlayerLocation"), OwnerMonster->AttackPlayer->GetActorLocation());
			}

			//몬스터 목적지 변수 업데이트
			if (PathTargets.Num() > 0)
			{
				if (OwnerMonster->bIsLastTarget == true || OwnerMonster->MonsterState == EMonsterStateType::IsTowerDetect)
				{
					if (IsValid(Tower))
					{
						//UE_LOG(LogTemp, Warning, TEXT("Change Value!"));
						BBC->SetValueAsVector(TEXT("TargetLocation"), Tower->GetActorLocation());
					}
				}
				else
				{
					if (PathTargets.Num() > 0)
					{
						if (PathMonster)
						{
							BBC->SetValueAsVector(TEXT("TargetLocation"), PathTargets[OwnerMonster->CurrentTarget]->SubTargetLocation3);

						}
						else if (Boss_Phase1)
						{
							if (PathTargets.Num() >= OwnerMonster->CurrentTarget+2)
							{
								BBC->SetValueAsVector(TEXT("TargetLocation"), PathTargets[OwnerMonster->CurrentTarget]->BossTargetLocation);
							}
							else
							{
								if (IsValid(this->Tower))
								{
									//UE_LOG(LogTemp, Warning, TEXT("GoToTower"));

									BBC->SetValueAsVector(TEXT("TargetLocation"), this->Tower->GetActorLocation());
								}
							}
						}
						else if (Boss_Phase2)
						{
							if (PathTargets.Num() >= OwnerMonster->CurrentTarget+2)
							{
								BBC->SetValueAsVector(TEXT("TargetLocation"), PathTargets[OwnerMonster->CurrentTarget]->BossTargetLocation);
							}
							else
							{
								if (IsValid(this->Tower))
								{
									//UE_LOG(LogTemp, Warning, TEXT("GoToTower"));

									BBC->SetValueAsVector(TEXT("TargetLocation"), this->Tower->GetActorLocation());
								}
							}
						}
						else
						{
							if (PathTargets.Num() >= OwnerMonster->CurrentTarget + 2)
							{
								BBC->SetValueAsVector(TEXT("TargetLocation"), PathTargets[OwnerMonster->CurrentTarget]->GetRandomReachPoint());
							}
							else
							{
								//UE_LOG(LogTemp, Warning, TEXT("TargetLocation Else"));
								if (IsValid(this->Tower))
								{
									//UE_LOG(LogTemp, Warning, TEXT("GoToTower"));

									BBC->SetValueAsVector(TEXT("TargetLocation"), this->Tower->GetActorLocation());
								}   							
							}
						}
						//UE_LOG(LogTemp, Warning, TEXT("Tick : %d"), OwnerMonster->CurrentTarget);
						//UE_LOG(LogTemp, Warning, TEXT("Tick Name: %s"), *PathTargets[OwnerMonster->CurrentTarget]->GetName());
					}
				}

			}

			//Debuffer용 목적지 변수 업데이트
			TObjectPtr<ADebuffer> DebufferMonster = Cast<ADebuffer>(OwnerMonster);
			if (IsValid(DebufferMonster))
			{
				if (DebufferTargets.Num() > 0 && DebufferTargets.Num() - 1 >= DebufferMonster->CurrentTarget)
				{
					BBC->SetValueAsVector(TEXT("BufferTargetLocation"), DebufferTargets[DebufferMonster->CurrentTarget]->GetActorLocation());
				}

				if (DebufferMonster->bIsDebuffeStart == true)
				{
					DebufferMonster->MonsterState = EMonsterStateType::IsDebuffing;
					DebufferMonster->ServerRPCDebuffeState();
					UE_LOG(LogTemp, Warning, TEXT("Debuffe State"));
				}
				else
				{
					DebufferMonster->ServerRPCGoToTowerState();
					//UE_LOG(LogTemp, Warning, TEXT("Go To Tower State"));
				}

			}

			if (CurrentState != 24)
			{
				//DetectTime동안 Attack으로 되지 못하면 다시 Tower로 이동한다.
				if (CurrentState == 1 || CurrentState == 5)
				{
					DetectCheckTime += DeltaSeconds;

					//Detect Time over. Tower로 이동한다.
					if (DetectCheckTime >= OwnerMonster->DetectTime && CurrentState != 4)
					{
						DetectCheckTime = 0;
						UAnimInstance* AnimInstance = OwnerMonster->GetMesh()->GetAnimInstance();
						AnimInstance->StopAllMontages(0.0f);
						OwnerMonster->MonsterState = EMonsterStateType::GoToTower;
						//UE_LOG(LogTemp, Warning, TEXT("Go To Tower : Detect Time Over"));
					}

					//Attack 상태로 전환 성공. 
					if (CurrentState == 2 && CurrentState != 4)
					{
						DetectCheckTime = 0;
					}
				}
			}

			

			//Stun상태가 끝나고 다시 타워로 간다.
			if (CurrentState == 10)
			{
				UAnimInstance* AnimInstance = OwnerMonster->GetMesh()->GetAnimInstance();
				AnimInstance->StopAllMontages(0.0f);
				//CurrentState = 0;
				OwnerMonster->MonsterState = EMonsterStateType::GoToTower;
				UE_LOG(LogTemp, Warning, TEXT("StunEnd Go To Tower"));
			}

			//Boss Monster Phase Change
			if (IsValid(Boss_Phase1))
			{
				//UE_LOG(LogTemp, Warning, TEXT("Boss Service Check"));
				//Phase Change Condition1 HP under 50% Check!
				if (Boss_Phase1->MonsterState != EMonsterStateType::IsPhaseChanging)
				{
					if (Boss_Phase1->CurrentHP <= Boss_Phase1->MaxHP / 2.f)
					{
						Boss_Phase1->MonsterState = EMonsterStateType::IsPhaseChange;
						Boss_Phase1->BossChangeLocation = Boss_Phase1->GetActorLocation();
						UE_LOG(LogTemp, Warning, TEXT("BossPhaseChange HP"));
					}
					//Phase Change Conideition2 Trigger 
					if (Boss_Phase1->bIsPhaseChange == true)
					{
						Boss_Phase1->MonsterState = EMonsterStateType::IsPhaseChange;
						Boss_Phase1->BossChangeLocation = Boss_Phase1->GetActorLocation();
						UE_LOG(LogTemp, Warning, TEXT("BossPhaseChagne"));
					}
				}

			}

			//Boss Monster Trap Location Update
			if (IsValid(Boss_Phase2))
			{

				BBC->SetValueAsVector(TEXT("TrapLocation"), Boss_Phase2->DetectTrapLocation);

			}

			//Debuffer상태일때 타이머를 실행시켜주기
			if (OwnerMonster->bIsDebuffeTimerStart == true)
			{
				//UE_LOG(LogTemp, Warning, TEXT("Debuffe Timer Start"));
				//타이머가 안먹는다~
				//OwnerMonster->GetWorldTimerManager().SetTimer(OwnerMonster->DebufferTimerHandle, this, &UMyBTService_CheckState::CheckDebuffeState, OwnerMonster->DebuffeTime, false);

				OwnerMonster->bIsDebuffe = true;
				OwnerMonster->bIsDebuffeTimerStart = false;
			}

			if (OwnerMonster->bIsDebuffe == true)
			{
				DebufferTime += DeltaSeconds;

				if (DebufferTime >= OwnerMonster->DebuffeTime)
				{
					OwnerMonster->bIsDebuffe = false;
					DebufferTime = 0;
					OwnerMonster->ServerRPCEffectDeactive();
					//UE_LOG(LogTemp, Warning, TEXT("Debuffe End"));
				}
			}

			//UE_LOG(LogTemp, Warning, TEXT("Service Working!"))

			//AnimInstance에서 상태 받아와서 일정시간이상 멈춰잇음 다시 타워로 이동시키기
			TObjectPtr<UBaseMonsterAnimInstance>BaseMonsterAnimInstance = Cast<UBaseMonsterAnimInstance>(OwnerMonster->GetMesh()->GetAnimInstance());
			
			if (StuckCount >= 3)
			{
				if (!OwnerMonster->ActorHasTag("Debuffer") &&  !OwnerMonster->ActorHasTag("Boss"))
				{
					if (OwnerMonster->MonsterState != EMonsterStateType::IsTowerDetect && OwnerMonster->MonsterState != EMonsterStateType::IsTowerAttack)
					{
						//bIsStuck = true
						StuckCount = 0;
						//UE_LOG(LogTemp, Warning, TEXT("Monster Stuck! Name : %s "), *OwnerMonster->GetName());

						FVector Direction = OwnerMonster->GetActorForwardVector();   // 현재 이동 방향
						FVector BackwardDirection = -Direction;						 // 반대 방향
						FVector TargetLocation = OwnerMonster->GetActorLocation() + (BackwardDirection * 100);

						//OwnerMonster->TeleportTo(TargetLocation, OwnerMonster->GetActorRotation());
						OwnerMonster->MonsterState = EMonsterStateType::GoToTower;
						//UE_LOG(LogTemp, Warning, TEXT("GoToTower : Stuck End "));
					}
					
				}
				
			}

		}
	}

	
}

void UMyBTService_CheckState::ClearDebufferTarget()
{
	DebufferMonsterAIController->ClearTarget();
	//UE_LOG(LogTemp, Warning, TEXT("Debuffer Target Clear"));
}

void UMyBTService_CheckState::CheckDebuffeState()
{
	TObjectPtr<ABaseMonster> OwnerMonster = Cast<ABaseMonster>(OwnerMonsterAI->GetPawn());
	OwnerMonster->bIsDebuffe = false;
	//UE_LOG(LogTemp, Warning, TEXT("Debuffe End"));
}
