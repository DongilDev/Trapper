
// Fill out your copyright notice in the Description page of Project Settings.


#include "GameElement/Spawner.h"
#include "Kismet/GameplayStatics.h"
#include "EngineUtils.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "Framework/TrapperGameState.h"
#include "Framework/TrapperPlayerController.h"
#include "Monster/BaseMonster.h"
#include "Monster/ADC.h"
#include "Monster/CDC.h"
#include "Monster/DetectedMonster.h"
#include "Monster/Debuffer.h"
#include "Monster/Boss/Boss_Phase1.h"
#include "Monster/Boss/Boss_Phase2.h"
#include "Widget/BossHealthBar.h"
#include "TrapperProject.h"

// Sets default values
ASpawner::ASpawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Spawner = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Spawner"));
	RootComponent = Spawner;

	SpawnPoint1 = CreateDefaultSubobject<USceneComponent>(TEXT("SpawnPoint1"));
	SpawnPoint1->SetupAttachment(RootComponent);

	SpawnPoint2 = CreateDefaultSubobject<USceneComponent>(TEXT("SpawnPoint2"));
	SpawnPoint2->SetupAttachment(RootComponent);

	SpawnPoint3 = CreateDefaultSubobject<USceneComponent>(TEXT("SpawnPoint3"));
	SpawnPoint3->SetupAttachment(RootComponent);

	SpawnPoint4 = CreateDefaultSubobject<USceneComponent>(TEXT("SpawnPoint4"));
	SpawnPoint4->SetupAttachment(RootComponent);

	SpawnPoint5 = CreateDefaultSubobject<USceneComponent>(TEXT("SpawnPoint5"));
	SpawnPoint5->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void ASpawner::BeginPlay()
{
	if (!HasAuthority())
	{
		UE_LOG(LogTemp, Warning, TEXT("Only work in Server"));
		return;
	}

	Super::BeginPlay();

	//Location Setting
	SpawnLocation1 = SpawnPoint1->GetComponentLocation();
	SpawnLocation2 = SpawnPoint2->GetComponentLocation();
	SpawnLocation3 = SpawnPoint3->GetComponentLocation();
	SpawnLocation4 = SpawnPoint4->GetComponentLocation();
	SpawnLocation5 = SpawnPoint5->GetComponentLocation();

	for (TActorIterator<AActor> MyActor(GetWorld()); MyActor; ++MyActor)
	{
		if (MyActor->ActorHasTag("ADC"))
		{
			TObjectPtr<AADC> ADCMonster = Cast<AADC>(*MyActor);
			ADCList.Add(ADCMonster);
		}

		if (MyActor->ActorHasTag("CDC"))
		{
			TObjectPtr<ACDC> CDCMonster = Cast<ACDC>(*MyActor);
			CDCList.Add(CDCMonster);
		}

		if (MyActor->ActorHasTag("Detected"))
		{
			TObjectPtr<ADetectedMonster> DetectedMonster = Cast<ADetectedMonster>(*MyActor);
			DetectedList.Add(DetectedMonster);
		}

		if (MyActor->ActorHasTag("Debuffer"))
		{
			TObjectPtr<ADebuffer> DebufferMonster = Cast<ADebuffer>(*MyActor);
			DebufferList.Add(DebufferMonster);
		}

		if (MyActor->ActorHasTag("BossPhase1"))
		{
			TObjectPtr<ABoss_Phase1> BossPhase1 = Cast<ABoss_Phase1>(*MyActor);
			BossPhase1List.Add(BossPhase1);
			//UE_LOG(LogTemp, Warning, TEXT("BossPhase1 Check"));
		}

		if (MyActor->ActorHasTag("BossPhase2"))
		{
			TObjectPtr<ABoss_Phase2> BossPhase2 = Cast<ABoss_Phase2>(*MyActor);
			BossPhase2List.Add(BossPhase2);
			//UE_LOG(LogTemp, Warning, TEXT("BossPhase2 Check"));
		}
	}

}

