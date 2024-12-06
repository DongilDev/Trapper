// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster/Boss/Boss_Phase2.h"
#include "Components/BoxComponent.h"
#include "Components/WidgetComponent.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "Engine/World.h"
#include "Engine/DamageEvents.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "EngineUtils.h"

#include "Monster/Weapon/BossSword.h"
#include "Monster/Weapon/BossThrowSword.h"
#include "Trap/TrapBase.h"
#include "GameElement/Spawner.h"
#include "Framework/TrapperGameMode.h"
#include "Framework/TrapperGameState.h"
#include "Monster/DetectedMonster.h"
#include "Data/MonsterDataTables.h"
#include "GameElement/MagneticPillar.h"
#include "Character/TrapperPlayer.h"
#include "TrapperProject.h"
#include "GameElement/Arch.h"


ABoss_Phase2::ABoss_Phase2()
	: TrapSkillCoolTime(5)
	, bIsTrapAttackState(true)
	, MaxHitGauge(100)
	, CDCSkillCoolTime(10)
	, ADCSkillCoolTime(10)
	, bIsCDCAttackState(true)
	, bIsADCAttackState(true)
{
	CloseAttackSight = CreateDefaultSubobject<UBoxComponent>(TEXT("CloseAttackSight"));
	CloseAttackSight->SetupAttachment(RootComponent);
	
	TrapAttackSight = CreateDefaultSubobject<UBoxComponent>(TEXT("TrapAttackSight"));
	TrapAttackSight->SetupAttachment(RootComponent);

	SwordSpawn = CreateDefaultSubobject<USceneComponent>(TEXT("Sword Spawn"));
	SwordSpawn->SetupAttachment(RootComponent);

	//Add Dynamic
	CloseAttackSight->OnComponentBeginOverlap.AddDynamic(this, &ABoss_Phase2::OnOverlapBeginCloseSight);
	CloseAttackSight->OnComponentEndOverlap.AddDynamic(this, &ABoss_Phase2::OnOverlapEndCloseSight);
	TrapAttackSight->OnComponentBeginOverlap.AddDynamic(this, &ABoss_Phase2::OnOverlapBeginTrapSight);
	TrapAttackSight->OnComponentEndOverlap.AddDynamic(this, &ABoss_Phase2::OnOverlapEndTrapSight);
}

