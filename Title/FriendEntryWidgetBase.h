// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FriendEntryWidgetBase.generated.h"

/**
 * 
 */
UCLASS()
class TRAPPERPROJECT_API UFriendEntryWidgetBase : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;
};