void ASpawner::SpawnMonsters(int32 Skeleton, int32 Mymmy, int32 Zombie, int32 Debuffer, int32 Boss)
{
	if (!HasAuthority())
	{
		UE_LOG(LogTemp, Warning, TEXT("Only work in Server"));
		return;
	}

	//Location Init
	SpawnLocation1 = SpawnPoint1->GetComponentLocation();
	SpawnLocation2 = SpawnPoint2->GetComponentLocation();
	SpawnLocation3 = SpawnPoint3->GetComponentLocation();
	SpawnLocation4 = SpawnPoint4->GetComponentLocation();
	SpawnLocation5 = SpawnPoint5->GetComponentLocation();

	SpawnPosition = 0;

	GameState = Cast<ATrapperGameState>(GetWorld()->GetGameState());

	SpawnMymmy(Mymmy);
	SpawnSkeleton(Skeleton);
	SpawnZombie(Zombie);
	SpawnDebuffer(Debuffer);
	SpawnBoss(Boss);
	
}

void ASpawner::SpawnSkeleton(int32 Skeleton)
{
	//CDC
	if (Skeleton != 0)
	{
		if (GameState)
		{
			GameState->ChangeMonsterCount(Skeleton);
			GameState->ChangeRemainingMonsterCount(-Skeleton);
		}

		FRotator SpawnRotation = this->GetActorRotation();
		TObjectPtr<ACDC> SpawnCDC;

		for (int i = 0; i < Skeleton; i++)
		{
			if (SpawnPosition == 0)
			{
				//CDCList[0]->SetActorLocation(SpawnLocation1);
				//UE_LOG(LogTemp, Warning, TEXT("Skeleton 0 Spawn"));
				//CDCList[0]->MonsterState = EMonsterStateType::GoToTower;
				//CDCList[0]->bIsDead = false;
				//ServerRPCTeleportTo(SpawnLocation1);
				//CDCList[0]->TeleportTo(SpawnLocation1, CDCList[0]->GetActorRotation());
				//UE_LOG(LogTemp, Warning, TEXT("Loop : 1 Spawn Location  %f"), SpawnLocation1.Y);

				//Create Skeleton
				SpawnCDC = GetWorld()->SpawnActor<ACDC>(CDCMonsterClass, SpawnLocation1, SpawnRotation);
			}
			if (SpawnPosition == 1)
			{
				//CDCList[0]->SetActorLocation(SpawnLocation2);
				//UE_LOG(LogTemp, Warning, TEXT("Skeleton 1 Spawn"));
				//CDCList[0]->MonsterState = EMonsterStateType::GoToTower;
				//CDCList[0]->bIsDead = false;
				//ServerRPCTeleportTo(SpawnLocation2);
				//CDCList[0]->TeleportTo(SpawnLocation2, CDCList[0]->GetActorRotation());
				//UE_LOG(LogTemp, Warning, TEXT("Loop : 2 Spawn Location  %f"), SpawnLocation2.Y);

				SpawnCDC = GetWorld()->SpawnActor<ACDC>(CDCMonsterClass, SpawnLocation2, SpawnRotation);
			}
			if (SpawnPosition == 2)
			{
				//CDCList[0]->SetActorLocation(SpawnLocation3);
				//UE_LOG(LogTemp, Warning, TEXT("Skeleton 2 Spawn"));
				//CDCList[0]->MonsterState = EMonsterStateType::GoToTower;
				//CDCList[0]->bIsDead = false;
				//ServerRPCTeleportTo(SpawnLocation3);
				//CDCList[0]->TeleportTo(SpawnLocation3, CDCList[0]->GetActorRotation());
				//UE_LOG(LogTemp, Warning, TEXT("Loop : 3 Spawn Location  %f"), SpawnLocation3.Y);

				SpawnCDC = GetWorld()->SpawnActor<ACDC>(CDCMonsterClass, SpawnLocation3, SpawnRotation);
			}
			if (SpawnPosition == 3)
			{
				//CDCList[0]->SetActorLocation(SpawnLocation4);
				//UE_LOG(LogTemp, Warning, TEXT("Skeleton 3 Spawn"));
				//CDCList[0]->MonsterState = EMonsterStateType::GoToTower;
				//CDCList[0]->bIsDead = false;
				//ServerRPCTeleportTo(SpawnLocation4);
				//CDCList[0]->TeleportTo(SpawnLocation4, CDCList[0]->GetActorRotation());
				//UE_LOG(LogTemp, Warning, TEXT("Loop : 4 Spawn Location  %f"), SpawnLocation4.Y);

				SpawnCDC = GetWorld()->SpawnActor<ACDC>(CDCMonsterClass, SpawnLocation4, SpawnRotation);
			}
			if (SpawnPosition == 4)
			{
				//CDCList[0]->SetActorLocation(SpawnLocation5); 
				//UE_LOG(LogTemp, Warning, TEXT("Skeleton 4 Spawn"));
				//CDCList[0]->MonsterState = EMonsterStateType::GoToTower;
				//CDCList[0]->bIsDead = false;
				//ServerRPCTeleportTo(SpawnLocation5);
				//CDCList[0]->TeleportTo(SpawnLocation5, CDCList[0]->GetActorRotation());
				//UE_LOG(LogTemp, Warning, TEXT("Loop : 5 Spawn Location  %f"), SpawnLocation5.Y);

				SpawnCDC = GetWorld()->SpawnActor<ACDC>(CDCMonsterClass, SpawnLocation5, SpawnRotation);
			}

			/*CDCList[0]->SetActorEnableCollision(true);
			CDCList[0]->MonsterState = EMonsterStateType::GoToTower;
			CDCList[0]->bIsSpawn = true;
			CDCList[0]->bIsDead = false;
			CDCList[0]->GetCharacterMovement()->SetMovementMode(MOVE_Walking);*/
			//CDCList[0]->GetMesh()->SetVisibility(true);

			//SpawnCDC->SetActorEnableCollision(true);
			//SpawnCDC->MonsterState = EMonsterStateType::GoToTower;
			//SpawnCDC->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
			
			SpawnCDC->bIsSpawn = true;
			SpawnCDC->bIsDead = false;
			

			SpawnCDC->SetOwner(this);

			SpawnPosition++;
			//SpawnMonsterList.Add(Cast<ABaseMonster>(CDCList[0]));
			//CDCList.RemoveAt(0);

			SpawnMonsterList.Add(Cast<ABaseMonster>(SpawnCDC));
			
			if (SpawnPosition == 5)
			{
				SpawnPosition = 0;
				SpawnLocation1.Y += 50;
				SpawnLocation2.Y += 50;
				SpawnLocation3.Y += 50;
				SpawnLocation4.Y += 50;
				SpawnLocation5.Y += 50;

				//UE_LOG(LogTemp, Warning, TEXT("Loop : Check Y Location %f"), SpawnLocation1.Y);
			}

		}


	}
}