void ABoss_Phase2::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		BossSword = GetWorld()->SpawnActor<ABossSword>(BossSwordClass);
		BossSword->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, TEXT("Sword"));
		BossSword->SetOwner(this);
	}

	///SetData
	//HP
	if (MonsterInfoTable[5].HP)
	{
		MaxHP = MonsterInfoTable[5].HP;
		CurrentHP = MaxHP;
	}
	//Attack Damage
	if (MonsterInfoTable[5].Attack)
	{
		AttackCDCDamage = MonsterInfoTable[5].Attack;
	}
	//Speed
	if (MonsterInfoTable[5].MoveSpeed)
	{
		MonsterSpeed = MonsterInfoTable[5].MoveSpeed;
		GetCharacterMovement()->MaxWalkSpeed = MonsterSpeed;
	}
	if (MonsterInfoTable[5].ChaseSpeed)
	{
		MonsterDetectSpeed = MonsterInfoTable[5].ChaseSpeed;
	}
	if (MonsterInfoTable[5].ChaseSpeed)
	{
		MonsterEffectSpeed = MonsterInfoTable[5].EffectSpeed;
	}
	//DetectTime
	if (MonsterInfoTable[5].LocationTimer)
	{
		DetectTime = MonsterInfoTable[5].LocationTimer;
	}
	//Item
	if (MonsterInfoTable[5].SpawnAmount)
	{
		ItemMinDropNumber = MonsterInfoTable[5].SpawnAmount - 1;
		ItemMaxDropNumber = MonsterInfoTable[5].SpawnAmount;
	}
	
	//Animation Time Add
	if (MonsterInfoTable[5].TrapCoolTime)
	{
		TrapSkillCoolTime  = MonsterInfoTable[5].TrapCoolTime;
	}
	
	if (MonsterInfoTable[5].CDCCoolTime)
	{
		CDCSkillCoolTime = MonsterInfoTable[5].CDCCoolTime;
	}

	//SkillCoolTime
	if (MonsterInfoTable[5].TrapCoolTime)
	{
		TrapSkillCoolTime = MonsterInfoTable[5].TrapCoolTime;
	}

	if (MonsterInfoTable[5].CDCCoolTime)
	{
		CDCSkillCoolTime = MonsterInfoTable[5].CDCCoolTime;
	}

	if (MonsterInfoTable[5].ADCCoolTime)
	{
		ADCSkillCoolTime = MonsterInfoTable[5].ADCCoolTime;
	}

	//Pillar Speed
	if (MonsterInfoTable[5].PillarSpeed)
	{
		MonsterPillarSpeed = MonsterInfoTable[5].PillarSpeed;
	}
	//Pillar StunTime
	if (MonsterInfoTable[5].PillarStunTime)
	{
		MonsterPillarTime = MonsterInfoTable[5].PillarStunTime;
	}

	//Init Gauge
	HitGauge = MaxHitGauge;

	//Init AttackState
	bIsTrapAttackState = true;
	bIsADCAttackState = true;
	bIsCDCAttackState = true;

	//ADCSkillCoolTime = 10;
	//CDCSkillCoolTime = 10;
	//TrapSkillCoolTime = 5;

	//Set Material
	BossMaterial0 = this->GetMesh()->CreateDynamicMaterialInstance(0);
	BossMaterial1 = this->GetMesh()->CreateDynamicMaterialInstance(1);

	if (HasAuthority())
	{
		for (TActorIterator<AActor> MyActor(this->GetWorld()); MyActor; ++MyActor)
		{
			if (MyActor->ActorHasTag("Arch0"))
			{
				//AActor CurrentActor = MyActor;
				Boss2Arch0 = Cast<AArch>(*MyActor);
				//UE_LOG(LogTemp, Warning, TEXT("Find Arch0"));
			}

			if (MyActor->ActorHasTag("Arch1"))
			{
				//AActor* CurrentActor = *MyActor;
				Boss2Arch1 = Cast<AArch>(*MyActor);
				//UE_LOG(LogTemp, Warning, TEXT("Find Arch1"));
			}
		}
	}
}

void ABoss_Phase2::AttackHitCheck()
{
	if (IsValid(BossSword))
	{
		//UE_LOG(LogTemp, Warning, TEXT("Sword Valid"));
		TArray<FHitResult> HitResults;
		FCollisionQueryParams Params(SCENE_QUERY_STAT(Attack), false, this);
		const FVector Start = BossSword->BossSwordMesh->GetSocketLocation("Start");
		const FVector End = BossSword->BossSwordMesh->GetSocketLocation("End");

		UE_LOG(LogTemp, Warning, TEXT("AttackHitCheck?"));

		bool HitDetected = GetWorld()->SweepMultiByChannel
		(
			HitResults,
			Start,
			End,
			FQuat::Identity,
			ECollisionChannel::ECC_WorldDynamic,
			BossSword->BossSwordMesh->GetCollisionShape(),
			Params
		);

		//UE_LOG(LogTemp, Warning, TEXT("Hit results : %s"), *OutHitResult.GetActor()->GetName());

		if (HitDetected)
		{
			//UE_LOG(LogTemp, Warning, TEXT("OutHitResult : %s"), *OutHitResult.GetActor()->GetName());
			for (const FHitResult& Hit : HitResults)
			{
				if (IsValid(Hit.GetActor()))
				{
					if (!Hit.GetActor()->ActorHasTag("Monster"))
					{
						if (Hit.GetActor()->ActorHasTag("BossAttackableTrap"))
						{
							//UE_LOG(LogTemp, Warning, TEXT("TrapAttack"));
							if (!Hit.GetComponent()->ComponentHasTag("GunpowderTrigger"))
							{
								TObjectPtr<ATrapBase> DetectTrap = Cast<ATrapBase>(Hit.GetActor());
								if (IsValid(DetectTrap))
								{
									DetectTrap->OnAttackedByBoss();
									//bIsAttackEnd = true;
									UE_LOG(LogTemp, Warning, TEXT("Remove Trap : %s"), *DetectTrap->GetName());
								}
							}
						}
						else if (Hit.GetActor()->ActorHasTag("MagneticPillar"))
						{
							//UE_LOG(LogTemp, Warning, TEXT("TrapAttack"));
							TObjectPtr<AMagneticPillar> DetectPillar = Cast<AMagneticPillar>(Hit.GetActor());

							if (IsValid(DetectPillar))
							{
								DetectPillar->Break();
								//bIsAttackEnd = true;
								//UE_LOG(LogTemp, Warning, TEXT("Remove Trap : %s"), *DetectTrap->GetName());
							}
						}

						else
						{
							//UE_LOG(LogTemp, Warning, TEXT("OutHitResult : %s"), *OutHitResult.GetActor()->GetName());
							FDamageEvent DamageEvent;
							Hit.GetActor()->TakeDamage(AttackCDCDamage, DamageEvent, nullptr, BossSword);
							//ServerRPCAttackHitSoundPlay();
						}

					}
				}
				
			}
			
			

		}
	}
}

