// Fill out your copyright notice in the Description page of Project Settings.


#include "Turret/Turret.h"
#include "Components/ArrowComponent.h"
#include "Perception/PawnSensingComponent.h"
#include "Turret/TurretProjectile.h"
#include "Components/StaticMeshComponent.h"
#include "TrapperProject.h"
#include "Monster/BaseMonster.h"
#include "Monster/ADC.h"
#include "Net/UnrealNetwork.h"
#include "Widget/TurretHealthBar.h"
#include "Components/WidgetComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "Framework/TrapperGameMode.h"
#include "NiagaraComponent.h"


ATurret::ATurret()
{
	PrimaryActorTick.bCanEverTick = false;

	DummyRoot = CreateDefaultSubobject<USceneComponent>(TEXT("DummyRoot"));
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	MeshHit = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshHit"));
	ProjectileSpawnLocation = CreateDefaultSubobject<UArrowComponent>(TEXT("ProjectileSpawnLocation"));
	PawnSensing = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensing"));
	WidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("WidgetComponent"));
	MonsterDetectCollider = CreateDefaultSubobject<UBoxComponent>(TEXT("MonsterDetectCollider"));
	MonsterAttackPoint = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MonsterAttackPoint"));
	//MonsterHitCollider = CreateDefaultSubobject<UBoxComponent>(TEXT("MonsterHitCollider"));
	PlayerDetectCollider = CreateDefaultSubobject<USphereComponent>(TEXT("PlayerDetectCollider"));

	NiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("NiagaraComponent"));
	NiagaraComponent->SetupAttachment(Mesh);
	NiagaraComponent->SetAutoActivate(false);

	RootComponent = DummyRoot;
	Mesh->SetupAttachment(RootComponent);
	MeshHit->SetupAttachment(RootComponent);
	ProjectileSpawnLocation->SetupAttachment(Mesh);
	WidgetComponent->SetupAttachment(RootComponent);
	MonsterDetectCollider->SetupAttachment(RootComponent);
	MonsterAttackPoint->SetupAttachment(RootComponent);
	//MonsterHitCollider->SetupAttachment(RootComponent);
	PlayerDetectCollider->SetupAttachment(RootComponent);

	//Add Dynamic 
	MonsterDetectCollider->OnComponentBeginOverlap.AddDynamic(this, &ATurret::OnOverlapBeginMonster);
	MonsterDetectCollider->OnComponentEndOverlap.AddDynamic(this, &ATurret::OnOverlapEndMonster);
}
	
void ATurret::BeginPlay()
{
	Super::BeginPlay();

	CurrentHP = MaxHP;
	HealthHUDRef = Cast<UTurretHealthBar>(WidgetComponent->GetWidget());

	GetWorld()->GetTimerManager().SetTimer(UpdateHealthHUDRotationTimerHandle, this, &ATurret::UpdateHealthHUDRotation, 0.1, true);

	if (HasAuthority())
	{
		PawnSensing->OnSeePawn.AddDynamic(this, &ATurret::OnSeePawn);
		//GetWorld()->GetTimerManager().SetTimer(FireTimerHandle, this, &ATurret::FireAtClosestPawn, 1.f, true);
	}

	PlayerDetectCollider->OnComponentBeginOverlap.AddDynamic(this, &ATurret::OnOverlapBeginPlayer);
	PlayerDetectCollider->OnComponentEndOverlap.AddDynamic(this, &ATurret::OnOverlapEndPlayer);
}

void ATurret::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	GetWorld()->GetTimerManager().ClearAllTimersForObject(this);
	
	if (HasAuthority())
	{
		

		if (DetectADCList.Num() != 0)
		{
			for (const auto& ADCMonster : DetectADCList)
			{
				if (!IsValid(ADCMonster))
				{
					continue;
				}
				//ADC에게 나 부서짐~ 전달해야 함.
				if (ADCMonster->DetectTurretNumber == TargetNumber)
				{
					//destroy Notify
					ADCMonster->DetectTurret = nullptr;
					ADCMonster->bIsTurretDetected = false;
					//UE_LOG(LogTemp, Warning, TEXT("Check ADC List Delete"));
				}
			}
		}
		
		
	}
	
}

void ATurret::BeginDestroy()
{
	Super::BeginDestroy();
	bIsNotSafe = true;

	if (WidgetComponent)
	{
		WidgetComponent->UnregisterComponent();
		WidgetComponent = nullptr;
	}
}

