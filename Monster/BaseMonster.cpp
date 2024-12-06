// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseMonster.h"

#include "Kismet/GameplayStatics.h"
#include "Animation/AnimInstance.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "Components/BoxComponent.h"
#include "Components/WidgetComponent.h"
#include "Blueprint/UserWidget.h"
#include "GameElement/Item.h"
#include "Net/UnrealNetwork.h"
#include "Math/UnrealMathUtility.h"
#include "Ping/MarkerComponent/MapMarkerComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "Camera/PlayerCameraManager.h"

#include "Character/TrapperPlayer.h"
#include "Bow/Arrow.h"
#include "Framework/TrapperGameState.h"
#include "Framework/TrapperGameMode.h"
#include "Widget/MonsterHealthBar.h"
#include "Widget/BossHealthBar.h"
#include "GameElement/Spawner.h"
#include "TrapperProject.h"
#include "Widget/MultikillHUD.h"
#include "Framework/TrapperPlayerController.h"
#include "Turret/TurretProjectile.h"
#include "Turret/Turret.h"
#include "Data/MonsterDataTables.h"
#include "Trap/PlankTrap.h"
#include "Trap/GunpowderBarrelTrap.h"
#include "Turret/Turret.h"
#include "Monster/CDC.h"
#include "Monster/ADC.h"
#include "Monster/DetectedMonster.h"
#include "Monster/Boss/Boss_Phase1.h"
#include "Monster/Boss/Boss_Phase2.h"

#include "Engine/DamageEvents.h"
#include "DamageType/DamageTypeDown.h"
#include "DamageType/DamageTypeKnockBack.h"
#include "DamageType/DamageTypeStun.h"

// Sets default values
ABaseMonster::ABaseMonster()
	:
	CurrentHP(0)
	, bISArrowShoot(false)
	, CurrentTarget(0)
	, bIsLastTarget(false)
	, bIsSightDetect(false)
	, bIsAttackDetect(false)
	, ItemMinDropNumber(1.f)
	, ItemMaxDropNumber(6.f)
	, SpawnLocationCorrection(100.f)
	, MaxHP(100.f)
	, FireRate(1.f)
	, DetectTime(100)
	, MonsterSpeed(300.f)
	, MonsterDetectSpeed(500.f)
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	//DataTable
	static ConstructorHelpers::FObjectFinder<UDataTable> DataTableFinder(TEXT("/Script/Engine.DataTable'/Game/Blueprints/Data/DT_MonsterData.DT_MonsterData'"));
	if (DataTableFinder.Succeeded())
	{
		DataTable = DataTableFinder.Object;

	}

	MainSight = CreateDefaultSubobject<USphereComponent>(TEXT("MainSight"));
	MainSight->SetupAttachment(RootComponent);

	AttackSight = CreateDefaultSubobject<UBoxComponent>(TEXT("AttackSight"));
	AttackSight->SetupAttachment(RootComponent);

	HeadShootCollider = CreateDefaultSubobject<UCapsuleComponent>(TEXT("HeadShootCollider"));
	HeadShootCollider->SetupAttachment(RootComponent);
	HeadShootCollider->ComponentTags.Add("MonsterHead");

	ItemDropPoint = CreateDefaultSubobject<USceneComponent>(TEXT("ItemDropPoint"));
	ItemDropPoint->SetupAttachment(RootComponent);

	MapMarker = CreateDefaultSubobject<UMapMarkerComponent>(TEXT("MapMarkerComponent"));
	MapMarker->ObjectType = EMapObjectType::Monster;
	AddOwnedComponent(MapMarker);

	WidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("WidgetComponent"));
	WidgetComponent->SetupAttachment(RootComponent);
	WidgetComponent->SetGenerateOverlapEvents(false);
	WidgetComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WidgetComponent->SetTickMode(ETickMode::Disabled);
	WidgetComponent->SetComponentTickEnabled(false);

	DebuffeEffectPosition = CreateDefaultSubobject<USceneComponent>(TEXT("DebuffeEffectPosition"));
	DebuffeEffectPosition->SetupAttachment(RootComponent);

	//Nav Component
	/*NavInvoker = CreateDefaultSubobject<UNavigationInvokerComponent>(TEXT("Nav Invoker"));
	AddOwnedComponent(NavInvoker);*/

	//Add Dynamic
	//HeadShootCollider->OnComponentBeginOverlap.AddDynamic(this, &ABaseMonster::OnOverlapBeginHead);
	HeadShootCollider->OnComponentHit.AddDynamic(this, &ABaseMonster::OnHitHead);

	MainSight->OnComponentBeginOverlap.AddDynamic(this, &ABaseMonster::OnOverlapBeginCharacter);
	MainSight->OnComponentEndOverlap.AddDynamic(this, &ABaseMonster::OnOverlapEndCharacter);

	AttackSight->OnComponentBeginOverlap.AddDynamic(this, &ABaseMonster::OnOverlapBeginAttackCharacter);
	AttackSight->OnComponentEndOverlap.AddDynamic(this, &ABaseMonster::OnOverlapEndAttackCharacter);
}

