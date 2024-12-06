// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyGM.h"
#include "LobbyGS.h"
#include "LobbyPC.h"
#include "Protocol.pb.h"
#include "ServerPacketHandler.h"
#include "TrapperGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Camera/CameraActor.h"

#include "MediaPlayer.h"
#include "MediaSource.h"
#include "MediaTexture.h"

void ALobbyGM::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
}

void ALobbyGM::Logout(AController* Exiting)
{
	Super::Logout(Exiting);

	if (!Exiting->IsLocalController())
	{
		if (bServerPlayerTitleTravle)
		{
			UGameplayStatics::OpenLevel(this, "Title");
			Cast<UTrapperGameInstance>(GetGameInstance())->SetRoomNumber(0);
			Cast<UTrapperGameInstance>(GetGameInstance())->SetRoomIp("");
		}
	}
}

void ALobbyGM::BeginPlay()
{
	Super::BeginPlay();
}

void ALobbyGM::DecreaseTime()
{
	ALobbyGS* GS = GetGameState<ALobbyGS>();
	if (GS)
	{
		GS->LeftTime--;
		GS->OnRep_LeftTime();

		if (GS->LeftTime == 0)
		{
			// 모든 클라이언트에 로딩 패널 띄우기
			for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
			{
				ALobbyPC* PC = Cast<ALobbyPC>(It->Get());
				if (PC)
				{
					PC->ClientShowLoading();  // 클라이언트에서 로딩 패널을 띄우기 위한 RPC 호출
				}
			}

			// 게임 시작 
			GetWorld()->ServerTravel(TEXT("Opening"));

			// 게임 시작했으니 방 지우라고 서버에 전송
			Protocol::C_DESTROY_ROOM Msg;
			Msg.set_roomnum(Cast<UTrapperGameInstance>(GetGameInstance())->GetRoomNumber());
			SendBufferRef SendBuffer = ServerPacketHandler::MakeSendBuffer(Msg);
			Cast<UTrapperGameInstance>(GetGameInstance())->SendPacket(SendBuffer);
			Cast<UTrapperGameInstance>(GetGameInstance())->SetRoomNumber(0);
			Cast<UTrapperGameInstance>(GetGameInstance())->SetRoomIp("");

			// 타이머 클리어
			GetWorldTimerManager().ClearTimer(LobbyTimer);
		}
	}
}

void ALobbyGM::StartGame()
{
	// 5초 카운트 다운
	CountDown();
}

void ALobbyGM::CountDown()
{
	GetWorldTimerManager().SetTimer(
		LobbyTimer,
		this,
		&ALobbyGM::DecreaseTime,
		1.0f,
		true,
		1.0f
	);
}