void ATurret::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ATurret, CurrentHP);
}

void ATurret::OnOverlapBeginMonster(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//UE_LOG(LogTemp, Warning, TEXT("Work?"));
	if (HasAuthority())
	{
		TObjectPtr<ABaseMonster>CurretMonster = Cast<ABaseMonster>(OtherActor);

		if (IsValid(CurretMonster))
		{
			if (OtherActor->ActorHasTag("Monster") && CurretMonster->bIsTurretAttack != true)
			{
				if (CurrentHP > 0 && bIsNotSafe == false)
				{
					if (!OtherActor->ActorHasTag("Boss") && !OtherActor->ActorHasTag("Debuffer"))
					{
						CurretMonster->MonsterState = EMonsterStateType::IsTurretDetect;
						CurretMonster->DetectTurret = this;
						CurretMonster->bIsTurretDetected = true;
						//CurretMonster->bIsTurretAttack = true;
						TurretAttackPoint = MonsterAttackPoint->GetComponentLocation();
					}

					if (OtherActor->ActorHasTag("ADC"))
					{
						TObjectPtr<AADC>DetectADC = Cast<AADC>(OtherActor);
						
						if (IsValid(DetectADC))
						{
							DetectADC->DetectTurretNumber = TargetNumber;
							DetectADCList.Add(DetectADC);
							GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, FString::Printf(TEXT("TurretNumber %d Count: %d"),TargetNumber,DetectADCList.Num()));
							
						}
						
					}

				}

			}
		}
	}
}

void ATurret::OnOverlapEndMonster(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (HasAuthority())
	{
		TObjectPtr<AADC>DetectADC = Cast<AADC>(OtherActor);

		if (IsValid(DetectADC))
		{
			if (OtherActor->ActorHasTag("ADC"))
			{
				if (DetectADCList.Find(DetectADC))
				{
					DetectADC->DetectTurret = nullptr;
					DetectADC->bIsTurretDetected = false;
					DetectADCList.Remove(DetectADC);
				}
			}
		}
	}
	
}

void ATurret::OnOverlapBeginPlayer(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepHitResult)
{
	if (!OtherActor->ActorHasTag("Player"))
	{
		//UE_LOG(LogTrap, Warning, TEXT("return"));
		return;
	}

	++OverlapPlayerNum;
	//UE_LOG(LogTrap, Warning, TEXT("OverlapPlayerNum : %d"), OverlapPlayerNum);

	if (OverlapPlayerNum >= 1)
	{
		//UE_LOG(LogTrap, Warning, TEXT("NiagaraComponent Activate"));
		NiagaraComponent->Activate(true);
	}
}

void ATurret::OnOverlapEndPlayer(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (!OtherActor->ActorHasTag("Player"))
	{
		return;
	}

	--OverlapPlayerNum;
	//UE_LOG(LogTrap, Warning, TEXT("OverlapPlayerNum : %d"), OverlapPlayerNum);

	if (OverlapPlayerNum <= 0)
	{
		//UE_LOG(LogTrap, Warning, TEXT("NiagaraComponent DeactivateImmediate"));
		NiagaraComponent->DeactivateImmediate();
	}
}

void ATurret::OnSeePawn(APawn* Pawn)
{
	if (!IsValid(Pawn))
	{
		return;
	}

	//UE_LOG(LogTrap, Warning, TEXT("OnSeePawn"));

	// 몬스터가 아니거나
	// 몬스터지만 디버퍼 몬스터라면 리턴
	if (!Pawn->ActorHasTag("Monster") || Pawn->ActorHasTag("Debuffer") || Pawn->ActorHasTag("PathMonster")
		|| Pawn->ActorHasTag("Tutorial"))
	{
		return;
	}

	if (DetectedPawns.Contains(Pawn))
	{
		return;
	}

	//UE_LOG(LogTrap, Warning, TEXT("Pawn Name %s"), *Pawn->GetName());

	ABaseMonster* BaseMonster = Cast<ABaseMonster>(Pawn);
	if (!BaseMonster)
	{
		return;
	}

	if (BaseMonster->bIsDead)
	{
		return;
	}

	DetectedPawns.Add(Pawn);

	if (!bSetFireTimerNextTick)
	{
		bSetFireTimerNextTick = true;
		GetWorld()->GetTimerManager().SetTimerForNextTick(this, &ATurret::FireAtClosestPawn);
	}
}

