// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Quest/Interact.h"
#include "BlockBox.generated.h"

/**
 * 
 */
UCLASS()
class TRAPPERPROJECT_API ABlockBox : public AInteract
{
	GENERATED_BODY()
	
public:
	ABlockBox();

public:
	UPROPERTY(EditDefaultsOnly, Category = "Components")
	TObjectPtr<class UBoxComponent> Box;

};
