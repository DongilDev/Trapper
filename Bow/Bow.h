// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BowEnums.h"
#include "Bow.generated.h"

UCLASS()
class TRAPPERPROJECT_API ABow : public AActor
{
	GENERATED_BODY()
	
public:	
	ABow();
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
	UPROPERTY(EditDefaultsOnly) 
	class USkeletalMeshComponent* BowMesh;
	
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Bow")
	EBowDrawState DrawState = EBowDrawState::E_Idle;
};
