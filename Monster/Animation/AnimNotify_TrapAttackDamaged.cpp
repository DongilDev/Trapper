// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster/Animation/AnimNotify_TrapAttackDamaged.h"
#include "Engine/DamageEvents.h"
#include "Kismet/GameplayStatics.h"

#include "Monster/Boss/Boss_Phase1.h"
#include "Monster/Boss/Boss_Phase2.h"

void UAnimNotify_TrapAttackDamaged::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	TObjectPtr<class ABoss_Phase1> AttackMonster1 = Cast<ABoss_Phase1>(MeshComp->GetOwner());
	TObjectPtr<class ABoss_Phase2> AttackMonster2 = Cast<ABoss_Phase2>(MeshComp->GetOwner());

	

	if (IsValid(AttackMonster1))
	{
		AController* MyOwnerInstigator = AttackMonster1->GetInstigatorController();
		auto DamageTypeClass = UDamageType::StaticClass();
		FDamageEvent DamageEvent;

		AttackMonster1->bSafeRanged = true;

		if (AttackMonster1->DamagedPlayer1P != nullptr)
		{
			UGameplayStatics::ApplyDamage(Cast<AActor>(AttackMonster1->DamagedPlayer1P), AttackMonster1->TrapAttackDamage, MyOwnerInstigator, AttackMonster1, DamageTypeClass);
		}

		if (AttackMonster1->DamagedPlayer2P != nullptr)
		{
			UGameplayStatics::ApplyDamage(Cast<AActor>(AttackMonster1->DamagedPlayer2P), AttackMonster1->TrapAttackDamage, MyOwnerInstigator, AttackMonster1, DamageTypeClass);
		}
		AttackMonster1->bSafeRanged = false;
	}

	if (IsValid(AttackMonster2))
	{
		AController* MyOwnerInstigator = AttackMonster2->GetInstigatorController();
		auto DamageTypeClass = UDamageType::StaticClass();
		FDamageEvent DamageEvent;

		AttackMonster2->bSafeRanged = true;

		if (AttackMonster2->DamagedPlayer1P != nullptr)
		{
			UGameplayStatics::ApplyDamage(Cast<AActor>(AttackMonster2->DamagedPlayer1P), AttackMonster2->AttackCDCDamage, MyOwnerInstigator, AttackMonster2, DamageTypeClass);
		}

		if (AttackMonster2->DamagedPlayer2P != nullptr)
		{
			UGameplayStatics::ApplyDamage(Cast<AActor>(AttackMonster2->DamagedPlayer2P), AttackMonster2->AttackCDCDamage, MyOwnerInstigator, AttackMonster2, DamageTypeClass);
		}
		AttackMonster2->bSafeRanged = false;
	}
}