void ASpawner::SpawnMymmy(int32 Mymmy)
{
	//ADC
	if (Mymmy != 0)
	{
		if (GameState)
		{
			GameState->ChangeMonsterCount(Mymmy);
			GameState->ChangeRemainingMonsterCount(-Mymmy);
		}

		FRotator SpawnRotation = this->GetActorRotation();
		TObjectPtr<AADC> SpawnADC;

		for (int i = 0; i < Mymmy; i++)
		{
			if (SpawnPosition == 0)
			{

				//ADCList[0]->SetActorLocation(SpawnLocation1);
				//ADCList[0]->MonsterState = EMonsterStateType::GoToTower;
				//ADCList[0]->bIsDead = false;
				//ServerRPCTeleportToADC(SpawnLocation1);
				//ADCList[0]->TeleportTo(SpawnLocation1, ADCList[0]->GetActorRotation());
				//UE_LOG(LogTemp, Warning, TEXT("Loop : 1 Spawn Location  %f"), SpawnLocation1.Y);

				SpawnADC = GetWorld()->SpawnActor<AADC>(ADCMonsterClass, SpawnLocation1, SpawnRotation);
			}
			if (SpawnPosition == 1)
			{
				//ADCList[0]->SetActorLocation(SpawnLocation2);
				//ADCList[0]->MonsterState = EMonsterStateType::GoToTower;
				//ADCList[0]->bIsDead = false;
				//ServerRPCTeleportToADC(SpawnLocation2);
				//ADCList[0]->TeleportTo(SpawnLocation2, ADCList[0]->GetActorRotation());
				//UE_LOG(LogTemp, Warning, TEXT("Loop : 2 Spawn Location  %f"), SpawnLocation2.Y);

				SpawnADC = GetWorld()->SpawnActor<AADC>(ADCMonsterClass, SpawnLocation2, SpawnRotation);
			}
			if (SpawnPosition == 2)
			{
				//ADCList[0]->SetActorLocation(SpawnLocation3);
				//ADCList[0]->MonsterState = EMonsterStateType::GoToTower;
				//ADCList[0]->bIsDead = false;
				//ServerRPCTeleportToADC(SpawnLocation3);
				//ADCList[0]->TeleportTo(SpawnLocation3, ADCList[0]->GetActorRotation());
				//UE_LOG(LogTemp, Warning, TEXT("Loop : 3 Spawn Location  %f"), SpawnLocation3.Y)
				// 
				SpawnADC = GetWorld()->SpawnActor<AADC>(ADCMonsterClass, SpawnLocation3, SpawnRotation);;
			}
			if (SpawnPosition == 3)
			{
				//ADCList[0]->SetActorLocation(SpawnLocation4);
				//ADCList[0]->MonsterState = EMonsterStateType::GoToTower;
				//ADCList[0]->bIsDead = false;
				//ServerRPCTeleportToADC(SpawnLocation4);
				//ADCList[0]->TeleportTo(SpawnLocation4, ADCList[0]->GetActorRotation());
				//UE_LOG(LogTemp, Warning, TEXT("Loop : 4 Spawn Location  %f"), SpawnLocation4.Y);

				SpawnADC = GetWorld()->SpawnActor<AADC>(ADCMonsterClass, SpawnLocation4, SpawnRotation);
			}
			if (SpawnPosition == 4)
			{
				//ADCList[0]->SetActorLocation(SpawnLocation5);
				//ADCList[0]->MonsterState = EMonsterStateType::GoToTower;
				//ADCList[0]->bIsDead = false;
				//ServerRPCTeleportToADC(SpawnLocation5);
				//ADCList[0]->TeleportTo(SpawnLocation5, ADCList[0]->GetActorRotation());
				//UE_LOG(LogTemp, Warning, TEXT("Loop : 5 Spawn Location  %f"), SpawnLocation5.Y);

				SpawnADC = GetWorld()->SpawnActor<AADC>(ADCMonsterClass, SpawnLocation5, SpawnRotation);
			}

			//ADCList[0]->SetActorEnableCollision(true);
			
			//ADCList[0]->bIsSpawn = true;
			//ADCList[0]->bIsDead = false;
			////ADCList[0]->GetMesh()->SetVisibility(true);
			//SpawnMonsterList.Add(Cast<ABaseMonster>(ADCList[0]));
			//ADCList.RemoveAt(0);
			//UE_LOG(LogTemp, Warning, TEXT("Spawn Monster Name : %s"), *ADCList[0].GetName());

			SpawnADC->bIsSpawn = true;
			SpawnADC->bIsDead = false;

			SpawnADC->SetOwner(this);
			
			SpawnPosition++;

			SpawnMonsterList.Add(Cast<ABaseMonster>(SpawnADC));

			if (SpawnPosition == 5)
			{
				SpawnPosition = 0;
				SpawnLocation1.Y += 20;
				SpawnLocation2.Y += 20;
				SpawnLocation3.Y += 20;
				SpawnLocation4.Y += 20;
				SpawnLocation5.Y += 20;
			}

		}

	}

}

