// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify_PlaySound.h"
#include "MyAnimNotify_PlaySoundAtLocation.generated.h"

/**
 * 
 */
UCLASS()
class TRAPPERPROJECT_API UMyAnimNotify_PlaySoundAtLocation : public UAnimNotify_PlaySound
{
	GENERATED_BODY()
	
public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;

public:
	//Sound
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Sound)
	TObjectPtr<USoundBase> MySound;
	

};
