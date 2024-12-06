// Fill out your copyright notice in the Description page of Project Settings.


#include "Arrow.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/Character.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "Sound/SoundCue.h"
#include "Components/AudioComponent.h"
#include "TrapperProject.h"

#include "Character/TrapperPlayer.h"
#include "Upgrade/UpgradeTable.h"
#include "Framework/TrapperGameState.h"

AArrow::AArrow()
{
	PrimaryActorTick.bCanEverTick = true;

	// 컴포넌트 부착
	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Collision Box"));
	RootComponent = CollisionBox;
	CollisionBox->SetBoxExtent(FVector(50, 3, 3), false);
	CollisionBox->SetUseCCD(true);

	AwakenCollisionShepre = CreateDefaultSubobject<USphereComponent>(TEXT("AwakenCollisionShepre"));
	AwakenCollisionShepre->SetupAttachment(RootComponent);
	AwakenCollisionShepre->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	AwakenCollisionShepre->SetRelativeLocation(FVector(40,0,0));
	AwakenCollisionShepre->SetSphereRadius(240.f);

	ArrowMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Arrow Mesh"));
	ArrowMesh->SetupAttachment(RootComponent);

	// 델리게이트
	CollisionBox->OnComponentBeginOverlap.AddDynamic(this, &AArrow::OnOverlapBegin);
	CollisionBox->OnComponentHit.AddDynamic(this, &AArrow::OnArrowHit);

	// 이펙트
	TrailAttachPoint = CreateDefaultSubobject<USceneComponent>(TEXT("TrailAttachPoint"));
	TrailAttachPoint->SetupAttachment(ArrowMesh);
	TrailAttachPoint->AddLocalOffset(FVector(-41.1, 0, 0));

	TrailAttachPointOffset = CreateDefaultSubobject<USceneComponent>(TEXT("TrailAttachPointOffset"));
	TrailAttachPointOffset->SetupAttachment(ArrowMesh);
	TrailAttachPointOffset->AddLocalOffset(FVector(-41.1, 0, 0));

	EffectSpawnPoint = CreateDefaultSubobject<USceneComponent>(TEXT("EffectSpawnPoint"));
	EffectSpawnPoint->SetupAttachment(ArrowMesh);
	EffectSpawnPoint->AddLocalOffset(FVector(48, 0, 0));

	AudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("AudioComponent"));
	AudioComponent->SetupAttachment(RootComponent);

	// 태그
	Tags.Add(FName("DefaultArrow"));
	Tags.Add(FName("Arrow"));

	bReplicates = true;
	bAlwaysRelevant = true;  //화살 발사 위치 동기화를 위함.
}


void AArrow::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AArrow, bIsActive);
}


void AArrow::BeginPlay()
{
	Super::BeginPlay();

	CollisionBox->SetCanEverAffectNavigation(false);

	if(HasAuthority())
	{
		SetReplicates(true);
		SetReplicateMovement(true);
		bAlwaysRelevant = true;
	}

	OwningActor = Cast<ATrapperPlayer>(GetOwner());
	GameState = GetWorld()->GetGameState<ATrapperGameState>();

	if(HasAuthority())
	{
		if(IsValid(OwningActor) && OwningActor->IsLocallyControlled())
		{
			bIsSpawnedBy1P = true;
		}
	}
	else
	{
		if(IsValid(OwningActor) && !OwningActor->IsLocallyControlled())
		{
			bIsSpawnedBy1P = true;
		}
	}

	SpawnTrailNiagara();
	LastTrailAttachPosition = TrailAttachPoint->GetComponentLocation();

	if(bIsAwaken)
	{
		AwakenCollisionShepre->OnComponentBeginOverlap.AddDynamic(this, &AArrow::OnExplosionOverlapBegin);
		AwakenCollisionShepre->OnComponentEndOverlap.AddDynamic(this, &AArrow::OnExplosionOverlapEnd);
	}
	else
	{
		AwakenCollisionShepre->DestroyComponent();
	}
}


void AArrow::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if(IsValid(TrailEffectRef))
	{
		TrailEffectRef->DestroyComponent();
	}

	if (IsValid(GameState))
	{
		if (IsValid(OwningActor))
		{
			GameState->CheckKillCount(OwningActor->GetController(), this);
		}
	}

	Super::EndPlay(EndPlayReason);
}

