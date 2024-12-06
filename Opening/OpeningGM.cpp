// Fill out your copyright notice in the Description page of Project Settings.


#include "Opening/OpeningGM.h"
#include "Opening/OpeningGS.h"
#include "Opening/OpeningPC.h"
#include "Opening/OpeningWidgetBase.h"

void AOpeningGM::ServerTravelToTrapper_Implementation()
{
	GetWorld()->ServerTravel(TEXT("Trapper"));
}

void AOpeningGM::BeginPlay()
{
	Super::BeginPlay();

	ServerSkip = false;
	ClientSkip = false;

	CurrentClientNum = 0;
}

void AOpeningGM::PlayOpeningMediaGM()
{
	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		AOpeningPC* PC = Cast<AOpeningPC>(Iterator->Get());
		if (PC)
		{
			// 서버와 클라이언트에서 MediaPlayer를 재생
			if (PC->IsLocalController())
			{
				// 서버의 MediaPlayer 재생
				PC->PlayOpeningMediaPC();
			}
			else
			{
				// 클라이언트에서 MediaPlayer 재생
				PC->ClientPlayMedia();
			}
		}
	}
}

void AOpeningGM::OnPlayerLoaded()
{
	CurrentClientNum++;
	if(CurrentClientNum == ExpectedClientNum)
	{
		PlayOpeningMediaGM();
	}
}

void AOpeningGM::PressedSkip(int32 Type)
{
	if (!this || (ClientSkip && ServerSkip))
	{
		return;
	}
	AOpeningGS* GS = GetGameState<AOpeningGS>();

	if (Type == 1 && ServerSkip == false)
	{
		ServerSkip = true;
		GS->SkipCount++;
		GS->OnRep_SkipCount();
	}
	else if (Type == 2 && ClientSkip == false)
	{
		ClientSkip = true;
		GS->SkipCount++;
		GS->OnRep_SkipCount();
	}
	
	// 둘다 스킵을 눌렀다면
	if (ClientSkip && ServerSkip)
	{
		for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
		{
			AOpeningPC* PC = Cast<AOpeningPC>(It->Get());
			if (PC)
			{
				PC->ClientShowLoading();  // 클라이언트에서 로딩 패널을 띄우기 위한 RPC 호출
			}
		}

		GetWorld()->ServerTravel(TEXT("Trapper"));
	}

}