// Called when the game starts or when spawned
void ABaseMonster::BeginPlay()
{
	Super::BeginPlay();

	//GetData
	check(DataTable->GetRowMap().Num() > 0);

	TArray<FName> RowNames;
	if (DataTable)
	{
		RowNames = DataTable->GetRowNames();

		for (const FName& RowName : RowNames)
		{
			FMonsterInfo* MonsterInfo = DataTable->FindRow<FMonsterInfo>(RowName, FString());
			if (MonsterInfo)
			{
				MonsterInfoTable.Add(*MonsterInfo);
			}
		}

		//HP
		if (MonsterInfoTable[0].HP)
		{
			MaxHP = MonsterInfoTable[0].HP;
			CurrentHP = MaxHP;
		}
	}

	//Set CurrentHP to MaxHP
	CurrentHP = MaxHP;
	GetCharacterMovement()->MaxWalkSpeed = MonsterSpeed;

	//MonsterState = EMonsterStateType::Idle;
	//Set Monster Speed
	//GetMovementComponent()->SetMaxSpeed(MonsterSpeed);

	//시작 위치 저장.
	StartPoint = GetActorLocation();

	//Montage Add Dynamic
	//AnimInstance 준비
	AnimInstance = GetMesh()->GetAnimInstance();

	// 체력바
	HealthHUDRef = Cast<UMonsterHealthBar>(WidgetComponent->GetWidget());

	//Effect
	DebuffeActiveEffect = LoadObject<UNiagaraSystem>(nullptr, TEXT("/Script/Niagara.NiagaraSystem'/Game/Effect/MegaMagicVFXBundle/VFX/MagicAuraVFX/VFX/NatureAura/Systems/N_NatureAura.N_NatureAura'"));
	if (IsValid(DebuffeActiveEffect))
	{
		DebuffeActiveEffectComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(
			DebuffeActiveEffect,
			DebuffeEffectPosition,
			NAME_None,
			FVector(0.f, 0.f, 0.f),
			FRotator::ZeroRotator,
			EAttachLocation::KeepRelativeOffset,
			false
		);

		if (DebuffeActiveEffectComponent)
		{
			DebuffeActiveEffectComponent->Deactivate();
			//UE_LOG(LogTemp, Warning, TEXT("Check Deactive"));
		}
	}

	//Debuffe Effect꺼두기	
	DebuffeEffectPosition->SetVisibility(false, true);

	if (HasAuthority())
	{
		TrapperGameState = GetWorld()->GetGameState<ATrapperGameState>();
	}

	//ServiceTick 끄기
	//bIsSpawn = false;
	//GetMesh()->SetVisibility(false);
}

void ABaseMonster::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	GetWorld()->GetTimerManager().ClearAllTimersForObject(this);

	//FVector SpawnLocation = ItemDropPoint->GetComponentLocation();

	//if(WoodHitParticle)
	//{
	//	UGameplayStatics::SpawnEmitterAtLocation(
	//		GetWorld(),
	//		WoodHitParticle,
	//		SpawnLocation
	//	);
	//}
}

void ABaseMonster::BeginDestroy()
{
	Super::BeginDestroy();

	GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, FString::Printf(TEXT("Monster Destroied T_T")));

	if (WidgetComponent)
	{
		WidgetComponent->UnregisterComponent();
		WidgetComponent = nullptr;
	}
}

// Called to bind functionality to input
void ABaseMonster::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