void AArrow::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	TrailAttachPointOffset->SetWorldLocation(LastTrailAttachPosition);
	LastTrailAttachPosition = TrailAttachPoint->GetComponentLocation();
}

///   ---------------------------------------------------------------------------------------------------
///											Collision
///   ---------------------------------------------------------------------------------------------------


void AArrow::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(!HasAuthority())
	{
		return;
	}

	UE_LOG(LogBow, Warning, TEXT("Arrow Overlap <-> [ Other Actor : %s ],   [ Other Comp : %s ]")
		, *OtherActor->GetName(), *OtherComp->GetName());

	// 몬스터일 때
	if(OtherActor->ActorHasTag("Monster") && bIsActive)
	{
		MulticastRPCSpawnHitParticle(SweepResult.Location);

		auto MyOwnerInstigator = GetOwner()->GetInstigatorController();
		auto DamageTypeCalss = UDamageType::StaticClass();

		bool bIsHeadShoot = false;
		if(OtherComp->ComponentHasTag("MonsterHead") && !GameState->bIsRemovedHeadShoot)
		{
			bIsHeadShoot = true;
		}

		if(bIsHeadShoot)
		{
			UGameplayStatics::ApplyDamage(OtherActor, Damage, MyOwnerInstigator, this, DamageTypeCalss);
			if(IsValid(OwningActor))
			{
				OwningActor->ShowHitFeedbackHUD(true, bIsFullCharged);
			}
		}
		else
		{
			UGameplayStatics::ApplyDamage(OtherActor, Damage, MyOwnerInstigator, this, DamageTypeCalss);
			if(IsValid(OwningActor))
			{
				OwningActor->ShowHitFeedbackHUD(false, bIsFullCharged);
			}
		}

		if(!(GameState->bIsArrowPenetrable))
		{
			DeactivateArrow();
		}
	}
}

void AArrow::OnArrowHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if(!HasAuthority())
	{
		return;
	}

	UE_LOG(LogBow, Warning, TEXT("Arrow Hit <-> [ Other Actor : %s ],   [ Other Comp : %s ]")
		, *OtherActor->GetName(), *OtherComp->GetName());

	if(OtherActor->ActorHasTag("StaticMesh") && bIsActive)
	{
		MulticastRPCSpawnHitParticle(Hit.Location);

		// Static Actor에 부착시키기
		FAttachmentTransformRules attachRule = FAttachmentTransformRules(
			EAttachmentRule::KeepWorld, EAttachmentRule::KeepWorld, EAttachmentRule::KeepWorld, true);
		this->AttachToActor(OtherActor, attachRule);

		// 충돌하지 않도록 변경
		CollisionBox->SetCollisionProfileName("NoCollision", false);
		CollisionBox->SetGenerateOverlapEvents(false);

		bIsActive = false;
		if(IsValid(ProjectileCompRef))
		{
			ProjectileCompRef->DestroyComponent();
		}
	}

	// 몬스터일 때
	else if(OtherActor->ActorHasTag("Monster") && bIsActive)
	{
		MulticastRPCSpawnHitParticle(Hit.Location);

		auto MyOwnerInstigator = GetOwner()->GetInstigatorController();
		auto DamageTypeCalss = UDamageType::StaticClass();

		bool bIsHeadShoot = false;
		if(OtherComp->ComponentHasTag("MonsterHead") && !GameState->bIsRemovedHeadShoot)
		{
			bIsHeadShoot = true;
		}

		if(bIsHeadShoot)
		{
			UGameplayStatics::ApplyDamage(OtherActor, Damage, MyOwnerInstigator, this, DamageTypeCalss);
			if(IsValid(OwningActor))
				OwningActor->ShowHitFeedbackHUD(true, bIsFullCharged);
		}
		else
		{
			UGameplayStatics::ApplyDamage(OtherActor, Damage, MyOwnerInstigator, this, DamageTypeCalss);
			if(IsValid(OwningActor))
				OwningActor->ShowHitFeedbackHUD(false, bIsFullCharged);
		}

		if(bIsAwaken)
		{
			MulticastRPCSpawnExplosionParticle(Hit.Location);
			GetWorld()->GetTimerManager().SetTimerForNextTick(this, &AArrow::ApplyExplosionDamage);
		}
		DeactivateArrow();
	}

	// 자성 기둥
	else if(OtherActor->ActorHasTag("MagneticPillar"))
	{
		MulticastRPCSpawnHitParticle(Hit.Location);
		CollisionBox->SetCollisionProfileName("NoCollision");
		SetLifeSpan(0.5);
	}

	// 트랩
	else if(OtherActor->ActorHasTag("ArrowTrap"))
	{
		MulticastRPCSpawnHitParticle(Hit.Location);
		CollisionBox->SetCollisionProfileName("NoCollision");
		SetLifeSpan(0.5);
	}

	// 마지막(플레이어만 아니면 됨)
	else if(OtherActor != Owner && bIsActive)
	{
		MulticastRPCSpawnHitParticle(Hit.Location);
		DeactivateArrow();
	}
}

