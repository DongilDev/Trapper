// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster/Animation/AnimNotify_BossPhaseChange.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"

#include "Monster/Boss/Boss_Phase1.h"
#include "Monster/Boss/Boss_Phase2.h"
#include "GameElement/Spawner.h"
#include "Framework/TrapperGameMode.h"

void UAnimNotify_BossPhaseChange::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	TObjectPtr<ABoss_Phase1> AttackMonster = Cast<ABoss_Phase1>(MeshComp->GetOwner());

	
	if (IsValid(AttackMonster) && IsValid(AttackMonster->TrapperGameMode))
	{
		//0. Boss가 길을 알수 있게 Target을 미리 저장해둔다 && HP도 저장해둔다
		int BossATarget = AttackMonster->CurrentTarget;
		float BossHP = AttackMonster->CurrentHP;

		//1. Phase1 보스를 시작 자리로 보낸다.
		//AttackMonster->SetActorLocation(AttackMonster->StartPoint);

		//1-1. 자기 자리로 보냇으니 죽음처리를 해준다 -> 이걸 그냥 삭제 로직으로 변경.
		/*AttackMonster->CurrentHP = AttackMonster->MaxHP;
		AttackMonster->CurrentTarget = 0;
		AttackMonster->MonsterState = EMonsterStateType::GoToTower;
		AttackMonster->GetCharacterMovement()->MaxWalkSpeed = AttackMonster->MonsterSpeed;
		AttackMonster->bIsSpawn = false;
		AttackMonster->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);*/
	
		//if (!AttackMonster->TrapperGameMode->Spanwer->BossPhase1List.Find(AttackMonster))
		//{
		//	AttackMonster->TrapperGameMode->Spanwer->BossPhase1List.Add(Cast<ABoss_Phase1>(AttackMonster));
		//	//GetWorld()->GetGameState<ATrapperGameState>()->ChangeMonsterCount(-1);
		//}
		//if (AttackMonster->TrapperGameMode->Spanwer->SpawnMonsterList.Find(AttackMonster) != INDEX_NONE)
		//{
		//	AttackMonster->TrapperGameMode->Spanwer->SpawnMonsterList.Remove(AttackMonster);
		//}
		
		AttackMonster->MonsterState = EMonsterStateType::IsDestroy;

		//2. Phase2 보스를 불러온다
		AttackMonster->TrapperGameMode->Spanwer->BossPhase2List[0]->MonsterState = EMonsterStateType::GoToTower;
		AttackMonster->TrapperGameMode->Spanwer->BossPhase2List[0]->TeleportTo(AttackMonster->BossChangeLocation, AttackMonster->GetActorRotation());
		AttackMonster->TrapperGameMode->Spanwer->BossPhase2List[0]->SetActorEnableCollision(true);
		AttackMonster->TrapperGameMode->Spanwer->BossPhase2List[0]->CurrentTarget = BossATarget;
		AttackMonster->TrapperGameMode->Spanwer->BossPhase2List[0]->bIsSpawn = true;
		AttackMonster->TrapperGameMode->Spanwer->BossPhase2List[0]->bIsBossStage = true;
		AttackMonster->TrapperGameMode->Spanwer->BossPhase2List[0]->GetMesh()->SetVisibility(true);
		AttackMonster->TrapperGameMode->Spanwer->BossPhase2List[0]->CurrentHP = BossHP;
		AttackMonster->TrapperGameMode->Spanwer->BossPhase2List[0]->bIsCDCAttackState = true;
		AttackMonster->TrapperGameMode->Spanwer->SpawnMonsterList.Add(Cast<ABaseMonster>(AttackMonster->TrapperGameMode->Spanwer->BossPhase2List[0]));
		AttackMonster->TrapperGameMode->Spanwer->BossPhase2List.RemoveAt(0);

		//skill start를 false로 해준다.
		AttackMonster->bIsSkillStart = false;

		//HPBar를 업데이트 해준다
		AttackMonster->TrapperGameMode->Spanwer->BossPhase2List[0]->OnRep_IsDamaged();

		//ATrapperGameMode* TrapperGameMode = Cast<ATrapperGameMode>(UGameplayStatics::GetGameMode(GetWorld()));

		//숫자를 1 늘려준다
		AttackMonster->GetWorld()->GetGameState<ATrapperGameState>()->ChangeMonsterCount(+1);
	}
}
