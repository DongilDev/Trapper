// Fill out your copyright notice in the Description page of Project Settings.


#include "Quest/QuestEffect.h"
#include "Particles/ParticleSystemComponent.h"
#include "Components/BoxComponent.h"

// Sets default values
AQuestEffect::AQuestEffect()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Box = CreateDefaultSubobject<UBoxComponent>(TEXT("Box"));
	RootComponent = Box;

	QuestPingEffect = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("QuestEffect"));
	QuestPingEffect->bAutoActivate = false;
	QuestPingEffect->SetupAttachment(Box);

	static ConstructorHelpers::FObjectFinder<UParticleSystem> Effect(TEXT("/Script/Engine.ParticleSystem'/Game/RPGEffects/Particles/P_Status_Quest.P_Status_Quest'"));
	if (Effect.Succeeded() && Effect.Object)
	{
		QuestPingEffect->SetTemplate(Effect.Object);
	}
}

// Called when the game starts or when spawned
void AQuestEffect::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AQuestEffect::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AQuestEffect::SetQuestEffect(bool Value, FVector Location)
{
	if(!QuestPingEffect) return;

	Location.Z += 250.f;
	SetActorLocation(Location);

	if (Value)
	{
		QuestPingEffect->Activate();
	}
	else
	{
		QuestPingEffect->Deactivate();
	}
}