// Fill out your copyright notice in the Description page of Project Settings.


#include "Network/PacketSession.h"
#include "NetworkWorker.h"
#include "ServerPacketHandler.h"
#include "TrapperGameInstance.h"
#include "Engine/World.h"
#include "Engine/GameInstance.h"

PacketSession::PacketSession(FSocket* Socket) : Socket(Socket)
{
	ServerPacketHandler::Init();
}

PacketSession::~PacketSession()
{
	Disconnect();
}

void PacketSession::Run()
{
	RecvWorkerThread = MakeShared<RecvWorker>(Socket, AsShared());
	SendWorkerThread = MakeShared<SendWorker>(Socket, AsShared());
}

void PacketSession::HandleRecvPackets()
{
	while (true)
	{
		TArray<uint8> Packet;
		if (RecvPacketQueue.Dequeue(OUT Packet) == false)
			break;

		PacketSessionRef ThisPtr = AsShared();
		ServerPacketHandler::HandlePacket(ThisPtr, Packet.GetData(), Packet.Num());
	}
}

void PacketSession::SendPacket(SendBufferRef SendBuffer)
{
	SendPacketQueue.Enqueue(SendBuffer);
}

void PacketSession::Disconnect()
{
	if (RecvWorkerThread)
	{
		RecvWorkerThread->Destroy();
		RecvWorkerThread = nullptr;
	}

	if (SendWorkerThread)
	{
		SendWorkerThread->Destroy();
		SendWorkerThread = nullptr;
	}

	if (UWorld* World = GWorld)
	{
		if (UGameInstance* GameInstance = World->GetGameInstance())
		{
			if (UTrapperGameInstance* MyGameInstance = Cast<UTrapperGameInstance>(GameInstance))
			{
				MyGameInstance->DisconnectFromGameServer();
			}
		}
	}
}