void AArrow::OnExplosionOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(!OtherActor->ActorHasTag("Monster") || OtherComp->GetName() == "HeadShootCollider")
	{
		return;
	}

	if(!OverlapingMonsters.Contains(OtherActor))
	{
		OverlapingMonsters.Add(OtherActor);
	}
}

void AArrow::OnExplosionOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if(!OtherActor->ActorHasTag("Monster") || OtherComp->GetName() == "HeadShootCollider")
	{
		return;
	}

	if(OverlapingMonsters.Contains(OtherActor))
	{
		OverlapingMonsters.Remove(OtherActor);
	}
}

///   ---------------------------------------------------------------------------------------------------
///							               Fire, Activation
///   ---------------------------------------------------------------------------------------------------

void AArrow::FireArrow(FVector DirectionIn, float StrengthIn)
{
	ActivateTrailNiagara();
	MulticastRPCSetClientVisibility();
	SetLifeSpan(LifeSpan);

	if(bIsAwaken)
	{
		SpawnAwakenEffect();
	}

	Direction = DirectionIn;
	Strength = StrengthIn;

	GetWorld()->GetTimerManager().SetTimerForNextTick(this, &AArrow::FireArrowReal);
}

void AArrow::FireArrowReal()
{
	ProjectileCompRef = Cast<UProjectileMovementComponent>
		(AddComponentByClass(UProjectileMovementComponent::StaticClass(), true, FTransform(), false));

	Damage = FMath::Lerp(MinDamage, MaxDamage, Strength);
	Damage *= GameState->ArrowDamage;

	ProjectileCompRef->Velocity = FMath::Lerp(MinSpeed, MaxSpeed, Strength) * Direction * GameState->ArrowSpeed;
	ProjectileCompRef->ProjectileGravityScale = FMath::Lerp(MaxGravity, MinGravity, Strength);
	ProjectileCompRef->bRotationFollowsVelocity = true;

	bIsActive = true;

	if(GameState->bIsArrowPenetrable && !bIsAwaken)
	{
		CollisionBox->SetCollisionProfileName("PenetrableArrow");
	}
	else
	{
		CollisionBox->SetCollisionProfileName("Arrow");
	}

	if(bIsAwaken)
	{
		AwakenCollisionShepre->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		AwakenCollisionShepre->SetCollisionProfileName("OverlapResponsOnlyMonster");
	}
}

void AArrow::DeactivateArrow()
{
	bIsActive = false;

	if(IsValid(ProjectileCompRef))
	{
		ProjectileCompRef->DestroyComponent();
	}

	CollisionBox->SetCollisionProfileName("NoCollision");
	SetLifeSpan(0.5);
}

///   ---------------------------------------------------------------------------------------------------
///							             VFX
///   ---------------------------------------------------------------------------------------------------


void AArrow::SpawnTrailNiagara()
{
	if(TrailNiagara)
	{
		TrailEffectRef = UNiagaraFunctionLibrary::SpawnSystemAttached(
			TrailNiagara, TrailAttachPointOffset, //TrailAttachPointOffset
			NAME_None,
			FVector(0.f), FRotator(0.f),
			EAttachLocation::Type::KeepRelativeOffset, false, false);

		TrailEffectRef->SetIsReplicated(false);
		TrailEffectRef->Deactivate();
	}
}