float ATurret::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float SuperDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	if (CurrentHP <= 0)
	{
		return SuperDamage;
	}

	float RealDamage = FMath::Min(CurrentHP, SuperDamage);
	CurrentHP -= RealDamage;
	OnRep_CurrentHP();

	// 타워가 처음으로 피격당했을시 대사 및 튜토리얼 출력
	if (HasAuthority() && !GetWorld()->GetGameState<ATrapperGameState>()->bTowerTutorial)
	{
		UE_LOG(LogTemp, Warning, TEXT("Turret Dialog Signal"));
		GetWorld()->GetAuthGameMode<ATrapperGameMode>()->EventHandle(ETrapperEventCode::E_Turret_Tutorial_Dialog);
	}

	return RealDamage;
}

void ATurret::FireAtClosestPawn()
{
	//UE_LOG(LogTrap, Warning, TEXT("FireAtClosestPawn"));

	bSetFireTimerNextTick = false;

	if (DetectedPawns.Num() == 0)
	{
		return;
	}

	APawn* ClosestPawn = nullptr;
	float ClosestDistanceSquared = FLT_MAX;

	FVector TurretLocation = GetActorLocation();

	for (APawn* Pawn : DetectedPawns)
	{
		ABaseMonster* BaseMonster = Cast<ABaseMonster>(Pawn);
		if (IsValid(BaseMonster) && BaseMonster->CurrentHP <= 0)
		{
			//UE_LOG(LogTrap, Warning, TEXT("BaseMonster->CurrentHP : %f"), BaseMonster->CurrentHP);
			continue;
		}

		FVector PawnLocation = Pawn->GetActorLocation();
		float DistanceSquared = FVector::DistSquared(TurretLocation, PawnLocation);

		if (DistanceSquared < ClosestDistanceSquared)
		{
			ClosestDistanceSquared = DistanceSquared;
			ClosestPawn = Pawn;
		}
	}

	if (ClosestPawn)
	{
		FTransform Transform = ProjectileSpawnLocation->GetComponentTransform();
		ATurretProjectile* SpawnedProjectile = GetWorld()->SpawnActor<ATurretProjectile>(Projectile, Transform);
		if (SpawnedProjectile)
		{
			//UE_LOG(LogTrap, Warning, TEXT("ClosestPawn Name %s"), *ClosestPawn->GetName());
			//SpawnedProjectile->SetOwner(this);
			float RealDamage = Damage;
			if (OverlapPlayerNum == 2)
			{
				SpawnedProjectile->EnhancedProjectile();
				RealDamage *= 1.5;
			}

			SpawnedProjectile->SetHomingTarget(ClosestPawn);
			SpawnedProjectile->SetDamage(RealDamage);
			SpawnedProjectile->LaunchingProjectile();

			ABaseMonster* BaseMonster = Cast<ABaseMonster>(ClosestPawn);
			if (BaseMonster)
			{
				BaseMonster->AddProjectile(SpawnedProjectile);
			}
		}
	}

	DetectedPawns.Empty();
}

void ATurret::UpdateHealthHUDRotation()
{
	if (IsValid(HealthHUDRef))
	{
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

				FVector CameraLocation = PlayerCameraManager->GetCameraLocation();
				FVector WidgetLocation = WidgetComponent->GetComponentLocation();
				FRotator LookAtRotation = (CameraLocation - WidgetLocation).Rotation();
				WidgetComponent->SetWorldRotation(LookAtRotation);
			}
		}
	}
}

void ATurret::OnRep_CurrentHP()
{
	if (IsValid(HealthHUDRef))
	{
		HealthHUDRef->UpdateHealth(CurrentHP, MaxHP);
	}

	if (CurrentHP <= 0)
	{
		GetWorld()->GetTimerManager().ClearAllTimersForObject(this);

		SetActorHiddenInGame(true);
		SetActorEnableCollision(false);
		SetLifeSpan(3);
		PawnSensing->Deactivate();

		MulticastRPCSpawnGC();
	}
}

void ATurret::MulticastRPCSpawnGC_Implementation()
{
	SetActorEnableCollision(false);

	AActor* GC = GetWorld()->SpawnActor<AActor>(TurretGC, GetActorTransform());
	if (GC)
	{
		GC->SetLifeSpan(10);
	}
}
