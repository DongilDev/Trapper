// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Data/EventCode.h"
#include "QuestDataTables.generated.h"

USTRUCT()
struct FQuestInfo : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

public:
	FQuestInfo()
	: QuestCode()
	, bTutorialEnd()
	, bChangeMainQuest()
	, Title()
	, Description()
	, TutorialText()
	, GoalCount1P()
	, GoalCount2P()
	, bIsAlwaysChecking()
	, QuestActorCode()
	, EffectPosition()
	, Memo()
	{}

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite) int32 QuestCode;

	UPROPERTY(EditAnywhere, BlueprintReadWrite) uint8 bTutorialEnd : 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) uint8 bChangeMainQuest : 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite) FString Title;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FString Description;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FString TutorialText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite) int32 GoalCount1P;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int32 GoalCount2P;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) uint8 bIsAlwaysChecking : 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite) TArray<int32> QuestActorCode;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FVector EffectPosition;

	UPROPERTY(EditAnywhere, BlueprintReadWrite) FString Memo;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int32 ExceptionCode;

	UPROPERTY(EditAnywhere, BlueprintReadWrite) TArray<ETrapperEventCode> SendEventCode;
};

UCLASS()
class TRAPPERPROJECT_API AQuestDataTables : public AActor
{
	GENERATED_BODY()
};