void ASpawner::SpawnZombie(int32 Zombie)
{
	//Detect
	if (Zombie != 0)
	{
		if (GameState)
		{
			GameState->ChangeMonsterCount(Zombie);
			GameState->ChangeRemainingMonsterCount(-Zombie);
		}

		FRotator SpawnRotation = this->GetActorRotation();
		TObjectPtr<ADetectedMonster> SpawnDetected;

		for (int i = 0; i < Zombie; i++)
		{
			if (SpawnPosition == 0)
			{
				//DetectedList[0]->SetActorLocation(SpawnLocation1);
				//UE_LOG(LogTemp, Warning, TEXT("Skeleton 0 Spawn"));
				//DetectedList[0]->MonsterState = EMonsterStateType::GoToTower;
				//DetectedList[0]->bIsDead = false;
				//ServerRPCTeleportToDetect(SpawnLocation1);
				//DetectedList[0]->TeleportTo(SpawnLocation1, DetectedList[0]->GetActorRotation());

				SpawnDetected = GetWorld()->SpawnActor<ADetectedMonster>(DetectedMonsterClass, SpawnLocation1, SpawnRotation);
			}
			if (SpawnPosition == 1)
			{
				//DetectedList[0]->SetActorLocation(SpawnLocation2);
				//UE_LOG(LogTemp, Warning, TEXT("Skeleton 1 Spawn"));
				//DetectedList[0]->MonsterState = EMonsterStateType::GoToTower;
				//DetectedList[0]->bIsDead = false;
				//ServerRPCTeleportToDetect(SpawnLocation2);
				//DetectedList[0]->TeleportTo(SpawnLocation2, DetectedList[0]->GetActorRotation());

				SpawnDetected = GetWorld()->SpawnActor<ADetectedMonster>(DetectedMonsterClass, SpawnLocation2, SpawnRotation);
			}
			if (SpawnPosition == 2)
			{
				//DetectedList[0]->SetActorLocation(SpawnLocation3);
				//UE_LOG(LogTemp, Warning, TEXT("Skeleton 2 Spawn"));
				//DetectedList[0]->MonsterState = EMonsterStateType::GoToTower;
				//DetectedList[0]->bIsDead = false;
				//ServerRPCTeleportToDetect(SpawnLocation3);
				//DetectedList[0]->TeleportTo(SpawnLocation3, DetectedList[0]->GetActorRotation());

				SpawnDetected = GetWorld()->SpawnActor<ADetectedMonster>(DetectedMonsterClass, SpawnLocation3, SpawnRotation);
			}
			if (SpawnPosition == 3)
			{
				//DetectedList[0]->SetActorLocation(SpawnLocation4);
				//UE_LOG(LogTemp, Warning, TEXT("Skeleton 3 Spawn"));
				//DetectedList[0]->MonsterState = EMonsterStateType::GoToTower;
				//DetectedList[0]->bIsDead = false;
				//ServerRPCTeleportToDetect(SpawnLocation4);
				//DetectedList[0]->TeleportTo(SpawnLocation4, DetectedList[0]->GetActorRotation());

				SpawnDetected = GetWorld()->SpawnActor<ADetectedMonster>(DetectedMonsterClass, SpawnLocation4, SpawnRotation);
			}
			if (SpawnPosition == 4)
			{
				//DetectedList[0]->SetActorLocation(SpawnLocation5);
				//UE_LOG(LogTemp, Warning, TEXT("Skeleton 4 Spawn"));
				//DetectedList[0]->MonsterState = EMonsterStateType::GoToTower;
				//DetectedList[0]->bIsDead = false;
				//ServerRPCTeleportToDetect(SpawnLocation5);
				//DetectedList[0]->TeleportTo(SpawnLocation5, DetectedList[0]->GetActorRotation());

				SpawnDetected = GetWorld()->SpawnActor<ADetectedMonster>(DetectedMonsterClass, SpawnLocation5, SpawnRotation);
			}

			//DetectedList[0]->SetActorEnableCollision(true);
			//DetectedList[0]->bIsSpawn = true;
			////DetectedList[0]->GetMesh()->SetVisibility(true);
			//SpawnPosition++;
			//SpawnMonsterList.Add(Cast<ABaseMonster>(DetectedList[0]));
			//DetectedList.RemoveAt(0);

			SpawnDetected->bIsSpawn = true;
			SpawnDetected->bIsDead = false;

			SpawnDetected->SetOwner(this);

			SpawnPosition++;

			SpawnMonsterList.Add(Cast<ABaseMonster>(SpawnDetected));

			if (SpawnPosition == 5)
			{
				SpawnPosition = 0;
				SpawnLocation1.Y += 10;
				SpawnLocation2.Y += 10;
				SpawnLocation3.Y += 10;
				SpawnLocation4.Y += 10;
				SpawnLocation5.Y += 10;
			}

		}


	}
}

