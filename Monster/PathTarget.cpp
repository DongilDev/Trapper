// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster/PathTarget.h"
#include "Components/BoxComponent.h"
#include "Math/UnrealMathUtility.h"

#include "Monster/BaseMonster.h"

// Sets default values
APathTarget::APathTarget()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	MainTarget = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MainTarget"));
	MainTarget->SetupAttachment(RootComponent);

	ReachCheck = CreateDefaultSubobject<UBoxComponent>(TEXT("ReachCheck"));
	ReachCheck->SetupAttachment(MainTarget);

	SubTarget1 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SubTarget1"));
	SubTarget1->SetupAttachment(MainTarget);

	SubTarget2 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SubTarget2"));
	SubTarget2->SetupAttachment(MainTarget);

	SubTarget3 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SubTarget3"));
	SubTarget3->SetupAttachment(MainTarget);

	SubTarget4 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SubTarget4"));
	SubTarget4->SetupAttachment(MainTarget);

	SubTarget5 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SubTarget5"));
	SubTarget5->SetupAttachment(MainTarget);

	BossTarget = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BossTarget"));
	BossTarget->SetupAttachment(MainTarget);

	//Add Dynamic
	ReachCheck->OnComponentBeginOverlap.AddDynamic(this, &APathTarget::OnOverlapBeginMonster);
}

// Called when the game starts or when spawned
void APathTarget::BeginPlay()
{
	Super::BeginPlay();
	
	SubTargetLocation1 = SubTarget1->GetComponentLocation();
	SubTargetLocation2 = SubTarget2->GetComponentLocation();
	SubTargetLocation3 = SubTarget3->GetComponentLocation();
	SubTargetLocation4 = SubTarget4->GetComponentLocation();
	SubTargetLocation5 = SubTarget5->GetComponentLocation();

	BossTargetLocation = BossTarget->GetComponentLocation();

	//UE_LOG(LogTemp, Warning, TEXT("Location1 : %f %f %f"), SubTargetLocation1.X, SubTargetLocation1.Y, SubTargetLocation1.Z);

}

void APathTarget::OnOverlapBeginMonster(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (HasAuthority())
	{
		if (IsValid(OtherActor))
		{
			if (OtherActor->ActorHasTag("Monster"))
			{
				ABaseMonster* OwnerMonster = Cast<ABaseMonster>(OtherActor);

				if (!CheckTarget.Contains(OwnerMonster))
				{
					//UE_LOG(LogTemp, Warning, TEXT("Actor Name : [%s]"), *OwnerMonster->GetName());
					CheckTarget.Add(OwnerMonster);
					OwnerMonster->CurrentTarget += 1;

				}
				//PathTarget이 마지막인 경우, 
				
				if(OwnerMonster->CurrentTarget == OwnerMonster->LastTargetNumber)
				{
					UE_LOG(LogTemp, Warning, TEXT("Last!"));
					OwnerMonster->bIsLastTarget = true;
				}
				else
				{
					//UE_LOG(LogTemp, Warning, TEXT("Actor Name : [%s]"), *OwnerMonster->GetName());
					
				}
			}
		}
	}
	
}

FVector APathTarget::GetRandomReachPoint()
{
	float RandomNum = FMath::FRandRange(1.0, 6.0);

	if (RandomNum < 2)
	{
		return SubTargetLocation1;
	}
	else if (RandomNum < 3)
	{
		return SubTargetLocation2;
	}
	else if (RandomNum < 4)
	{
		return SubTargetLocation3;
	}
	else if (RandomNum < 5)
	{
		return SubTargetLocation4;
	}
	else
	{
		return SubTargetLocation5;
	}
}

