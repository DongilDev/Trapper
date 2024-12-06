// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster/DebufferPathTarget.h"
#include "Components/BoxComponent.h"

#include "Monster/Debuffer.h"

// Sets default values
ADebufferPathTarget::ADebufferPathTarget()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	MainTarget = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MainTarget"));
	MainTarget->SetupAttachment(RootComponent);

	ReachCheck = CreateDefaultSubobject<UBoxComponent>(TEXT("ReachCheck"));
	ReachCheck->SetupAttachment(MainTarget);

	//Add Dynamic
	ReachCheck->OnComponentBeginOverlap.AddDynamic(this, &ADebufferPathTarget::OnOverlapBeginDebufferMonster);
	ReachCheck->OnComponentEndOverlap.AddDynamic(this, &ADebufferPathTarget::OnOverlapEndDebufferMonster);
}

// Called when the game starts or when spawned
void ADebufferPathTarget::BeginPlay()
{
	Super::BeginPlay();
	

}

void ADebufferPathTarget::OnOverlapBeginDebufferMonster(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (HasAuthority())
	{
		//UE_LOG(LogTemp, Warning, TEXT("OverlapBegin"));

		if (IsValid(OtherActor))
		{
			//UE_LOG(LogTemp, Warning, TEXT("OtherActor Valid?"));
			//UE_LOG(LogTemp, Warning, TEXT("Monster Detected : %s"), *OtherActor->GetName());
			
			if (OtherActor->ActorHasTag("Debuffer"))
			{
				TObjectPtr<ADebuffer> OwnerMonster = Cast<ADebuffer>(OtherActor);
				//UE_LOG(LogTemp, Warning, TEXT("Check Debuffer"));

				//그 공간에 잇을 수 잇는 몬스터의 수가 넘어가면
				if (!CheckTarget.Contains(OwnerMonster) && RemainMonsterCount > static_cast<int32>(DebufferMonsterCount))
				{
					CheckTarget.Add(OwnerMonster);
					OwnerMonster->CurrentTarget += 1;
					//UE_LOG(LogTemp, Warning, TEXT("Number of Monsters : %d"), CheckTarget.Num());
				}

				else if (!CheckTarget.Contains(OwnerMonster))
				{
					CheckTarget.Add(OwnerMonster);
					RemainMonsterCount += 1;
					OwnerMonster->bIsDebuffeStart = true;
					//OwnerMonster->CurrentTarget += 1;
					//UE_LOG(LogTemp, Warning, TEXT("CurrentTarget Check Order: %d"), TargetOrder);
				}

				////PathTarget이 마지막인 경우, 다시 처음으로
				//if (OwnerMonster->CurrentTarget >= OwnerMonster->LastTargetNumber)
				//{
				//	OwnerMonster->CurrentTarget = 0;
				//	OwnerMonster->bIsLastDebufferTarget = true;
				//	UE_LOG(LogTemp, Warning, TEXT("LastPathTarget Check"));
				//	//CheckTarget.Remove(OwnerMonster);
				//}

				
			}
		}
	}
}

void ADebufferPathTarget::OnOverlapEndDebufferMonster(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (HasAuthority())
	{
		if (IsValid(OtherActor))
		{
			if (OtherActor->ActorHasTag("Debuffer"))
			{
				UE_LOG(LogTemp, Warning, TEXT("Overlap End Call"));
				TObjectPtr<ADebuffer> OwnerMonster = Cast<ADebuffer>(OtherActor);
				if (OwnerMonster->bIsDead == true)
				{
					RemainMonsterCount--;
					//UE_LOG(LogTemp, Warning, TEXT("Monster Die!"));
				}
			}
		}
	}
	
}





