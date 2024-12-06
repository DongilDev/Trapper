// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Item.generated.h"

UCLASS()
class TRAPPERPROJECT_API AItem : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AItem();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	void BeDrawnToPlayer();
	void SetPlayer(class ATrapperPlayer* Player);
	UFUNCTION(NetMulticast, Reliable)
	void MulticastSetPlayer(bool Is1P);

private:
	TObjectPtr<class ATrapperPlayer> PlayerRef;

	FTimerHandle ItemDestroyHandle;
	float Acceleration;
	float MaxAcceleration;

	UFUNCTION() void AddBoneAndDestory();
};
