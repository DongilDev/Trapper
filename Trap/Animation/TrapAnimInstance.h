// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "TrapAnimInstance.generated.h"

/**
 *
 */
UCLASS()
class TRAPPERPROJECT_API UTrapAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
public:
	UTrapAnimInstance();

	void SetActive(bool Value) { bIsActivate = Value; }

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Trap")
	uint8 bIsActivate : 1;
};