void ASpawner::SpawnDebuffer(int32 Debuffer)
{
	//Debuffer
	if (Debuffer != 0)
	{
		if (GameState)
		{
			GameState->ChangeMonsterCount(Debuffer);
			GameState->ChangeRemainingMonsterCount(-Debuffer);
		}

		FRotator SpawnRotation = this->GetActorRotation();
		TObjectPtr<ADebuffer> SpawnDebuffer;

		for (int i = 0; i < Debuffer; i++)
		{
			if (SpawnPosition == 0)
			{
				//DetectedList[0]->SetActorLocation(SpawnLocation1);
				//UE_LOG(LogTemp, Warning, TEXT("Skeleton 0 Spawn"));
				//DebufferList[0]->MonsterState = EMonsterStateType::GoToTower;
				//DebufferList[0]->bIsDead = false;
				//ServerRPCTeleportToDebuffer(SpawnLocation1);
				//DebufferList[0]->TeleportTo(SpawnLocation1, DebufferList[0]->GetActorRotation());

				SpawnDebuffer = GetWorld()->SpawnActor<ADebuffer>(DebufferMonsterClass, SpawnLocation1, SpawnRotation);
			}
			if (SpawnPosition == 1)
			{
				//DetectedList[0]->SetActorLocation(SpawnLocation2);
				//UE_LOG(LogTemp, Warning, TEXT("Skeleton 1 Spawn"));
				//DebufferList[0]->MonsterState = EMonsterStateType::GoToTower;
				//DebufferList[0]->bIsDead = false;
				//ServerRPCTeleportToDebuffer(SpawnLocation2);
				//DebufferList[0]->TeleportTo(SpawnLocation2, DebufferList[0]->GetActorRotation());

				SpawnDebuffer = GetWorld()->SpawnActor<ADebuffer>(DebufferMonsterClass, SpawnLocation2, SpawnRotation);
			}
			if (SpawnPosition == 2)
			{
				//DetectedList[0]->SetActorLocation(SpawnLocation3);
				//UE_LOG(LogTemp, Warning, TEXT("Skeleton 2 Spawn"));
				//DebufferList[0]->MonsterState = EMonsterStateType::GoToTower;
				//DebufferList[0]->bIsDead = false;
				//ServerRPCTeleportToDebuffer(SpawnLocation3);
				//DebufferList[0]->TeleportTo(SpawnLocation3, DebufferList[0]->GetActorRotation());

				SpawnDebuffer = GetWorld()->SpawnActor<ADebuffer>(DebufferMonsterClass, SpawnLocation3, SpawnRotation);
			}
			if (SpawnPosition == 3)
			{
				//DetectedList[0]->SetActorLocation(SpawnLocation4);
				//UE_LOG(LogTemp, Warning, TEXT("Skeleton 3 Spawn"));
				//DebufferList[0]->MonsterState = EMonsterStateType::GoToTower;
				//DebufferList[0]->bIsDead = false;
				//ServerRPCTeleportToDebuffer(SpawnLocation4);
				//DebufferList[0]->TeleportTo(SpawnLocation4, DebufferList[0]->GetActorRotation());

				SpawnDebuffer = GetWorld()->SpawnActor<ADebuffer>(DebufferMonsterClass, SpawnLocation4, SpawnRotation);
			}
			if (SpawnPosition == 4)
			{
				//DetectedList[0]->SetActorLocation(SpawnLocation5);
				//UE_LOG(LogTemp, Warning, TEXT("Skeleton 4 Spawn"));
				//DebufferList[0]->MonsterState = EMonsterStateType::GoToTower;
				//DebufferList[0]->bIsDead = false;
				//ServerRPCTeleportToDebuffer(SpawnLocation5);
				//DebufferList[0]->TeleportTo(SpawnLocation5, DebufferList[0]->GetActorRotation());

				SpawnDebuffer = GetWorld()->SpawnActor<ADebuffer>(DebufferMonsterClass, SpawnLocation5, SpawnRotation);
			}

			//DebufferList[0]->SetActorEnableCollision(true);
			//DebufferList[0]->bIsSpawn = true;
			////DebufferList[0]->GetMesh()->SetVisibility(true);
			//SpawnPosition++;
			//SpawnMonsterList.Add(Cast<ABaseMonster>(DebufferList[0]));
			//DebufferList.RemoveAt(0);

			SpawnDebuffer->bIsSpawn = true;
			SpawnDebuffer->bIsDead = false;

			SpawnDebuffer->SetOwner(this);

			SpawnPosition++;

			SpawnMonsterList.Add(Cast<ABaseMonster>(SpawnDebuffer));

			if (SpawnPosition == 5)
			{
				SpawnPosition = 0;
				SpawnLocation1.Y += 10;
				SpawnLocation2.Y += 10;
				SpawnLocation3.Y += 10;
				SpawnLocation4.Y += 10;
				SpawnLocation5.Y += 10;
			}

		}


	}
}

