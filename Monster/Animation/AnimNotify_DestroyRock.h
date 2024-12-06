// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotify_DestroyRock.generated.h"

/**
 * 
 */
UCLASS()
class TRAPPERPROJECT_API UAnimNotify_DestroyRock : public UAnimNotify
{
	GENERATED_BODY()

protected:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

private:
	TSet<TObjectPtr<class ATrapBase>> DestroyTrapList;
	TSet<TObjectPtr<class AMagneticPillar>> DestroyPillarList;

	TObjectPtr<USkeletalMeshComponent> AnimMeshComp;

	void FindArch();

	///Arch
	TObjectPtr<class AArch> Arch0;
	TObjectPtr<class AArch> Arch1;
};