void AArrow::SpawnAwakenEffect()
{
	if(IsValid(AwakenParticle1P) && bIsSpawnedBy1P)
	{
		UNiagaraFunctionLibrary::SpawnSystemAttached(
			AwakenParticle1P, EffectSpawnPoint,
			NAME_None,
			FVector(0.f), FRotator(0.f),
			EAttachLocation::Type::KeepRelativeOffset, true, true);
	}
	else if(IsValid(AwakenParticle2P) && !bIsSpawnedBy1P)
	{
		UNiagaraFunctionLibrary::SpawnSystemAttached(
			AwakenParticle2P, EffectSpawnPoint,
			NAME_None,
			FVector(0.f), FRotator(0.f),
			EAttachLocation::Type::KeepRelativeOffset, true, true);
	}

	if(IsValid(FlySound))
	{
		AudioComponent->SetSound(FlySound);
		AudioComponent->Play();
	}
}

void AArrow::SpawnMagneticEffect()
{
	if(MagneticEffect1P && IsSpawnedBy1P())
	{
		MagneticEffectRef = UNiagaraFunctionLibrary::SpawnSystemAttached(
			MagneticEffect1P, EffectSpawnPoint,
			NAME_None,
			FVector(0.f), FRotator(0.f),
			EAttachLocation::Type::KeepRelativeOffset, true, true);

		MagneticEffectRef->SetRelativeScale3D(FVector(0.2, 0.2, 0.2));
	}
	else if(MagneticEffect2P && !IsSpawnedBy1P())
	{
		MagneticEffectRef = UNiagaraFunctionLibrary::SpawnSystemAttached(
			MagneticEffect2P, EffectSpawnPoint,
			NAME_None,
			FVector(0.f), FRotator(0.f),
			EAttachLocation::Type::KeepRelativeOffset, true, true);

		MagneticEffectRef->SetRelativeScale3D(FVector(0.2, 0.2, 0.2));
	}
}

void AArrow::ApplyExplosionDamage()
{
	auto MyOwnerInstigator = GetOwner()->GetInstigatorController();
	auto DamageTypeCalss = UDamageType::StaticClass();

	TArray<TObjectPtr<AActor>> Temp(OverlapingMonsters);

	for(int i = 0; i < Temp.Num(); i++)
	{
		UGameplayStatics::ApplyDamage(Temp[i], Damage, MyOwnerInstigator, this, DamageTypeCalss);
	}

	//DrawDebugSphere(GetWorld(), AwakenCollisionShepre->GetComponentLocation(),AwakenCollisionShepre->GetScaledSphereRadius(), 16, FColor::Red, true);
}

void AArrow::ActivateTrailNiagara()
{
	if(IsValid(TrailEffectRef))
	{
		TrailEffectRef->Activate();
	}
}

///   ---------------------------------------------------------------------------------------------------
///							              RPC
///   ---------------------------------------------------------------------------------------------------

void AArrow::MulticastRPCSetClientVisibility_Implementation()
{
	if(!HasAuthority() && !bIsSpawnedBy1P)
	{
		ArrowMesh->SetVisibility(false);
		if(IsValid(MagneticEffectRef))
		{
			MagneticEffectRef->Deactivate();
		}
	}
	else
	{
		ActivateTrailNiagara();
	}
}

void AArrow::MulticastRPCSpawnHitParticle_Implementation(FVector Location)
{
	if(bIsSpawnedBy1P && IsValid(HitEffect1P))
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(), HitEffect1P, Location);
	}
	else if(IsValid(HitEffect2P))
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(), HitEffect2P, Location);
	}

	// 사운드
	if(IsValid(HitSound))
	{
		// 특정 지점에서 사운드 재생
		UGameplayStatics::PlaySoundAtLocation(this, HitSound, GetActorLocation());
	}
}

void AArrow::MulticastRPCSpawnExplosionParticle_Implementation(FVector Location)
{
	if(IsValid(AwakenExplosion1P) && bIsSpawnedBy1P)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(), AwakenExplosion1P, Location);
	}

	else if(IsValid(AwakenExplosion2P) && !bIsSpawnedBy1P)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(), AwakenExplosion2P, Location);
	}
}

