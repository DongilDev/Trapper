// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Monster/BaseMonster.h"
#include "PathMonster.generated.h"

/**
 * 
 */
UCLASS()
class TRAPPERPROJECT_API APathMonster : public ABaseMonster
{
	GENERATED_BODY()
	
public:
	APathMonster();

protected:
	virtual void BeginPlay() override;

protected:
	virtual float TakeDamage(float DamageAmout, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

};