float ABoss_Phase2::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	HitGauge += 5;
	UE_LOG(LogTemp, Warning, TEXT("Hit Gauge : %f"), HitGauge);

	//Blink
	if (DamageCauser->ActorHasTag("Trap"))
	{
		ServerRPCBossHitEffect();
	}
	
	if (MonsterState == EMonsterStateType::IsTrapAttack ||
		MonsterState == EMonsterStateType::IsTrapDetect ||
		MonsterState == EMonsterStateType::IsRockAttack ||
		bIsSkillStart == true)
	{
		return 0.f;
	}

	//HitGauge가 100이상일 경우, Spawn Monster skill발동
	if (HitGauge >= 100 && CurrentHP > 0)
	{

		if (MonsterState != EMonsterStateType::IsMonsterSpawn)
		{
			MonsterState = EMonsterStateType::IsMonsterSpawn;
			HitGauge = 0;
		}

		//UE_LOG(LogTemp, Warning, TEXT("Monster State : %d"),MonsterState);
	}
	

	if (CurrentHP <= 0)
	{
		bIsBossStage = false;
	}

	return 0.0f;
}

void ABoss_Phase2::SpawnSword()
{
	if (!HasAuthority())
	{
		return;
	}

	FVector SpawnLocation = SwordSpawn->GetComponentLocation();
	FRotator SpawnRotation = SwordSpawn->GetComponentRotation();

	auto BossADCAttack = GetWorld()->SpawnActor<ABossThrowSword>(ThrowSword, SpawnLocation, SpawnRotation);
	UE_LOG(LogTemp, Warning, TEXT("TargetName : %s"), *DetectPlayer.GetName());

	BossADCAttack->TargetCharacter = Cast<ACharacter>(DetectPlayer);
	BossADCAttack->SetOwner(this);

	//UE_LOG(LogTemp, Warning, TEXT("Fire!"));
}