float ABaseMonster::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (HasAuthority())
	{
		float Damage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

		if (MonsterState == EMonsterStateType::IsDead) return 0;
		if (MonsterState == EMonsterStateType::IsPhaseChange) return 0;
		if (MonsterState == EMonsterStateType::IsPhaseChanging) return 0;

		Damage = FMath::Min(CurrentHP, Damage);

		if (IsValid(DamageCauser) && DamageCauser != nullptr)
		{
			player = Cast<ATrapperPlayer>(DamageCauser->GetOwner());
			arrow = Cast<AArrow>(DamageCauser);
		}

		if (DamageCauser == arrow)
		{
			bISArrowShoot = true;

			if (IsValid(player))
			{
				DetectPlayer = player;
			}
		}

		CurrentHP -= Damage;
		OnRep_IsDamaged();

		//If Monster is doing skills do not play animation
		if (bIsSkillStart != true)
		{
			//If Damaged object is TurretProjectile, Do Not Play Animation
			if (!DamageCauser->ActorHasTag("TurretProjectile"))
			{
				//Change Monster State
				if (!this->ActorHasTag("Debuffer") && !this->ActorHasTag("Boss"))
				{
					MonsterState = EMonsterStateType::IsHit;
				}

				//Play each Damage Type Animation
				//Normal Damage Event
				if (!IsValid(DamageEvent.DamageTypeClass))
				{
					if (this->ActorHasTag("Debuffer"))
					{
						UE_LOG(LogTemp, Warning, TEXT("Debuffer"));
					}
					else
					{
						ServerRPCHit();
						UE_LOG(LogTemp, Warning, TEXT("Hit"));
					}

				}

				//If Damage Type exist
				else
				{
					const UDamageType* DamageType = DamageEvent.DamageTypeClass->GetDefaultObject<UDamageType>();

					if (DamageType->IsA<UDamageTypeDown>())
					{
						ServerRPCDownLoop();
					}
					else if (DamageType->IsA<UDamageTypeStun>())
					{
						ServerRPCStunLoop();
					}
					else if (DamageType->IsA<UDamageTypeKnockBack>())
					{
						ServerRPCKnockback();
					}
					else
					{

						if (this->ActorHasTag("Debuffer") || this->ActorHasTag("Boss"))
						{

						}
						else
						{
							ServerRPCHit();
							UE_LOG(LogTemp, Warning, TEXT("Hit"));
						}

					}
				}

			}
		}

		//Is HP <= 0 == Dead
		if (CurrentHP <= 0 && MonsterState != EMonsterStateType::IsDead)
		{
			SetActorEnableCollision(false);

			// Turret
			AllProjectileStopHoming();

			// Multikill Hud
			UpdateKillCount(DamageCauser);

			MonsterState = EMonsterStateType::IsDead;
			bIsDead = true;

			//UE_LOG(LogTemp, Warning, TEXT("MonsterDie! -> BaseMonster"));
			/*ATrapperGameState* GameState = Cast<ATrapperGameState>(GetWorld()->GetGameState());
			if (!ActorHasTag("Tutorial"))
			{
				if (GameState) GameState->ChangeMonsterCount(-1);
				UE_LOG(LogTemp, Warning, TEXT("Monster die"));
			}	*/
			//if(GameState) GameState->ChangeMonsterCount(-1);

			//UE_LOG(LogTemp, Warning, TEXT("Actor Die! (Damage)"), Damage, CurrentHP);
			//show player MonsterDead Icon
			if (player)
			{
				player->ShowKillFeedbackHUD();

			}

			//CDC
			if (DamageCauser->ActorHasTag("Trap") && this->ActorHasTag("CDC"))
			{
				Cast<ACDC>(this)->CDCDeadByTrap();
				MonsterState = EMonsterStateType::IsDestroy;

				ItemSpawn(DamageCauser->GetOwner());
				return Damage;
			}
			//Trap
			else if (DamageCauser->ActorHasTag("Trap") && this->ActorHasTag("ADC"))
			{
				Cast<AADC>(this)->ADCDeadByTrap();
				MonsterState = EMonsterStateType::IsDestroy;
				ItemSpawn(DamageCauser->GetOwner());
				return Damage;
			}
			//Detected
			else if (DamageCauser->ActorHasTag("Trap") && this->ActorHasTag("Detected"))
			{
				Cast<ADetectedMonster>(this)->DetectedDeadByTrap();
				MonsterState = EMonsterStateType::IsDestroy;
				ItemSpawn(DamageCauser->GetOwner());

				return Damage;
			}
			else
			{
				if (IsValid(DamageCauser))
				{
					ItemSpawn(DamageCauser);
				}

				ActorDie();
			}
		}

		// Score
		UpdateScore(DamageCauser);

		return Damage;
	}

	return 0;
}


void ABaseMonster::ActorDie()
{
	if (!HasAuthority())
	{
		return;
	}
	//GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);
	ServerRPCDead();
}

void ABaseMonster::ItemSpawn(AActor* DamageCauser)
{
	if (!HasAuthority())
	{
		return;
	}

	FVector SpawnLocation = ItemDropPoint->GetComponentLocation();
	FRotator SpawnRotation = ItemDropPoint->GetComponentRotation();

	//RandomNumber
	float ItemSpawnNumber = FMath::FRandRange(ItemMinDropNumber, ItemMaxDropNumber);

	for (int i = 0; i < ItemSpawnNumber; i++)
	{
		float RangeX = FMath::FRandRange(-SpawnLocationCorrection, SpawnLocationCorrection);
		float RangeY = FMath::FRandRange(-SpawnLocationCorrection, SpawnLocationCorrection);

		//UE_LOG(LogTemp, Warning, TEXT("Item Spawn Range %f %f"), RangeX, RangeY);

		SpawnLocation.X += RangeX;
		SpawnLocation.Y += RangeY;

		if (IsValid(ItemClass))
		{
			if (DamageCauser->ActorHasTag("TurretProjectile"))
			{
				GetWorld()->GetGameState<ATrapperGameState>()->AddBone();
				continue;
			}

			//UE_LOG(LogTrap, Warning, TEXT("DamageCauser Name : %s"), *DamageCauser->GetName());

			auto DropItem = GetWorld()->SpawnActor<AItem>(ItemClass, SpawnLocation, SpawnRotation);
			DropItem->SetOwner(this);

			if (DamageCauser->ActorHasTag("Arrow"))
			{
				DropItem->SetPlayer(Cast<ATrapperPlayer>(DamageCauser->GetOwner()));
			}
			else
			{
				DropItem->SetPlayer(Cast<ATrapperPlayer>(DamageCauser));
			}
		}
	}
}


void ABaseMonster::ServerRPCTeleportToDie_Implementation()
{
	MulticastRPCTeleportToDie();
}

void ABaseMonster::MulticastRPCTeleportToDie_Implementation()
{
	this->TeleportTo(StartPoint, GetActorRotation());
}

void ABaseMonster::UpdateBossHPBar()
{
	//컨트롤러의 UI 업데이트하기
	// GetGameMode()
	ATrapperPlayerController* PlayerController = Cast<ATrapperPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	if (IsValid(PlayerController))
	{
		PlayerController->BossHealthBar->UpdateHealth(CurrentHP, MaxHP);
	}
}

