// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BossItem.generated.h"

UCLASS()
class TRAPPERPROJECT_API ABossItem : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABossItem();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	UPROPERTY(EditAnywhere, Category = "Item")
	TObjectPtr<class UStaticMeshComponent> Item;

	UPROPERTY(EditAnywhere, Category = "Dummy")
	TObjectPtr<class UStaticMeshComponent> DummyItem;

};