void ABoss_Phase2::SpawnMonsters()
{
	if (!HasAuthority())
	{
		return;
	}

	//UE_LOG(LogTrap, Warning, TEXT("SpawnMonsters"));

	ATrapperGameMode* TrapperGameMode = Cast<ATrapperGameMode>(UGameplayStatics::GetGameMode(GetWorld()));

	uint32 NextTarget = this->CurrentTarget;

	if (12 <= this->CurrentTarget + 2)
	{
		NextTarget = 12;
	}

	if (IsValid(TrapperGameMode))
	{
		MonsterSpawner = Cast<ASpawner>(TrapperGameMode->Spanwer);
	}

	GameState = Cast<ATrapperGameState>(GetWorld()->GetGameState());
	
	FVector BossLocation = GetActorLocation();

	FVector MonsterSpawnLocation1 = FVector(BossLocation.X - 100, BossLocation.Y, BossLocation.Z + 100);
	FVector MonsterSpawnLocation2 = FVector(BossLocation.X, BossLocation.Y + 100, BossLocation.Z + 100);
	FVector MonsterSpawnLocation3 = FVector(BossLocation.X + 100, BossLocation.Y, BossLocation.Z + 100);
	FVector MonsterSpawnLocation4 = FVector(BossLocation.X + 200, BossLocation.Y, BossLocation.Z + 100);
	FVector MonsterSpawnLocation5 = FVector(BossLocation.X - 200, BossLocation.Y, BossLocation.Z + 100);
	FVector MonsterSpawnLocation6 = FVector(BossLocation.X, BossLocation.Y + 200, BossLocation.Z + 100);
	FVector MonsterSpawnLocation7 = FVector(BossLocation.X, BossLocation.Y + 300, BossLocation.Z + 100);
																									
	if (IsValid(MonsterSpawner))
	{
		TObjectPtr<ADetectedMonster> SpawnDetected1;

		//MonsterSpawner->DetectedList[0]->MonsterState = EMonsterStateType::GoToTower;
		//MonsterSpawner->SpawnMonsterList.Add(MonsterSpawner->DetectedList[0]);
		//MonsterSpawner->DetectedList[0]->TeleportTo(MonsterSpawnLocation1, MonsterSpawner->DetectedList[0]->GetActorRotation());
		//MonsterSpawner->DetectedList[0]->SetActorEnableCollision(true);
		//MonsterSpawner->DetectedList[0]->GetMesh()->SetVisibility(true);
		//MonsterSpawner->DetectedList.RemoveAt(0);

		SpawnDetected1 = GetWorld()->SpawnActor<ADetectedMonster>(DetectedMonsterClass, MonsterSpawnLocation1, this->GetActorRotation());
		MonsterSpawner->SpawnMonsterList.Add(SpawnDetected1);
		SpawnDetected1->CurrentTarget = NextTarget;
		SpawnDetected1->bIsSpawn = true;
		SpawnDetected1->SetOwner(this);
		if (GameState) GameState->ChangeMonsterCount(1);
		
		
		TObjectPtr<ADetectedMonster> SpawnDetected2;

		//MonsterSpawner->DetectedList[0]->MonsterState = EMonsterStateType::GoToTower;
		//MonsterSpawner->SpawnMonsterList.Add(MonsterSpawner->DetectedList[0]);
		//MonsterSpawner->DetectedList[0]->TeleportTo(MonsterSpawnLocation1, MonsterSpawner->DetectedList[0]->GetActorRotation());
		//MonsterSpawner->DetectedList[0]->SetActorEnableCollision(true);
		//MonsterSpawner->DetectedList[0]->GetMesh()->SetVisibility(true);
		//MonsterSpawner->DetectedList.RemoveAt(0);

		SpawnDetected2 = GetWorld()->SpawnActor<ADetectedMonster>(DetectedMonsterClass, MonsterSpawnLocation2, this->GetActorRotation());
		MonsterSpawner->SpawnMonsterList.Add(SpawnDetected2);
		SpawnDetected2->CurrentTarget = NextTarget;
		SpawnDetected2->bIsSpawn = true;
		SpawnDetected2->SetOwner(this);
		if (GameState) GameState->ChangeMonsterCount(1);

		TObjectPtr<ADetectedMonster> SpawnDetected3;

		//MonsterSpawner->DetectedList[0]->MonsterState = EMonsterStateType::GoToTower;
		//MonsterSpawner->SpawnMonsterList.Add(MonsterSpawner->DetectedList[0]);
		//MonsterSpawner->DetectedList[0]->TeleportTo(MonsterSpawnLocation1, MonsterSpawner->DetectedList[0]->GetActorRotation());
		//MonsterSpawner->DetectedList[0]->SetActorEnableCollision(true);
		//MonsterSpawner->DetectedList[0]->GetMesh()->SetVisibility(true);
		//MonsterSpawner->DetectedList.RemoveAt(0);ㄴ

		SpawnDetected3 = GetWorld()->SpawnActor<ADetectedMonster>(DetectedMonsterClass, MonsterSpawnLocation3, this->GetActorRotation());
		MonsterSpawner->SpawnMonsterList.Add(SpawnDetected3);
		SpawnDetected3->CurrentTarget = NextTarget;
		SpawnDetected3->bIsSpawn = true;
		SpawnDetected3->SetOwner(this);
		if (GameState) GameState->ChangeMonsterCount(1);

		TObjectPtr<ADetectedMonster> SpawnDetected4;

		/*MonsterSpawner->DetectedList[3]->MonsterState = EMonsterStateType::GoToTower;
		MonsterSpawner->SpawnMonsterList.Add(MonsterSpawner->DetectedList[3]);
		MonsterSpawner->DetectedList[3]->TeleportTo(MonsterSpawnLocation4, MonsterSpawner->DetectedList[3]->GetActorRotation());
		MonsterSpawner->DetectedList[3]->CurrentTarget = NextTarget;
		MonsterSpawner->DetectedList[3]->SetActorEnableCollision(true);
		if (GameState) GameState->ChangeMonsterCount(1);
		MonsterSpawner->DetectedList.RemoveAt(3);
		*/

		SpawnDetected4 = GetWorld()->SpawnActor<ADetectedMonster>(DetectedMonsterClass, MonsterSpawnLocation4, this->GetActorRotation());
		MonsterSpawner->SpawnMonsterList.Add(SpawnDetected4);
		SpawnDetected4->CurrentTarget = NextTarget;
		SpawnDetected4->bIsSpawn = true;
		SpawnDetected4->SetOwner(this);
		if (GameState) GameState->ChangeMonsterCount(1);


		TObjectPtr<ADetectedMonster> SpawnDetected5;
		
		/*
		MonsterSpawner->DetectedList[4]->MonsterState = EMonsterStateType::GoToTower;
		MonsterSpawner->SpawnMonsterList.Add(MonsterSpawner->DetectedList[4]);
		MonsterSpawner->DetectedList[4]->TeleportTo(MonsterSpawnLocation5, MonsterSpawner->DetectedList[4]->GetActorRotation());
		MonsterSpawner->DetectedList[4]->CurrentTarget = NextTarget;
		MonsterSpawner->DetectedList[4]->SetActorEnableCollision(true);
		if (GameState) GameState->ChangeMonsterCount(1);
		MonsterSpawner->DetectedList.RemoveAt(4);
		*/

		SpawnDetected5 = GetWorld()->SpawnActor<ADetectedMonster>(DetectedMonsterClass, MonsterSpawnLocation5, this->GetActorRotation());
		MonsterSpawner->SpawnMonsterList.Add(SpawnDetected5);
		SpawnDetected5->CurrentTarget = NextTarget;
		SpawnDetected5->bIsSpawn = true;
		SpawnDetected5->SetOwner(this);
		if (GameState) GameState->ChangeMonsterCount(1);

		TObjectPtr<ADetectedMonster> SpawnDetected6;
		/*
		MonsterSpawner->DetectedList[5]->MonsterState = EMonsterStateType::GoToTower;
		MonsterSpawner->SpawnMonsterList.Add(MonsterSpawner->DetectedList[5]);
		MonsterSpawner->DetectedList[5]->TeleportTo(MonsterSpawnLocation6, MonsterSpawner->DetectedList[5]->GetActorRotation());
		MonsterSpawner->DetectedList[5]->CurrentTarget = NextTarget;
		MonsterSpawner->DetectedList[5]->SetActorEnableCollision(true);
		if (GameState) GameState->ChangeMonsterCount(1);
		MonsterSpawner->DetectedList.RemoveAt(5);
		*/

		SpawnDetected6 = GetWorld()->SpawnActor<ADetectedMonster>(DetectedMonsterClass, MonsterSpawnLocation6, this->GetActorRotation());
		MonsterSpawner->SpawnMonsterList.Add(SpawnDetected6);
		SpawnDetected6->CurrentTarget = NextTarget;
		SpawnDetected6->bIsSpawn = true;
		SpawnDetected6->SetOwner(this);
		if (GameState) GameState->ChangeMonsterCount(1);

		TObjectPtr<ADetectedMonster> SpawnDetected7;
		/*
		MonsterSpawner->DetectedList[6]->MonsterState = EMonsterStateType::GoToTower;
		MonsterSpawner->SpawnMonsterList.Add(MonsterSpawner->DetectedList[6]);
		MonsterSpawner->DetectedList[6]->TeleportTo(MonsterSpawnLocation7, MonsterSpawner->DetectedList[6]->GetActorRotation());
		MonsterSpawner->DetectedList[6]->CurrentTarget = NextTarget;
		MonsterSpawner->DetectedList[6]->SetActorEnableCollision(true);
		if (GameState) GameState->ChangeMonsterCount(1);
		MonsterSpawner->DetectedList.RemoveAt(6);*/

		SpawnDetected7 = GetWorld()->SpawnActor<ADetectedMonster>(DetectedMonsterClass, MonsterSpawnLocation7, this->GetActorRotation());
		MonsterSpawner->SpawnMonsterList.Add(SpawnDetected7);
		SpawnDetected7->CurrentTarget = NextTarget;
		SpawnDetected7->bIsSpawn = true;
		SpawnDetected7->SetOwner(this);
		if (GameState) GameState->ChangeMonsterCount(1);

	}

}

