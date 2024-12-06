// Fill out your copyright notice in the Description page of Project Settings.


#include "Opening/OpeningGS.h"
#include "Opening/OpeningPC.h"
#include "Opening/OpeningWidgetBase.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

void AOpeningGS::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AOpeningGS, SkipCount);
}

void AOpeningGS::OnRep_SkipCount()
{
	AOpeningPC* PC = Cast<AOpeningPC>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	if (PC && PC->OpeningWidgetObject)
	{
		PC->OpeningWidgetObject->SetSkipCount(SkipCount);
	}

	if (SkipCount == 2)
	{
		PC->OpeningWidgetObject->ShowLoading();
	}
}