void ABaseMonster::ChangeMonsterSpeed()
{
	float SpeedVariation = TrapperGameState->MonsterMoveSpeed;

	MonsterSpeed = MonsterSpeed - (MonsterSpeed * SpeedVariation);
	MonsterDetectSpeed = MonsterDetectSpeed - (MonsterDetectSpeed * SpeedVariation);
	MonsterEffectSpeed = MonsterEffectSpeed - (MonsterEffectSpeed * SpeedVariation);
}

void ABaseMonster::ChangeMonsterHealth()
{
	float HealthVariation = TrapperGameState->MonsterHealth;

	MaxHP = MaxHP - (MaxHP * HealthVariation);
}

void ABaseMonster::OnOverlapBeginHead(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!HasAuthority())
	{
		return;
	}

	//Headshoot Collider is active
	if (OtherActor->ActorHasTag("DefaultArrow"))
	{
		bIsHeadShoot = true;
	}
}

void ABaseMonster::OnHitHead(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (!HasAuthority())
	{
		return;
	}

	//Headshoot Collider is active
	if (OtherActor->ActorHasTag("DefaultArrow") || OtherActor->ActorHasTag("MagneticArrow"))
	{
		bIsHeadShoot = true;
	}
}

void ABaseMonster::OnHeadShootHit()
{
	if (!HasAuthority())
	{
		return;
	}

	if (player && arrow && MonsterState != EMonsterStateType::IsDead && MonsterState != EMonsterStateType::IsDetect)
	{
		//Check AttackPlayer
		AttackPlayer = player;
	}
}

void ABaseMonster::OnNotHeadShootHit()
{
	if (!HasAuthority())
	{
		return;
	}

	//UE_LOG(LogTemp, Warning, TEXT("Damage : %.1f, HP : %.1f"), Damage, CurrentHP);

	if (player && arrow && MonsterState != EMonsterStateType::IsDead && MonsterState != EMonsterStateType::IsDetect)
	{
		//Check AttackPlayer
		AttackPlayer = player;
	}
}

void ABaseMonster::OnOverlapBeginCharacter(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!HasAuthority())
	{
		return;
	}

	if (OtherActor->ActorHasTag("Monster"))
	{
		return;
	}

	if (MonsterState == EMonsterStateType::IsPhaseChange)
	{
		return;
	}

	if (MonsterState == EMonsterStateType::IsPhaseChanging)
	{
		return;
	}

	if (MonsterState != EMonsterStateType::IsPillarActived &&
		MonsterState != EMonsterStateType::IsStun &&
		MonsterState != EMonsterStateType::IsDead &&
		MonsterState != EMonsterStateType::ISArrowShoot &&
		MonsterState != EMonsterStateType::IsMonsterSpawn &&
		MonsterState != EMonsterStateType::IsTrapAttack &&
		MonsterState != EMonsterStateType::IsPhaseChange &&
		bIsDebuffe == false)
	{
		if (OtherActor->ActorHasTag("Player") &&
			MonsterState != EMonsterStateType::IsDetect &&
			MonsterState != EMonsterStateType::IsDead &&
			MonsterState != EMonsterStateType::IsStun &&
			MonsterState != EMonsterStateType::IsDetectStart &&
			MonsterState != EMonsterStateType::IsTurretDetect &&
			MonsterState != EMonsterStateType::IsTowerDetect &&
			MonsterState != EMonsterStateType::IsTowerAttack &&
			MonsterState != EMonsterStateType::IsTowerReach
			)
		{
			ServerRPCDetectStart();
			MonsterState = EMonsterStateType::IsDetectStart;
			DetectPlayer = Cast<ATrapperPlayer>(OtherActor);
			//UE_LOG(LogTemp, Warning, TEXT("Detach Start"));

			bIsPlayerDetected = true;
		}
	}

	/*if (OtherActor->ActorHasTag("Turret"))
	{


	}*/

	if (OtherActor->ActorHasTag("Tower"))
	{
		Tower = Cast<ATower>(OtherActor);
		MonsterState = EMonsterStateType::IsTowerAttack;
		//UE_LOG(LogTemp, Warning, TEXT("GoToTower"));

		bIsTowerDetected = true;
	}

	if (IsValid(OtherActor))
	{
		//UE_LOG(LogTemp, Warning, TEXT("OTher Actor Name : %s"), *OtherActor->GetName());
	}
	//UE_LOG(LogTemp, Warning, TEXT("Detach Start"));
}

void ABaseMonster::OnOverlapEndCharacter(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor->ActorHasTag("Player"))
	{
		bIsPlayerDetected = false;
	}

	if (OtherActor->ActorHasTag("Turret"))
	{
		bIsTurretAttack = false;
	}

	if (OtherActor->ActorHasTag("Tower"))
	{
		bIsTowerDetected = false;
	}

}