void ABoss_Phase2::OnOverlapBeginCloseSight(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

	//UE_LOG(LogTemp, Warning, TEXT("Overlap?"));

	if (!HasAuthority())
	{
		return;
	}

	if (MonsterState != EMonsterStateType::IsPillarActived &&
		MonsterState != EMonsterStateType::IsStun &&
		MonsterState != EMonsterStateType::IsDead &&
		MonsterState != EMonsterStateType::IsAttackStart)
	{
		if (OtherActor->ActorHasTag("Tower"))
		{
			//UE_LOG(LogTemp, Warning, TEXT("TowerAttack"));
			bIsAttackDetect = true;
			MonsterState = EMonsterStateType::IsTowerAttack;
			//bIsCDCAttackState = false;
			bIsTowerDetected = true;

		}

		if (bIsCDCAttackState == true)
		{
			
			if (OtherActor->ActorHasTag("Player"))
			{
				bIsAttackDetect = true;
				AttackPlayer = Cast<ATrapperPlayer>(OtherActor);
				MonsterState = EMonsterStateType::IsCloseAttack;
				bIsAttackEnd = false;
				bIsCDCComboState = true;
				UE_LOG(LogTemp, Warning, TEXT("Attack Start"));
				bIsCDCAttackState = false;
			}

			//UE_LOG(LogTemp, Warning, TEXT("OverlapBegin : %s"), *OtherActor->GetName());

			

			if (OtherActor->ActorHasTag("BossDetectionTrap"))
			{
				TObjectPtr<ATrapBase> DetectTrap = Cast<ATrapBase>(OtherActor);
				UE_LOG(LogTemp, Warning, TEXT("Trap Detected"));

				if (IsValid(DetectTrap))
				{
					MonsterState = EMonsterStateType::IsCloseAttack;
					bIsAttackEnd = false;
					DetectTrapLocation = DetectTrap->GetActorLocation();
					bIsCDCAttackState = false;

				}
			}
		}

	}
}