//Boss
void ASpawner::SpawnBoss(int32 Boss)
{
	if (Boss != 0)
	{
		if (GameState)
		{
			GameState->ChangeMonsterCount(Boss);
			GameState->ChangeRemainingMonsterCount(-Boss);
		}

		FRotator SpawnRotation = this->GetActorRotation();
		TObjectPtr<ABoss_Phase1> SpawnBossPhase1;

		for (int i = 0; i < Boss; i++)
		{
			if (SpawnPosition == 0)
			{
				//CDCList[0]->SetActorLocation(SpawnLocation1);
				//UE_LOG(LogTemp, Warning, TEXT("Skeleton 0 Spawn"));
				//BossPhase1List[0]->MonsterState = EMonsterStateType::GoToTower;
				//BossPhase1List[0]->TeleportTo(SpawnLocation1, BossPhase1List[0]->GetActorRotation());
				//UE_LOG(LogTemp, Warning, TEXT("Loop : 1 Spawn Location  %f"), SpawnLocation1.Y);

				SpawnBossPhase1 = GetWorld()->SpawnActor<ABoss_Phase1>(Boss_Phase1Class, SpawnLocation1, SpawnRotation);
			}
			if (SpawnPosition == 1)
			{
				//CDCList[0]->SetActorLocation(SpawnLocation2);
				//UE_LOG(LogTemp, Warning, TEXT("Skeleton 1 Spawn"));
				//BossPhase1List[0]->MonsterState = EMonsterStateType::GoToTower;
				//BossPhase1List[0]->TeleportTo(SpawnLocation2, BossPhase1List[0]->GetActorRotation());
				//UE_LOG(LogTemp, Warning, TEXT("Loop : 2 Spawn Location  %f"), SpawnLocation2.Y);

				SpawnBossPhase1 = GetWorld()->SpawnActor<ABoss_Phase1>(Boss_Phase1Class, SpawnLocation2, SpawnRotation);
			}
			if (SpawnPosition == 2)
			{
				//CDCList[0]->SetActorLocation(SpawnLocation3);
				//UE_LOG(LogTemp, Warning, TEXT("Skeleton 2 Spawn"));
				//BossPhase1List[0]->MonsterState = EMonsterStateType::GoToTower;
				//BossPhase1List[0]->TeleportTo(SpawnLocation3, BossPhase1List[0]->GetActorRotation());
				//UE_LOG(LogTemp, Warning, TEXT("Loop : 3 Spawn Location  %f"), SpawnLocation3.Y);

				SpawnBossPhase1 = GetWorld()->SpawnActor<ABoss_Phase1>(Boss_Phase1Class, SpawnLocation3, SpawnRotation);
			}
			if (SpawnPosition == 3)
			{
				//CDCList[0]->SetActorLocation(SpawnLocation4);
				//UE_LOG(LogTemp, Warning, TEXT("Skeleton 3 Spawn"));
				//BossPhase1List[0]->MonsterState = EMonsterStateType::GoToTower;
				//BossPhase1List[0]->TeleportTo(SpawnLocation4, BossPhase1List[0]->GetActorRotation());
				//UE_LOG(LogTemp, Warning, TEXT("Loop : 4 Spawn Location  %f"), SpawnLocation4.Y);

				SpawnBossPhase1 = GetWorld()->SpawnActor<ABoss_Phase1>(Boss_Phase1Class, SpawnLocation4, SpawnRotation);
			}
			if (SpawnPosition == 4)
			{
				//CDCList[0]->SetActorLocation(SpawnLocation5); 
				//UE_LOG(LogTemp, Warning, TEXT("Skeleton 4 Spawn"));
				//BossPhase1List[0]->MonsterState = EMonsterStateType::GoToTower;
				//BossPhase1List[0]->TeleportTo(SpawnLocation5, BossPhase1List[0]->GetActorRotation());
				//UE_LOG(LogTemp, Warning, TEXT("Loop : 5 Spawn Location  %f"), SpawnLocation5.Y);

				SpawnBossPhase1 = GetWorld()->SpawnActor<ABoss_Phase1>(Boss_Phase1Class, SpawnLocation5, SpawnRotation);
			}

			//SpawnPosition++;
			//BossPhase1List[0]->bIsSpawn = true;
			////BossPhase1List[0]->GetMesh()->SetVisibility(true);
			//BossPhase1List[0]->bIsBossStage = true;
			//SpawnMonsterList.Add(Cast<ABaseMonster>(BossPhase1List[0]));
			//BossPhase1List.RemoveAt(0);

			SpawnBossPhase1->bIsSpawn = true;
			SpawnBossPhase1->bIsDead = false;

			SpawnBossPhase1->SetOwner(this);

			SpawnPosition++;

			SpawnMonsterList.Add(Cast<ABaseMonster>(SpawnBossPhase1));

			//ShowUI
			SpawnBossPhase1->ServerRPCShowBossHPBar();

			if (SpawnPosition == 5)
			{
				SpawnPosition = 0;
				SpawnLocation1.Y += 50;
				SpawnLocation2.Y += 50;
				SpawnLocation3.Y += 50;
				SpawnLocation4.Y += 50;
				SpawnLocation5.Y += 50;

				//UE_LOG(LogTemp, Warning, TEXT("Loop : Check Y Location %f"), SpawnLocation1.Y);
			}

		}
	}
}

