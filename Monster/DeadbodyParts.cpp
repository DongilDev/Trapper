// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster/DeadbodyParts.h"

#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"

// Sets default values
ADeadbodyParts::ADeadbodyParts()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	DeadbodyPart = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DeadbodyPart"));
	RootComponent = DeadbodyPart;

	DissolveParticlePosition = CreateDefaultSubobject<USceneComponent>(TEXT("DissolveParticlePosition"));
	DissolveParticlePosition->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void ADeadbodyParts::BeginPlay()
{
	Super::BeginPlay();
	
	FVector SpawnLocation = DeadbodyPart->GetComponentLocation();

	DeadbodyPart->SetCanEverAffectNavigation(false);

	DissolveParticle = LoadObject<UNiagaraSystem>(nullptr, TEXT("/Script/Niagara.NiagaraSystem'/Game/Effect/AdvancedNiagaraVFX/Particles/Bonus/NS_Dissolve.NS_Dissolve'"));
	if (DissolveParticle)
	{
		DissolveParticleComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(
			DissolveParticle,
			DissolveParticlePosition,
			NAME_None,
			SpawnLocation,
			FRotator::ZeroRotator,
			EAttachLocation::KeepRelativeOffset,
			false
		);
	} 

	DissolveParticlePosition->SetVisibility(false, true);
	DissolveParticleComponent->Deactivate();
}

void ADeadbodyParts::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	

}

void ADeadbodyParts::PlayParticle()
{
	//UE_LOG(LogTemp, Warning, TEXT("Work Check"));

	if (IsValid(DissolveParticleComponent))
	{
		DissolveParticlePosition->SetVisibility(true, true);
		DissolveParticleComponent->Activate();
		//UE_LOG(LogTemp, Warning, TEXT("Work Check"));
	}
	
}
