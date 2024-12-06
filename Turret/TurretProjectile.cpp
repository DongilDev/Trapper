// Fill out your copyright notice in the Description page of Project Settings.


#include "Turret/TurretProjectile.h"
#include "NiagaraSystem.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/StaticMeshComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"
#include "TrapperProject.h"
#include "Kismet/GameplayStatics.h"
#include "Monster/BaseMonster.h"


ATurretProjectile::ATurretProjectile()
{
	PrimaryActorTick.bCanEverTick = false;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	NormalNiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("NormalNiagaraComponent"));
	EnhanceNiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("EnhanceNiagaraComponent"));

	RootComponent = Mesh;
	NormalNiagaraComponent->SetupAttachment(Mesh);
	EnhanceNiagaraComponent->SetupAttachment(Mesh);
}

void ATurretProjectile::BeginPlay()
{
	Super::BeginPlay();

	//ProjectileMovement->bIsHomingProjectile = true;
	//ProjectileMovement->HomingAccelerationMagnitude = 6000.f;

	//UGameplayStatics::PlaySoundAtLocation(this, SpawnSound, GetActorLocation());

	Mesh->OnComponentBeginOverlap.AddDynamic(this, &ATurretProjectile::OnOverlapBeginCharacter);
}

void ATurretProjectile::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	GetWorld()->GetTimerManager().ClearAllTimersForObject(this);
}

void ATurretProjectile::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ATurretProjectile, HomingTarget);
	DOREPLIFETIME(ATurretProjectile, bEnhance);
}

void ATurretProjectile::StopHomingTarget()
{
	if (IsValid(ProjectileMovement))
	{
		ProjectileMovement->bIsHomingProjectile = false;
	}

	GetWorld()->GetTimerManager().ClearTimer(RotationUpdateTimerHandle);
}

void ATurretProjectile::LaunchingProjectile()
{
	if (!IsValid(HomingTarget))
	{
		return;
	}

	ProjectileMovement->HomingTargetComponent = HomingTarget->GetRootComponent();

	if (bEnhance)
	{
		EnhanceNiagaraComponent->Activate(true);
	}
	else
	{
		NormalNiagaraComponent->Activate(true);
	}

	GetWorld()->GetTimerManager().SetTimer(RotationUpdateTimerHandle, this, &ATurretProjectile::UpdateRotationTimer, 0.1, true);
}

void ATurretProjectile::OnOverlapBeginCharacter(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepHitResult)
{
	if (bHitCharacter)
	{
		return;
	}

	if (OtherActor->ActorHasTag("Debuffer") || OtherActor->ActorHasTag("PathMonster"))
	{
		return;
	}

	/*UE_LOG(LogTrap, Warning, TEXT("OnOverlapBeginCharacter"));
	UE_LOG(LogTrap, Warning, TEXT("OtherActor Name : %s"), *OtherActor->GetName());*/

	bHitCharacter = true;

	if (bEnhance)
	{
		EnhanceNiagaraComponent->DeactivateImmediate();
	}
	else
	{
		NormalNiagaraComponent->DeactivateImmediate();
	}

	UGameplayStatics::PlaySoundAtLocation(this, HitSound, GetActorLocation());

	UNiagaraComponent* SpawnedNiagara = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
		GetWorld(),
		HitNiagara,
		SweepHitResult.ImpactPoint,
		FRotator::ZeroRotator,
		FVector(1.0f),
		true,
		true,
		ENCPoolMethod::None,
		true
	);

	if (SpawnedNiagara)
	{
		SpawnedNiagara->OnSystemFinished.AddDynamic(this, &ATurretProjectile::OnNiagaraFinished);
	}

	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Mesh->SetHiddenInGame(true);

	GetWorld()->GetTimerManager().ClearTimer(RotationUpdateTimerHandle);

	if (HasAuthority())
	{
		if (OtherActor->ActorHasTag("Monster"))
		{
			ABaseMonster* BaseMonster = Cast<ABaseMonster>(HomingTarget);
			if (BaseMonster)
			{
				BaseMonster->RemoveProjectile(this);

				auto DamageTypeClass = UDamageType::StaticClass();
				UGameplayStatics::ApplyDamage(OtherActor, Damage, nullptr, this, DamageTypeClass);
			}
		}
	}
}

void ATurretProjectile::OnNiagaraFinished(UNiagaraComponent* PSystem)
{
	//UE_LOG(LogTrap, Warning, TEXT("OnNiagaraFinished"));
	if (HasAuthority())
	{
		Destroy();
	}
}

void ATurretProjectile::UpdateRotationTimer()
{
	if (!IsValid(HomingTarget))
	{
		return;
	}

	FVector ProjectileLocation = GetActorLocation();
	FVector TargetLocation = HomingTarget->GetActorLocation();
	FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(ProjectileLocation, TargetLocation);
	SetActorRotation(LookAtRotation);
}

void ATurretProjectile::OnRep_HomingTarget()
{
	if (IsValid(HomingTarget))
	{
		ProjectileMovement->HomingTargetComponent = HomingTarget->GetRootComponent();

		if (bEnhance)
		{
			EnhanceNiagaraComponent->Activate(true);
		}
		else
		{
			NormalNiagaraComponent->Activate(true);
		}
	}
}

void ATurretProjectile::MulticastRPCSafeClearTimer_Implementation(FVector ImpactPoint)
{
	if (!HasAuthority())
	{
		if (!bHitCharacter)
		{
			bHitCharacter = true;

			if (bEnhance)
			{
				EnhanceNiagaraComponent->DeactivateImmediate();
			}
			else
			{
				NormalNiagaraComponent->DeactivateImmediate();
			}

			UNiagaraFunctionLibrary::SpawnSystemAttached(
				HitNiagara, RootComponent,
				NAME_None,
				FVector(0.f), FRotator(0.f),
				EAttachLocation::Type::KeepRelativeOffset, true, true);

			UGameplayStatics::PlaySoundAtLocation(this, HitSound, GetActorLocation());

			/*UNiagaraFunctionLibrary::SpawnSystemAtLocation(
				GetWorld(),
				HitNiagara,
				ImpactPoint,
				FRotator::ZeroRotator,
				FVector(1.0f),
				true,
				true,
				ENCPoolMethod::None,
				true
			);*/

			Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			Mesh->SetHiddenInGame(true);
		}
	}
}