void ABaseMonster::OnOverlapBeginAttackCharacter(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//UE_LOG(LogTemp, Warning, TEXT("Attack Start"));
	if (!HasAuthority())
	{
		return;
	}

	//UE_LOG(LogTemp, Warning, TEXT("OverlapBegin : %s"), *OtherActor->GetName());

	if (MonsterState == EMonsterStateType::IsPhaseChange)
	{
		return;
	}

	if (MonsterState == EMonsterStateType::IsPhaseChanging)
	{
		return;
	}

	if (MonsterState != EMonsterStateType::IsPillarActived &&
		MonsterState != EMonsterStateType::IsStun &&
		MonsterState != EMonsterStateType::IsDead &&
		MonsterState != EMonsterStateType::IsAttackStart &&
		MonsterState != EMonsterStateType::IsPhaseChange &&
		MonsterState != EMonsterStateType::IsTowerAttack)
	{
		//UE_LOG(LogTemp, Warning, TEXT("Attack Check"));

		if (OtherActor->ActorHasTag("Player"))
		{
			ABoss_Phase1* Boss1 = Cast<ABoss_Phase1>(this);

			if (this->ActorHasTag("BossPhase1"))
			{
				if (IsValid(Boss1))
				{
					if (Boss1->bIsADCAttackState == true)
					{
						Boss1->bIsADCAttackState = false;
						//UE_LOG(LogTemp, Warning, TEXT("ADC ATtack!"));
						MonsterState = EMonsterStateType::IsAttack;
					}
				}
			}

			else if (this->ActorHasTag("BossPhase2"))
			{
				
				ABoss_Phase2* Boss2 = Cast<ABoss_Phase2>(this);

				if (IsValid(Boss2))
				{
					if (Boss2->bIsADCAttackState == true)
					{
						Boss2->bIsADCAttackState = false;
						MonsterState = EMonsterStateType::IsAttack;
					}
				}

			}

			else
			{
				MonsterState = EMonsterStateType::IsAttack;
			}

			bIsAttackDetect = true;
			AttackPlayer = Cast<ATrapperPlayer>(OtherActor);
			DetectPlayer = Cast<ATrapperPlayer>(OtherActor);
			bIsAttackEnd = false;
			//UE_LOG(LogTemp, Warning, TEXT("Attack Start"));
		}

		

		if (!this->ActorHasTag("Boss"))
		{
			if (OtherActor->ActorHasTag("Tower"))
			{
				UE_LOG(LogTemp, Warning, TEXT("Tower Attack"));
				bIsAttackDetect = true;
				MonsterState = EMonsterStateType::IsTowerAttack;

			}

			if (OtherActor->ActorHasTag("Turret"))
			{
				bIsAttackDetect = true;
				MonsterState = EMonsterStateType::IsTowerAttack;
				//UE_LOG(LogTemp, Warning, TEXT("Attack Check"));
			}
		}
	}
}

void ABaseMonster::OnOverlapEndAttackCharacter(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (!HasAuthority())
	{
		return;
	}

	if (MonsterState != EMonsterStateType::IsPillarActived && MonsterState != EMonsterStateType::IsStun && MonsterState != EMonsterStateType::IsDead)
	{
		if (OtherActor->ActorHasTag("Player"))
		{
			bIsAttackDetect = false;
			bIsAttackEnd = true;
			GetCharacterMovement()->MaxWalkSpeed = MonsterSpeed;
			//UE_LOG(LogTemp, Warning, TEXT("Attack Check"));
		}

		if (OtherActor->ActorHasTag("Turret"))
		{
			bIsAttackDetect = false;
			bIsAttackEnd = true;
			GetCharacterMovement()->MaxWalkSpeed = MonsterSpeed;
		}

	}

}

void ABaseMonster::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABaseMonster, CurrentHP);

}

void ABaseMonster::OnRep_IsDamaged()
{
	if (!IsValid(HealthHUDRef))
	{
		return;
	}

	WidgetComponent->SetTickMode(ETickMode::Enabled);
	GetWorld()->GetTimerManager().SetTimerForNextTick(this, &ABaseMonster::UpdateHealthHUD);
	GetWorld()->GetTimerManager().SetTimer(
		UpdateHealthHUDRotationTimer, this, &ABaseMonster::UpdateHealthHUDRotation, 0.03, true);

	if (CurrentHP <= 0)
	{
		HealthHUDRef->UpdateHealth(CurrentHP, MaxHP, HealthHUDVisibleTime);
		GetWorld()->GetTimerManager().SetTimerForNextTick(this, &ABaseMonster::UpdateHealthHUDTick);
	}

	//BossHPBar
	if (this->ActorHasTag("Boss"))
	{
		GetWorld()->GetTimerManager().SetTimerForNextTick(this, &ABaseMonster::UpdateBossHPBar);
	}
}



void ABaseMonster::OnPlayerSight()
{
	if (bIsBossStage == true)
	{
		return;
	}

	if (!IsValid(HealthHUDRef))
	{
		return;
	}

	WidgetComponent->SetTickMode(ETickMode::Enabled);
	HealthHUDRef->UpdateHealth(CurrentHP, MaxHP, HealthHUDSightVisibleTime);

	GetWorld()->GetTimerManager().SetTimerForNextTick(this, &ABaseMonster::UpdateHealthHUD);
	GetWorld()->GetTimerManager().SetTimer(
		UpdateHealthHUDRotationTimer, this, &ABaseMonster::UpdateHealthHUDRotation, 0.03, true);

	if (CurrentHP <= 0)
	{
		HealthHUDRef->UpdateHealth(CurrentHP, MaxHP, HealthHUDSightVisibleTime);
		GetWorld()->GetTimerManager().SetTimerForNextTick(this, &ABaseMonster::UpdateHealthHUDTick);
	}
}

