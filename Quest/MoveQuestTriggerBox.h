// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Quest/Interact.h"
#include "MoveQuestTriggerBox.generated.h"

UCLASS()
class TRAPPERPROJECT_API AMoveQuestTriggerBox : public AInteract
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMoveQuestTriggerBox();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<class UBoxComponent> BoxComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	uint8 bCheckMagneticMoving : 1;
};