void ASpawner::ServerRPCTeleportToDetect_Implementation(FVector SpawnLocation)
{
	MulticastRPCTeleportToDetect(SpawnLocation);
}

void ASpawner::MulticastRPCTeleportToDetect_Implementation(FVector SpawnLocation)
{
	DetectedList[0]->TeleportTo(SpawnLocation, DetectedList[0]->GetActorRotation());
}

void ASpawner::ServerRPCTeleportToADC_Implementation(FVector SpawnLocation)
{
	MulticastRPCTeleportToADC(SpawnLocation);
}

void ASpawner::MulticastRPCTeleportToADC_Implementation(FVector SpawnLocation)
{
	ADCList[0]->TeleportTo(SpawnLocation, ADCList[0]->GetActorRotation());
}

void ASpawner::ServerRPCTeleportTo_Implementation(FVector SpawnLocation)
{
	MulticastRPCTeleportTo(SpawnLocation);
}

void ASpawner::MulticastRPCTeleportTo_Implementation(FVector SpawnLocation)
{
	CDCList[0]->TeleportTo(SpawnLocation, CDCList[0]->GetActorRotation());
}

void ASpawner::ServerRPCTeleportToDebuffer_Implementation(FVector SpawnLocation)
{
	MulticastRPCTeleportToDebuffer(SpawnLocation);
}