// 항상 체력바가 플레이어를 바라보도록 업데이트
void ABaseMonster::UpdateHealthHUDRotation()
{
	if (bIsBossStage == true)
	{
		return;
	}

	if (IsValid(HealthHUDRef) && HealthHUDRef->bIsVariable)
	{
		if (!HealthHUDRef->bOnHit && !HealthHUDRef->bIsOnSight)
		{
			GetWorld()->GetTimerManager().ClearTimer(UpdateHealthHUDRotationTimer);
			return;
		}

		if (IsValid(PlayerCameraManager))
		{
			FVector CameraLocation = PlayerCameraManager->GetCameraLocation();
			FVector WidgetLocation = WidgetComponent->GetComponentLocation();
			FRotator LookAtRotation = (CameraLocation - WidgetLocation).Rotation();
			WidgetComponent->SetWorldRotation(LookAtRotation);
		}
		else
		{
			if (APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0))
			{
				PlayerCameraManager = PlayerController->PlayerCameraManager;
			}
		}
	}
}

void ABaseMonster::UpdateHealthHUD()
{
	HealthHUDRef->UpdateHealth(CurrentHP, MaxHP, HealthHUDSightVisibleTime);
}

void ABaseMonster::UpdateHealthHUDTick()
{
	WidgetComponent->SetTickMode(ETickMode::Disabled);
}

void ABaseMonster::UpdateKillCount(AActor* DamageCauser)
{
	if (!DamageCauser)
	{
		UE_LOG(LogTrap, Warning, TEXT("UpdateKillCount return? -> DamageCauser nullptr"));
		return;
	}

	//UE_LOG(LogTrap, Warning, TEXT("EventInstigator name : %s"), *EventInstigator->GetName());
	if (TrapperGameState.Get())
	{
		TrapperGameState->RegisterKill(DamageCauser);
	}
}

void ABaseMonster::AddProjectile(ATurretProjectile* Projectile)
{
	if (!TurretProjectileArray.Contains(Projectile))
	{
		if (bSafeRangedFor)
		{
			//UE_LOG(LogTrap, Error, TEXT("BaseMonster : bSafeRangedFor true -> Add Projectile"));

			SafeAddTurretProjectileArray.Add(Projectile);
		}
		else
		{
			TurretProjectileArray.Add(Projectile);
		}
	}
}

void ABaseMonster::RemoveProjectile(ATurretProjectile* Projectile)
{
	if (TurretProjectileArray.Find(Projectile))
	{
		if (bSafeRangedFor)
		{
			//UE_LOG(LogTrap, Error, TEXT("BaseMonster : bSafeRangedFor true -> Remove Projectile"));

			SafeRemoveTurretProjectileArray.Add(Projectile);
		}
		else
		{
			TurretProjectileArray.Remove(Projectile);
		}
	}
}

void ABaseMonster::AllProjectileStopHoming()
{
	bSafeRangedFor = true;
	for (const auto& Projectile : TurretProjectileArray)
	{
		if (IsValid(Projectile))
		{
			Projectile->StopHomingTarget();
		}
	}
	bSafeRangedFor = false;

	for (const auto& SafeAddProjectile : SafeAddTurretProjectileArray)
	{
		//UE_LOG(LogTrap, Error, TEXT("BaseMonster : SafeAddTurretProjectileArray"));

		if (!TurretProjectileArray.Contains(SafeAddProjectile))
		{

			TurretProjectileArray.Add(SafeAddProjectile);
		}
	}

	for (const auto& SafeRemoveProjectile : SafeRemoveTurretProjectileArray)
	{
		//UE_LOG(LogTrap, Error, TEXT("BaseMonster : SafeRemoveTurretProjectileArray"));

		if (TurretProjectileArray.Contains(SafeRemoveProjectile))
		{
			TurretProjectileArray.Remove(SafeRemoveProjectile);
		}
	}

	TurretProjectileArray.Empty();
	SafeAddTurretProjectileArray.Empty();
	SafeRemoveTurretProjectileArray.Empty();
}