void ABoss_Phase2::OnOverlapEndCloseSight(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	//UE_LOG(LogTemp, Warning, TEXT("Work?"));
	
	if (OtherActor->ActorHasTag("Player"))
	{
		bIsCDCComboState = false;
		bIsAttackEnd = true;
		//UE_LOG(LogTemp, Warning, TEXT("Check Combo false"));
	}
}

void ABoss_Phase2::OnOverlapBeginTrapSight(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!HasAuthority())
	{
		return;
	}
	//UE_LOG(LogTemp, Warning, TEXT("Trap Attack Active"));

	if (MonsterState != EMonsterStateType::IsPillarActived &&
		MonsterState != EMonsterStateType::IsStun &&
		MonsterState != EMonsterStateType::IsDead &&
		MonsterState != EMonsterStateType::IsAttackStart)
	{
		if (OtherActor->ActorHasTag("BossAttackableTrap"))
		{
			TObjectPtr<ATrapBase> DetectTrap = Cast<ATrapBase>(OtherActor);

			if (IsValid(DetectTrap))
			{
				if (DetectTrap->IsWeckage() == false)
				{
					if (!DetectTrapList.Contains(DetectTrap))
					{
						DetectTrapList.Add(DetectTrap);
						//UE_LOG(LogTemp, Warning, TEXT("Add Trap : %s"), *DetectTrap->GetName());
					}

					if (bIsTrapAttackState == true && OtherActor->ActorHasTag("BossDetectionTrap"))
					{
						bIsAttackEnd = false;
						bIsTrapAttackState = false;
						MonsterState = EMonsterStateType::IsTrapAttack;
					}
				}
			}
			
		}

		if (OtherActor->ActorHasTag("MagneticPillar"))
		{
			TObjectPtr<AMagneticPillar> DetectPillar = Cast<AMagneticPillar>(OtherActor);
			if (IsValid(DetectPillar))
			{
				if (!DetectMagneticList.Contains(DetectPillar))
				{
					DetectMagneticList.Add(DetectPillar);
				}
				if (bIsTrapAttackState == true)
				{
					bIsAttackEnd = false;
					bIsTrapAttackState = false;
					MonsterState = EMonsterStateType::IsTrapAttack;
					//UE_LOG(LogTemp, Warning, TEXT("TrapCheck"));
				}

			}
		}

		if (OtherActor->ActorHasTag("Player"))
		{
			if (Cast<ATrapperPlayer>(OtherActor)->HasAuthority())
			{
				DamagedPlayer1P = Cast<ATrapperPlayer>(OtherActor);
			}
			else
			{
				DamagedPlayer2P = Cast<ATrapperPlayer>(OtherActor);
			}

		}

	}
}

