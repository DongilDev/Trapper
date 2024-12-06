// Fill out your copyright notice in the Description page of Project Settings.

#include "TowerProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"


// Sets default values
ATowerProjectile::ATowerProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Projectile Mesh"));
	RootComponent = ProjectileMesh;

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Projectile Movement"));
	ProjectileMovement->MaxSpeed = 100.f;
	ProjectileMovement->InitialSpeed = 100.f;
}

// Called when the game starts or when spawned
void ATowerProjectile::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ATowerProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ATowerProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	auto MyOwner = GetOwner();
	
	UE_LOG(LogTemp, Warning, TEXT("Onwer : %s"), *MyOwner->GetName());

	if (MyOwner == nullptr)
	{
		//UE_LOG(LogTemp, Warning, TEXT("Is nullptr?"));
		Destroy();
		return;
	}


	auto myOwnerInstigator = MyOwner->GetInstigatorController();
	auto damageTypeClass = UDamageType::StaticClass();

	if (OtherActor && OtherActor != this && OtherActor != MyOwner)
	{
		UE_LOG(LogTemp, Warning, TEXT("Is Work?"));
		UGameplayStatics::ApplyDamage(OtherActor, Damage, myOwnerInstigator, this, damageTypeClass);
	}

	Destroy();
}

