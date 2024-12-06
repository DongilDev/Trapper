// Fill out your copyright notice in the Description page of Project Settings.


#include "Tower.h"

#include "Kismet/GameplayStatics.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Net/UnrealNetwork.h"

#include "Tower/TowerProjectile.h"
#include "Monster/BaseMonster.h"
#include "NiagaraComponent.h"
#include "NiagaraSystem.h"
#include "Framework/TrapperGameMode.h"
#include "Framework/TrapperPlayerController.h"


// Sets default values
ATower::ATower()
	: TowerHP(0)
	, MaxHP(0)
	, FireRate(0)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComp = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = RootComp;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(RootComp);

	TopMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TopMesh"));
	TopMesh->SetupAttachment(RootComp);

	BottomMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BottomMesh"));
	BottomMesh->SetupAttachment(RootComp);

	ProjectileSpawn = CreateDefaultSubobject<USceneComponent>(TEXT("Projectile Spawn"));
	ProjectileSpawn->SetupAttachment(RootComp);

	ReachTriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("ReachTrigger"));
	ReachTriggerBox->SetupAttachment(RootComp);

	TowerEffect = CreateDefaultSubobject<UNiagaraComponent>(TEXT("TowerEffect"));
	TowerEffect->bAutoActivate = false;
	TowerEffect->SetupAttachment(RootComp);

	TowerHP = MaxHP;
	FireRate = 2.f;

	//Add Dynamic
	ReachTriggerBox->OnComponentBeginOverlap.AddDynamic(this, &ATower::OnOverlapBeginMonster);
}

// Called when the game starts or when spawned
void ATower::BeginPlay()
{
	Super::BeginPlay();
	
	//Tower�� �ǰ� �پ�� ���, ������ �ϵ��� �Ѵ�.
	GetWorldTimerManager().SetTimer(FireRateTimerHandle, this, &ATower::CheckFireCondition, FireRate, true);

	TowerHP = MaxHP;

	UE_LOG(LogTemp, Warning, TEXT("ATower BeginPlay"));

	if (HasAuthority())
	{
		//GetWorldTimerManager().SetTimer(GameOverHandle, this, &ATower::GameOverDebug, 1.f, false, 10.f);
	}
}

// Called every frame
void ATower::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ATower::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ATower, TowerHP);
}

void ATower::GameOverDebug()
{
	UGameplayStatics::ApplyDamage(this, 99999.f, nullptr, this, UDamageType::StaticClass());
}

float ATower::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float Damage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	Damage = FMath::Min(TowerHP, Damage);
	ATrapperGameState* TrapperGameState = GetWorld()->GetGameState<ATrapperGameState>();

	if (TowerHP <= 0 || TrapperGameState->bIsInvincibilityMode)
	{
		return 0.f;
	}
	
	if (HasAuthority())
	{
		TowerHP -= Damage;
		OnRep_HpChanged();

		if (TowerHP <= 0.f)
		{
			Cast<ATrapperGameMode>(GetWorld()->GetAuthGameMode())->SetGameProgress(EGameProgress::GameOver);
		}

		UE_LOG(LogTemp, Warning, TEXT("Tower Health : %f"), TowerHP);
	}

	return Damage;
}

void ATower::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{

}

void ATower::CheckFireCondition()
{

	////Tower�� �ǰ� ���̱� �����ϸ�, Tower�� ������ �Ѵ�.
	//if (CurrentHP < 1000)
	//{
	//	Fire();
	//}
}

void ATower::Fire()
{
	FVector spawnLocation = ProjectileSpawn->GetComponentLocation();
	FRotator spawnRotation = ProjectileSpawn->GetComponentRotation();

	auto projectile = GetWorld()->SpawnActor<ATowerProjectile>(TowerProjectileClass, spawnLocation, spawnRotation);
	projectile->SetOwner(this);
}

void ATower::HealingTower(float Value)
{
	if (HasAuthority())
	{
		TowerHP += MaxHP * Value;
		TowerHP = FMath::Clamp(TowerHP, 0.f, MaxHP);

		OnRep_HpChanged();
	}
	else
	{
		ServerRPCHealingTower(Value);
	}
}

void ATower::ServerRPCHealingTower_Implementation(float Value)
{
	TowerHP += MaxHP * Value;
	TowerHP = FMath::Clamp(TowerHP, 0.f, MaxHP);

	OnRep_HpChanged();
}

void ATower::MulticastUpgradeTower_Implementation(int32 TowerLevel)
{
	UNiagaraSystem* SelectedEffect = nullptr;

	switch (TowerLevel)
	{
	case 1:
		SelectedEffect = TowerEffect1;
		break;
	case 2:
		SelectedEffect = TowerEffect2;
		break;
	case 3:
		SelectedEffect = TowerEffect3;
		break;
	case 4:
		SelectedEffect = TowerEffect4;
		break;
	case 5:
		SelectedEffect = TowerEffect5;
		break;
	case 6:
		SelectedEffect = TowerEffect6;
		break;
	default:
		break;
	}

	if (SelectedEffect)
	{
		TowerEffect->SetAsset(SelectedEffect);
		TowerEffect->Activate(true);
	}
}

void ATower::OnOverlapBeginMonster(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//UE_LOG(LogTemp, Warning, TEXT("Begin Overlap"));
	if (OtherActor->ActorHasTag("Monster"))
	{
		Cast<ABaseMonster>(OtherActor)->MonsterState = EMonsterStateType::IsTowerDetect;
		Cast<ABaseMonster>(OtherActor)->Tower = this;
		Cast<ABaseMonster>(OtherActor)->bIsTowerDetected = true;
		//MonsterState = 	
		//UE_LOG(LogTemp, Warning, TEXT("Monster Detect"));
	}
}

void ATower::OnRep_HpChanged()
{
	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		ATrapperPlayerController* PlayerController = Cast<ATrapperPlayerController>(Iterator->Get());
		if (PlayerController && PlayerController->IsLocalController())
		{
			PlayerController->SetTowerHPBar(TowerHP, MaxHP);
		}
	}
}

