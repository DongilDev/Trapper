// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster/PathMonster.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "Ping/MarkerComponent/MapMarkerComponent.h"

APathMonster::APathMonster()
{

}

void APathMonster::BeginPlay()
{
	Super::BeginPlay();

	MaxHP = 1000000;
	CurrentHP = MaxHP;

	//MonsterSpeed = 10000;
	//GetCharacterMovement()->MaxWalkSpeed = MonsterSpeed;

	MonsterState = EMonsterStateType::GoToTower;

	//MapMarker->SetActive(false);
	//MapMarker->Deactivate();
	//MapMarker->DestroyComponent();
	//MapMarker->UnregisterComponent();
	//MapMarker->SetType(EMapObjectType::None);
	//MapMarker->SetVisibility(false);
}


float APathMonster::TakeDamage(float DamageAmout, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	//Super::TakeDamage(DamageAmout, DamageEvent, EventInstigator, DamageCauser);

	return 0.0f;
}
