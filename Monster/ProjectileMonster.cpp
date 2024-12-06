// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileMonster.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Trap/TrapBase.h"

// Sets default values
AProjectileMonster::AProjectileMonster()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	projectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Projectile Mesh"));
	RootComponent = projectileMesh;

	projectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Projectile Movement"));
	projectileMovement->MaxSpeed = 100.f;
	projectileMovement->InitialSpeed = 100.f;
}

// Called when the game starts or when spawned
void AProjectileMonster::BeginPlay()
{
	Super::BeginPlay();
	
	projectileMesh->OnComponentHit.AddDynamic(this, &AProjectileMonster::OnHit);

}

// Called every frame
void AProjectileMonster::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AProjectileMonster::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	auto myOwner = GetOwner();
	UE_LOG(LogTemp, Warning, TEXT("Hit Comp : %s"), *OtherActor->GetName());
	UE_LOG(LogTemp, Warning, TEXT("Owner : %s"), *myOwner->GetName());
	
	if (myOwner == nullptr)
	{
		//UE_LOG(LogTemp, Warning, TEXT("Is nullptr?"));
		Destroy();
		return;
	}

	
	auto myOwnerInstigator = myOwner->GetInstigatorController();
	auto damageTypeClass = UDamageType::StaticClass();

	if (OtherActor && OtherActor != this && OtherActor != myOwner)
	{
		//UE_LOG(LogTemp, Warning, TEXT("Is Work?"));
		UGameplayStatics::ApplyDamage(OtherActor, damage, myOwnerInstigator, this, damageTypeClass);	

	}

	Destroy();
}