void ASpawner::MulticastRPCTeleportToDebuffer_Implementation(FVector SpawnLocation)
{
	DebufferList[0]->TeleportTo(SpawnLocation, DebufferList[0]->GetActorRotation());
}

void ASpawner::UpadateSpeed()
{
	//ADC
	for (TObjectPtr<AADC> Element : ADCList)
	{
		Element->ChangeMonsterSpeed();
	}

	//CDC
	for (TObjectPtr<ACDC> Element : CDCList)
	{
		Element->ChangeMonsterSpeed();
	}

	//Detected
	for (TObjectPtr<ADetectedMonster> Element : DetectedList)
	{
		Element->ChangeMonsterSpeed();
	}

	//Debuffer
	for (TObjectPtr<ADebuffer> Element : DebufferList)
	{
		Element->ChangeMonsterSpeed();
	}

}

void ASpawner::UpdateHealth()
{
	//ADC
	for (TObjectPtr<AADC> Element : ADCList)
	{
		Element->ChangeMonsterHealth();
	}

	//CDC
	for (TObjectPtr<ACDC> Element : CDCList)
	{
		Element->ChangeMonsterHealth();
	}

	//Detected
	for (TObjectPtr<ADetectedMonster> Element : DetectedList)
	{
		Element->ChangeMonsterHealth();
	}

	//Debuffer
	for (TObjectPtr<ADebuffer> Element : DebufferList)
	{
		Element->ChangeMonsterHealth();
	}
}

void ASpawner::UpdateDamage()
{

	//ADC
	for (TObjectPtr<AADC> Element : ADCList)
	{
		Element->ChangeADCDamage();
	}

	//CDC
	for (TObjectPtr<ACDC> Element : CDCList)
	{
		Element->ChangeCDCDamage();
	}

	//Detected
	for (TObjectPtr<ADetectedMonster> Element : DetectedList)
	{
		Element->ChangeDetectedDamage();
	}
}
