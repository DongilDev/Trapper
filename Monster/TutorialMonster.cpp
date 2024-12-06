// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster/TutorialMonster.h"

void ATutorialMonster::BeginPlay()
{
	Super::BeginPlay();

	MaxHP = 99999;
	CurrentHP = MaxHP;
}

void ATutorialMonster::Teleport(FVector Location)
{
	this->TeleportTo(Location, GetActorRotation());

}