void ABoss_Phase2::OnOverlapEndTrapSight(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (!HasAuthority())
	{
		return;
	}

	//UE_LOG(LogTemp, Warning, TEXT("Work?"));
	if (OtherActor->ActorHasTag("BossAttackableTrap"))
	{
		TObjectPtr<ATrapBase> DetectTrap = Cast<ATrapBase>(OtherActor);

		if (IsValid(DetectTrap))
		{
			if (bSafeRanged)
			{
				//UE_LOG(LogTemp, Warning, TEXT("bSafeRanged true good"));
			}
			else
			{
				DetectTrapList.Remove(DetectTrap);
			}
		}
	}

	if (OtherActor->ActorHasTag("MagneticPillar"))
	{
		TObjectPtr<AMagneticPillar> DetectPillar = Cast<AMagneticPillar>(OtherActor);



		if (IsValid(DetectPillar))
		{
			if (bSafeRanged)
			{
				//UE_LOG(LogTemp, Warning, TEXT("bSafeRanged true good"));
			}
			else
			{
				DetectMagneticList.Remove(DetectPillar);
			}

		}
	}

	if (OtherActor->ActorHasTag("Player"))
	{
		if (bSafeRanged)
		{
			//UE_LOG(LogTemp, Warning, TEXT("bSafeRanged true good"));
		}
		else
		{
			if (Cast<ATrapperPlayer>(OtherActor)->HasAuthority())
			{
				DamagedPlayer1P = nullptr;
			}
			else
			{
				DamagedPlayer2P = nullptr;
			}
		}

	}
}

void ABoss_Phase2::ServerRPCSpawnMonster_Implementation()
{
	MulticastRPCSpawnMonster();
	bIsSkillStart = true;
}

void ABoss_Phase2::MulticastRPCSpawnMonster_Implementation()
{
	if (!IsValid(AnimInstance))
	{
		return;
	}

	AnimInstance->Montage_Play(SpawnMonsterMontage, 1.5f);
} 

