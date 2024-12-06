// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interact.generated.h"

UCLASS()
class TRAPPERPROJECT_API AInteract : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AInteract();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual void CheckActivate(int32 Value, bool Is1P = true);
	 
public:
	uint8 bFirstPlayerActive : 1;
	uint8 bSecondPlayerActive : 1;
};
