// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Quest/Quest.h"
#include "QuestManager.generated.h"

UCLASS()
class TRAPPERPROJECT_API AQuestManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AQuestManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
	TObjectPtr<class UDataTable> QuestData;
	TObjectPtr<class UDataTable> QuestActorData;
	void AddQuest();

	TArray<FQuest> QuestList;

	UPROPERTY(ReplicatedUsing = OnRep_ChangeCurrentIndex)
	int32 CurrentQuestIndex = -1;

	UPROPERTY(Replicated)
	uint8 bIsQuestWaiting : 1;

	UFUNCTION()
	void OnRep_ChangeCurrentIndex();

	FQuest& GetCurrentQuest();

	void QuestStart();
	void QuestCheck(int32 InQuestCode, bool bIs1P = true);
	void AlwaysCheckQuestCheck(int32 InQuestCode, bool bIs1P);

	bool IsQuestClear();

	FTimerHandle QuestCompleteHandle;
	UFUNCTION() void QuestComplete();

	UFUNCTION(Server, Reliable)
	void ServerRPCQuestComplete();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPCAlwaysQuestNotCheck(int32 Index);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPCQuestEffectOff();

	void MulticastRPCQuestEffectOff_Implementation();

	UFUNCTION(Server, Reliable)
	void ServerRPCAddCount(int32 Index, int32 Count);

	UFUNCTION(Client, Reliable)
	void ClientRPCAddCount(int32 Index, int32 Count);

	void PlayQuestCompleteSound();

	int32 TutorialEndIndex;
	int32 LastQuestIndex;
	int32 MoveMaintenanceQuestIndex = 5;

	TArray<TObjectPtr<class AActor>> QuestActorBox;
	void CreateQuestActor();
	void DestroyQuestActor();

	TObjectPtr<class UDataTable> TutorialMonsterData;
	void TutorialMonsterSetting();
	void SkipTutorial();
	void TutorialEndSetting();
	void SkipBeforeBoss();
	void SetQuest(int Index);

	void HandleQuestExceptions();

	TObjectPtr<class AQuestEffect> QuestEffect;
	void SetQuestEffect();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TSubclassOf<class AQuestPing> QuestPingClass;

	UPROPERTY()
	TObjectPtr<class AQuestPing> QuestPing;

	void SetQuestUI();
};