void ABaseMonster::UpdateScore(AActor* DamageCauser)
{
	if (!IsValid(DamageCauser))
	{
		return;
	}

	if (DamageCauser->ActorHasTag("Arrow"))
	{
		AArrow* Arrow = Cast<AArrow>(DamageCauser);
		if (Arrow)
		{
			if (Arrow->IsSpawnedBy1P())
			{
				if (bIsDead)
				{
					TrapperGameState->AddScore(EScoreType::MonsterScore1P, 100);
				}
				else
				{
					TrapperGameState->AddScore(EScoreType::MonsterScore1P, 10);
				}
			}
			else
			{
				if (bIsDead)
				{
					TrapperGameState->AddScore(EScoreType::MonsterScore2P, 100);
				}
				else
				{
					TrapperGameState->AddScore(EScoreType::MonsterScore2P, 10);
				}
			}
		}
	}
	else if (DamageCauser->ActorHasTag("Trap"))
	{
		bool bTrapScoreAll = true;
		bool bIs1P = false;

		if (DamageCauser->ActorHasTag("PlankTrap"))
		{
			bTrapScoreAll = false;

			APlankTrap* PlankTrap = Cast<APlankTrap>(DamageCauser);
			if (PlankTrap)
			{
				if (PlankTrap->IsTriggered1P())
				{
					bIs1P = true;
				}
				else
				{
					bIs1P = false;
				}
			}
		}
		else if (DamageCauser->ActorHasTag("GunpowderBarrelTrap"))
		{
			bTrapScoreAll = false;

			AGunpowderBarrelTrap* GunpowderBarrelTrap = Cast<AGunpowderBarrelTrap>(DamageCauser);
			if (GunpowderBarrelTrap)
			{
				if (GunpowderBarrelTrap->IsTriggeredExternal())
				{
					if (GunpowderBarrelTrap->IsInstalled1P())
					{
						bIs1P = true;
					}
					else
					{
						bIs1P = false;
					}
				}
				else
				{
					if (GunpowderBarrelTrap->IsTriggered1P())
					{
						bIs1P = true;
					}
					else
					{
						bIs1P = false;
					}
				}

			}
		}
		else if (DamageCauser->ActorHasTag("SpikeTrap"))
		{
			bTrapScoreAll = false;

			ATrapBase* SpikeTrap = Cast<ATrapBase>(DamageCauser);
			if (SpikeTrap)
			{
				if (SpikeTrap->IsInstalled1P())
				{
					bIs1P = true;
				}
				else
				{
					bIs1P = false;
				}
			}
		}

		if (bIsDead)
		{
			if (bTrapScoreAll)
			{
				TrapperGameState->AddScore(EScoreType::MonsterScoreAll, 100);
			}
			else
			{
				if (bIs1P)
				{
					TrapperGameState->AddScore(EScoreType::MonsterScore1P, 100);
				}
				else
				{
					TrapperGameState->AddScore(EScoreType::MonsterScore2P, 100);
				}
			}
		}
		else
		{
			if (bTrapScoreAll)
			{
				TrapperGameState->AddScore(EScoreType::MonsterScoreAll, 10);
			}
			else
			{
				if (bIs1P)
				{
					TrapperGameState->AddScore(EScoreType::MonsterScore1P, 10);
				}
				else
				{
					TrapperGameState->AddScore(EScoreType::MonsterScore2P, 10);
				}
			}
		}
	}
}

void ABaseMonster::ServerRPCHit_Implementation()
{
	MulticastRPCHit();
}

void ABaseMonster::MulticastRPCHit_Implementation()
{
	if (!AnimInstance)
	{
		return;
	}

	float RandomNum = FMath::FRandRange(1.0, 4.0);
	AnimInstance->StopAllMontages(0.0f);

	if (HitMontage1 != nullptr)
	{
		if (HitMontage2 != nullptr)
		{
			if (HitMontage3 != nullptr)
			{

				if (RandomNum < 2)
				{
					AnimInstance->Montage_Play(HitMontage1, 1.f);
					//UE_LOG(LogTemp, Warning, TEXT("Play First Montage"), RandomNum);
				}

				else if (RandomNum < 3)
				{
					AnimInstance->Montage_Play(HitMontage2, 1.f);
					//UE_LOG(LogTemp, Warning, TEXT("Play Second Montage"), RandomNum);
				}

				else
				{
					AnimInstance->Montage_Play(HitMontage3, 1.f);
					//UE_LOG(LogTemp, Warning, TEXT("Play Third Montage"), RandomNum);
				}
			}
		}
	}
}

void ABaseMonster::ServerRPCDetectStart_Implementation()
{
	MulticastRPCDetectStart();
}

void ABaseMonster::MulticastRPCDetectStart_Implementation()
{
	if (!AnimInstance)
	{
		return;
	}

	//DetectStart Montage Play
	if (DetectStartMontage != nullptr)
	{
		AnimInstance->StopAllMontages(0.0f);
		GetCharacterMovement()->MaxWalkSpeed = 0.f;
		AnimInstance->Montage_Play(DetectStartMontage, 2.0f);
	}

}

void ABaseMonster::ServerRPCDead_Implementation()
{
	//UE_LOG(LogTemp, Warning, TEXT("ServerRPC Call Multicast"));
	MulticastRPCDead();

}

void ABaseMonster::MulticastRPCDead_Implementation()
{
	if (!AnimInstance)
	{
		return;
	}

	//Dead Montage Play

	if (DeadMontage != nullptr)
	{
		//UE_LOG(LogTemp, Warning, TEXT("Check Dead Montage"));
		AnimInstance->StopAllMontages(0.0f);
		AnimInstance->Montage_Play(DeadMontage, 1.0f);
		//GetCharacterMovement()->DisableMovement();

	}


}

void ABaseMonster::ServerRPCHeadHit_Implementation()
{
	MulticastRPCHeadHit();
}

void ABaseMonster::MulticastRPCHeadHit_Implementation()
{
	if (!AnimInstance)
	{
		return;
	}

	//HeadHit Montage Play

	if (HeadHitMontage != nullptr)
	{
		AnimInstance->StopAllMontages(0.0f);
		AnimInstance->Montage_Play(HeadHitMontage, 1.0f);
	}

}

