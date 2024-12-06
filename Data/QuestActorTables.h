// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "QuestActorTables.generated.h"

USTRUCT()
struct FQuestActorInfo : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

public:
	FQuestActorInfo()
	: QuestActor()
	, Position()
	, Rotation()
	, Scale()
	, Memo()
	{}

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<class AActor> QuestActor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector Position;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRotator Rotation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector Scale;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Memo;
};

UCLASS()
class TRAPPERPROJECT_API AQuestActorTables : public AActor
{
	GENERATED_BODY()
};
