// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster/BossDestroyTrigger.h"
#include "Components/BoxComponent.h"

#include "Monster/Boss/Boss_Phase1.h"
#include "Monster/Boss/Boss_Phase2.h"

// Sets default values
ABossDestroyTrigger::ABossDestroyTrigger()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
	TriggerBox->SetupAttachment(RootComponent);

	//Add Dynamic
	TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &ABossDestroyTrigger::OnOverlapBeginTriggerBox);
}

// Called when the game starts or when spawned
void ABossDestroyTrigger::BeginPlay()
{
	Super::BeginPlay();
	
}

void ABossDestroyTrigger::OnOverlapBeginTriggerBox(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (HasAuthority())
	{
		if (OtherActor->ActorHasTag("BossPhase1"))
		{
			TObjectPtr<ABoss_Phase1> Boss = Cast<ABoss_Phase1>(OtherActor);
			Boss->MonsterState = EMonsterStateType::IsRockAttack;

			if (this->ActorHasTag("Trigger0"))
			{
				Boss->ArchNum = 0;
			}
			if (this->ActorHasTag("Trigger1"))
			{
				Boss->ArchNum = 1;
			}

		}

		if (OtherActor->ActorHasTag("BossPhase2"))
		{
			TObjectPtr<ABoss_Phase2> Boss = Cast<ABoss_Phase2>(OtherActor);
			Boss->MonsterState = EMonsterStateType::IsRockAttack;

			if (this->ActorHasTag("Trigger0"))
			{
				Boss->ArchNum = 0;
			}
			if (this->ActorHasTag("Trigger1"))
			{
				Boss->ArchNum = 1;
			}
		}
	}

}