void ABaseMonster::ServerRPCPlayDieEffect_Implementation()
{
	MulticastPRCPlayDieEffect();
}

void ABaseMonster::MulticastPRCPlayDieEffect_Implementation()
{
	if (IsValid(DieEffect) && IsValid(GetMesh()))
	{
		UNiagaraFunctionLibrary::SpawnSystemAttached(
			DieEffect,
			GetMesh(),
			NAME_None,
			FVector::ZeroVector,
			FRotator::ZeroRotator,
			EAttachLocation::KeepRelativeOffset,
			true);
	}
}

void ABaseMonster::ServerRPCPlayArrowHitEffect_Implementation()
{
	MulticastPRCPlayArrowHitEffect();
}

void ABaseMonster::MulticastPRCPlayArrowHitEffect_Implementation()
{
	if (IsValid(ArrowHitEffect) && IsValid(GetMesh()))
	{

		if (bIsHeadShoot == true)
		{
			UNiagaraFunctionLibrary::SpawnSystemAttached(
				ArrowHitEffect,
				GetMesh(),
				NAME_None,
				FVector(0.f, 0.f, 180.f),
				FRotator::ZeroRotator,
				EAttachLocation::KeepRelativeOffset,
				true);
		}

		else
		{
			UNiagaraFunctionLibrary::SpawnSystemAttached(
				ArrowHitEffect,
				GetMesh(),
				NAME_None,
				FVector(0.f, 0.f, 100.f),
				FRotator::ZeroRotator,
				EAttachLocation::KeepRelativeOffset,
				true);
		}

	}
}

void ABaseMonster::ServerRPCIdle_Implementation()
{
	MulticastRPCIdle();
}

void ABaseMonster::MulticastRPCIdle_Implementation()
{
	if (!AnimInstance)
	{
		return;
	}

	//HeadHit Montage Play
	AnimInstance->StopAllMontages(0.0f);
	if (IsValid(IdleMontage))
	{
		AnimInstance->Montage_Play(IdleMontage, 1.0f);
	}

}

void ABaseMonster::ServerRPCEffectDeactive_Implementation()
{
	MulticastRPCEffectDeactive();
}

void ABaseMonster::MulticastRPCEffectDeactive_Implementation()
{
	DebuffeEffectPosition->SetVisibility(false, true);
	DebuffeActiveEffectComponent->Deactivate();
	//UE_LOG(LogTemp, Warning, TEXT("Off The Effect"));
}

void ABaseMonster::CheckDebufferState()
{
	if (bIsDebuffe == true)
	{
		bIsDebuffe = false;
	}
}

void ABaseMonster::ServerRPCEffectActive_Implementation()
{
	MulticastRPCEffectActive();
}

void ABaseMonster::MulticastRPCEffectActive_Implementation()
{
	DebuffeEffectPosition->SetVisibility(true, true);
	DebuffeActiveEffectComponent->Activate(true);
	//UE_LOG(LogTemp, Warning, TEXT("On The Effect"));
}

void ABaseMonster::ServerRPCKnockback_Implementation()
{
	MulticastRPCKnockback();
}

void ABaseMonster::MulticastRPCKnockback_Implementation()
{
	if (!AnimInstance)
	{
		return;
	}

	//Knockback Montage Play
	float RandomNum = FMath::FRandRange(1.0, 3.0);
	AnimInstance->StopAllMontages(0.0f);
	if (IsValid(KnockbackMontage1) && IsValid(KnockbackMontage2))
	{
		if (RandomNum < 2.0)
		{
			AnimInstance->Montage_Play(KnockbackMontage1, 1.0f);
		}
		else
		{
			AnimInstance->Montage_Play(KnockbackMontage2, 1.0f);
		}
	}
}

void ABaseMonster::ServerRPCStunLoop_Implementation()
{
	MulticastRPCStunLoop();
}

void ABaseMonster::MulticastRPCStunLoop_Implementation()
{
	if (!AnimInstance)
	{
		return;
	}

	AnimInstance->StopAllMontages(0.0f);
	if (IsValid(StunMontageLoop))
	{
		AnimInstance->Montage_Play(StunMontageLoop, 1.f);
	}
}

void ABaseMonster::ServerRPCDownLoop_Implementation()
{
	MulticastRPCDownLoop();
}

void ABaseMonster::MulticastRPCDownLoop_Implementation()
{
	if (!AnimInstance)
	{
		return;
	}

	AnimInstance->StopAllMontages(0.0f);
	if (IsValid(DownMontageLoop))
	{
		AnimInstance->Montage_Play(DownMontageLoop, 1.f);
	}
}

void ABaseMonster::ServerRPCShowBossHPBar_Implementation()
{
	MulticastRPCShowBossHPBar();
}

void ABaseMonster::MulticastRPCShowBossHPBar_Implementation()
{
	ATrapperPlayerController* PlayerController = Cast<ATrapperPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	UE_LOG(LogTemp, Warning, TEXT("MultiCastUI!"));
	if (IsValid(PlayerController))
	{
		PlayerController->BossHealthBar->ShowHUD();
		UE_LOG(LogTemp, Warning, TEXT("Show!"));
		/*if (bIsBossStage == true)
		{

		}
		else
		{
			PlayerController->BossHealthBar->HideHUD();
		}*/
	}
}
