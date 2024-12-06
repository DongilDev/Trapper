// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster/Animation/Effect/MyAnimNotify_PlaySoundAtLocation.h"
#include "Kismet/GameplayStatics.h"
#include "Monster/BaseMonster.h"

void UMyAnimNotify_PlaySoundAtLocation::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);

	if (IsValid(MeshComp))
	{
		TObjectPtr<ABaseMonster> SoundMonster = Cast<ABaseMonster>(MeshComp->GetOwner());
		
		if (SoundMonster != nullptr)
		{
			//USoundAttenuation AttenuationSettings;
			//AttenuationSettings.Attenuation.FalloffDistance = 100.f;
			if (MySound)
			{
				UGameplayStatics::PlaySoundAtLocation(MeshComp->GetWorld(), MySound, SoundMonster->GetActorLocation());
			}
		}
		
	}	
}