void ABoss_Phase2::ServerRPCTrapAttack_Implementation()
{
	if (HasAuthority())
	{
		MulticastRPCTrapAttack();
		GetWorld()->GetTimerManager().SetTimer(TimerHandleTrap, this, &ABoss_Phase2::TrapAttackState, TrapSkillCoolTime, false);

		APawn* Pawn = GetWorld()->GetFirstPlayerController()->GetPawn();

		if (IsValid(Pawn))
		{
			ATrapperPlayer* TP = Cast<ATrapperPlayer>(Pawn);
			if (IsValid(TP))
			{
				TP->ShakeCamera(this->GetActorLocation(), ECameraShakeType::BossMonster);
			}
		}
	}
	
}

void ABoss_Phase2::MulticastRPCTrapAttack_Implementation()
{
	if (!IsValid(AnimInstance))
	{
		return;
	}

	if (IsValid(AttackTrapMontage))
	{
		AnimInstance->Montage_Play(AttackTrapMontage, 1.5f);
		bIsSkillStart = true;
	}
}

void ABoss_Phase2::ServerRPCADCAttack_Implementation()
{
	MulticastRPCADCAttack();
	GetWorld()->GetTimerManager().SetTimer(TimerHandleADC, this, &ABoss_Phase2::ADCAttackState, ADCSkillCoolTime, false);
}

void ABoss_Phase2::MulticastRPCADCAttack_Implementation()
{
	if (!IsValid(AnimInstance))
	{
		return;
	}

	if (IsValid(AttackADCMontage))
	{
		AnimInstance->Montage_Play(AttackADCMontage);
	}
}

void ABoss_Phase2::ServerRPCCDCAttack_Implementation()
{
	MulticastRPCCDCAttack();
	bIsSkillStart = true;
	GetWorld()->GetTimerManager().SetTimer(TimerHandleCDC, this, &ABoss_Phase2::CDCAttackState, CDCSkillCoolTime, false);
}

void ABoss_Phase2::MulticastRPCCDCAttack_Implementation()
{
	if (!IsValid(AnimInstance))
	{
		return;
	}

	if (IsValid(AttackCDCMontage))
	{
		AnimInstance->Montage_Play(AttackCDCMontage);
	}
}

void ABoss_Phase2::ServerRPCCDCAttackCombo_Implementation()
{
	MulticastRPCCDCAttackCombo();
}

void ABoss_Phase2::MulticastRPCCDCAttackCombo_Implementation()
{
	if (!IsValid(AnimInstance))
	{
		return;
	}

	if (IsValid(AttackCDCComboMontage))
	{
		AnimInstance->Montage_Play(AttackCDCComboMontage);
	}
}

void ABoss_Phase2::ServerRPCCameraShaking_Implementation()
{
	MulticastRPCCameraShaking();
}

void ABoss_Phase2::MulticastRPCCameraShaking_Implementation()
{
	APawn* Pawn = GetWorld()->GetFirstPlayerController()->GetPawn();
	if (IsValid(Pawn))
	{
		ATrapperPlayer* TP = Cast<ATrapperPlayer>(Pawn);
		if (IsValid(TP))
		{
			TP->ShakeCamera(this->GetActorLocation(), ECameraShakeType::BossMonster);
		}
	}
}

void ABoss_Phase2::ServerRPCBossHitEffect_Implementation()
{
	MulticastRPCBossHitEffect();
}

void ABoss_Phase2::MulticastRPCBossHitEffect_Implementation()
{
	BossMaterial0->SetScalarParameterValue(TEXT("ONOFFEM"), 1);
	BossMaterial1->SetScalarParameterValue(TEXT("ONOFFEM"), 1);

	GetWorld()->GetTimerManager().SetTimer(TimerHandleBossHit, this, &ABoss_Phase2::ChangeBossMaterial, 0.2, false);
}

void ABoss_Phase2::ServerRPCRockAttack_Implementation()
{
	if (HasAuthority())
	{
		MulticastRPCRockAttack();
	}
}

void ABoss_Phase2::MulticastRPCRockAttack_Implementation()
{
	if (!IsValid(AnimInstance))
	{
		return;
	}

	if (IsValid(AttackRockMontage))
	{
		AnimInstance->Montage_Play(AttackRockMontage, 1.5f);
	}
}
