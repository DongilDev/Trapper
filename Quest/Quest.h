// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Data/EventCode.h"
#include "Quest.generated.h"

USTRUCT()
struct FQuest
{
	GENERATED_USTRUCT_BODY()

public:
	FQuest() {}
	void Initialize(int32 InQuestCode, FString InTitle, FString InDescription, 
		int32 InGoalCount1P, int32 InGoalCount2P, FVector InEffectPosition,
		TArray<int32> InQuestActorCode, FString InTutorialText);

	// 퀘스트 코드
	int32 QuestCode;

	// 메인 퀘스트 변경
	uint8 bChangeMainQuest : 1 = false;

	// 제목 & 설명
	FString Title;
	FString Description;
	FString TutorialText;

	// 해야하는 행동의 횟수
	int32 Count1P = 0;
	int32 Count2P = 0;
	int32 GoalCount1P = 0;
	int32 GoalCount2P = 0;
	int32 TotalGoalCount = 0;

	// 퀘스트를 진행중이 아닐때도 체크해야할 때
	uint8 bIsAlwaysChecking : 1 = false;

	// 활성화해야 할 액터 코드
	TArray<int32> QuestActorCode;

	// 퀘스트 이펙트 위치
	FVector EffectPosition = FVector();

	// 퀘스트 예외처리용
	int32 ExceptionCode = 0;

	// 이벤트 발생
	TArray<ETrapperEventCode> SendEventCode;
};