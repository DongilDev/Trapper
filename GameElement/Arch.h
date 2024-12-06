// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Arch.generated.h"

UCLASS()
class TRAPPERPROJECT_API AArch : public AActor
{
	GENERATED_BODY()
	
public:	
	AArch();

public:	
	UFUNCTION(BlueprintImplementableEvent)
	void SpawnFragment();

	void CallFragment();

	void DestoryFragment() { this->Destroy(); };

public:
	TArray<TWeakObjectPtr<class ATrapBase>> TrapArray;

	UPROPERTY(EditAnywhere, Category = "Trigger")
	TSoftObjectPtr<class ATrapSnapTrigger> TrapSnapTrigger;

	UPROPERTY(EditAnywhere, Category = "Trigger")
	TSoftObjectPtr<class AActor> CellingTrigger;

public:
	///-----------------------------------------------------
	///					[	READ ME	  ]
	/// 
	///		파괴시키는 함수에서 SpawnFragment 함수를 불러주세요
	///
	/// -----------------------------------------------------
};
