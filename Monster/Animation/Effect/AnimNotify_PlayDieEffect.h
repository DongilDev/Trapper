// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AnimNotify_PlayNiagaraEffect.h"
#include "AnimNotify_PlayDieEffect.generated.h"

/**
 * 
 */
UCLASS()
class TRAPPERPROJECT_API UAnimNotify_PlayDieEffect : public UAnimNotify_PlayNiagaraEffect
{
	GENERATED_BODY()
	
public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
};
