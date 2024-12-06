// Fill out your copyright notice in the Description page of Project Settings.


#include "Quest/Quest.h"

void FQuest::Initialize(
	int32 InQuestCode, FString InTitle, FString InDescription,
	int32 InGoalCount1P, int32 InGoalCount2P, FVector InEffectPosition,
	TArray<int32> InQuestActorCode, FString InTutorialText)
{
	this->QuestCode = InQuestCode;

	this->Title = InTitle;
	this->Description = InDescription;
	this->TutorialText = InTutorialText;

	this->GoalCount1P = InGoalCount1P;
	this->GoalCount2P = InGoalCount2P;
	this->EffectPosition = InEffectPosition;

	this->QuestActorCode = InQuestActorCode;
	this->TotalGoalCount = InGoalCount1P + InGoalCount2P;
}
