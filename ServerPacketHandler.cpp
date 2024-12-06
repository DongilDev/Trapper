#include "ServerPacketHandler.h"
#include "TrapperGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "Engine/GameInstance.h"
#include "Title/TitlePC.h"
#include "Main/MainPC.h"

#include "Sockets.h"
#include "SocketSubsystem.h"

PacketHandlerFunc GPacketHandler[UINT16_MAX];

bool Handle_INVALID(PacketSessionRef& session, BYTE* buffer, int32 len)
{
	PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);
	// TODO : Log
	return false;
}

bool Handle_S_CREATE_ACCOUNT(PacketSessionRef& session, Protocol::S_CREATE_ACCOUNT& pkt)
{
	if (pkt.success())
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Create Success")));
		if (UWorld* World = GWorld)
		{
			if (UGameInstance* GameInstance = World->GetGameInstance())
			{
				AMainPC* PC = Cast<AMainPC>(GameInstance->GetFirstLocalPlayerController());
				PC->ShowAfterJoinWidget(true);
			}
		}
	}

	return true;
	
}

bool Handle_S_CHECK_DUPLICATE_ID(PacketSessionRef& session, Protocol::S_CHECK_DUPLICATE_ID& pkt)
{
	if (UWorld* World = GWorld)
	{
		if (UGameInstance* GameInstance = World->GetGameInstance())
		{
			AMainPC* PC = Cast<AMainPC>(GameInstance->GetFirstLocalPlayerController());
			PC->ShowDuplicateIdWidget(pkt.duplicated());
		}
	}

	return true;
}

bool Handle_S_LOGIN(PacketSessionRef& session, Protocol::S_LOGIN& pkt)
{
	if (pkt.success() == true)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Login Success")));

		// 레벨 옮기기
		FString LevelName = TEXT("Title");

		if (UWorld* World = GWorld)
		{
			if (UGameInstance* GameInstance = World->GetGameInstance())
			{
				if (UTrapperGameInstance* MyGameInstance = Cast<UTrapperGameInstance>(GameInstance))
				{
					AMainPC* PC = Cast<AMainPC>(MyGameInstance->GetFirstLocalPlayerController());
					PC->RecvLoginPacket(true);

					MyGameInstance->SetPlayerID(FString(UTF8_TO_TCHAR(pkt.user().playerid().c_str())));
					MyGameInstance->SetPlayerNickname(FString(UTF8_TO_TCHAR(pkt.user().nickname().c_str())));

					// 친구 목록 가져오기
					for (auto f : pkt.friends())
					{
						MyGameInstance->AddFriends(FString(UTF8_TO_TCHAR(f.nickname().c_str())), f.isonline());
					}

					// 친구 요청 가져오기
					for (auto r : pkt.requests())
					{
						MyGameInstance->AddFriendRequests(FString(UTF8_TO_TCHAR(r.playerid().c_str())), FString(UTF8_TO_TCHAR(r.nickname().c_str())));
					}

					MyGameInstance->LoadNewLevel(LevelName);
				}
			}
		}
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Login Fail")));

		if (UWorld* World = GWorld)
		{
			if (UGameInstance* GameInstance = World->GetGameInstance())
			{
				if (UTrapperGameInstance* MyGameInstance = Cast<UTrapperGameInstance>(GameInstance))
				{
					AMainPC* PC = Cast<AMainPC>(MyGameInstance->GetFirstLocalPlayerController());
					PC->RecvLoginPacket(false);
				}
			}
		}
	}
	return true;
}

bool Handle_S_LOGOUT(PacketSessionRef& session, Protocol::S_LOGOUT& pkt)
{
	if (pkt.success())
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Logout Success")));
		// 레벨 옮기기
		FString LevelName = TEXT("Main");

		if (UWorld* World = GWorld)
		{
			if (UGameInstance* GameInstance = World->GetGameInstance())
			{
				if (UTrapperGameInstance* MyGameInstance = Cast<UTrapperGameInstance>(GameInstance))
				{
					MyGameInstance->LoadNewLevel(LevelName);
					MyGameInstance->InitPlayerInfo();
				}
			}
		}
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Logout Failed")));
	}

	return false;
}

bool Handle_S_ACTIVE_FRIEND(PacketSessionRef& session, Protocol::S_ACTIVE_FRIEND& pkt)
{
	// 친구가 로그인 했을 때 서버에서 걔 로그인 했다고 브로드캐스트 하는 곳
	if (UWorld* World = GWorld)
	{
		if (UGameInstance* GameInstance = World->GetGameInstance())
		{
			if (UTrapperGameInstance* MyGameInstance = Cast<UTrapperGameInstance>(GameInstance))
			{
				TMap<FString, bool>& Friends = MyGameInstance->GetFriends();
				FString FriendNickname = UTF8_TO_TCHAR(pkt.nickname().c_str());

				if (Friends.Contains(FriendNickname))
				{
					Friends[FriendNickname] = pkt.isactive() ? true : false;
				}
			}
		}
	}
	
	return false;
}

bool Handle_S_SEND_REQUEST(PacketSessionRef& session, Protocol::S_SEND_REQUEST& pkt)
{
	if (UWorld* World = GWorld)
	{
		if (UGameInstance* GameInstance = World->GetGameInstance())
		{
			if (UTrapperGameInstance* MyGameInstance = Cast<UTrapperGameInstance>(GameInstance))
			{
				TMap<FString, FString>& FriendRequests = MyGameInstance->GetFriendRequests();
				FString FriendId = UTF8_TO_TCHAR(pkt.playerid().c_str());
				FString FriendNickname = UTF8_TO_TCHAR(pkt.friendnickname().c_str());
				MyGameInstance->AddFriendRequests(FriendId, FriendNickname);
			}
		}
	}

	return false;
}

bool Handle_S_ADD_FRIEND(PacketSessionRef& session, Protocol::S_ADD_FRIEND& pkt)
{
	if (UWorld* World = GWorld)
	{
		if (UGameInstance* GameInstance = World->GetGameInstance())
		{
			if (UTrapperGameInstance* MyGameInstance = Cast<UTrapperGameInstance>(GameInstance))
			{
				TMap<FString, FString>& FriendRequests = MyGameInstance->GetFriendRequests();
				FString FriendNickname = UTF8_TO_TCHAR(pkt.friendinfo().nickname().c_str());
				if(pkt.approve())
				{
					MyGameInstance->AddFriends(FriendNickname, pkt.friendinfo().isonline());
				}
				FriendRequests.Remove(pkt.friendinfo().playerid().c_str());
			}
		}
	}

	return false;
}

bool Handle_S_CHECK_FRIEND(PacketSessionRef& session, Protocol::S_CHECK_FRIEND& pkt)
{
	TArray<int32> SuccessArray;
	for (int32 Success : pkt.success())
	{
		SuccessArray.Add(Success);
	}

	TArray<FString> NicknameArray;
	for (auto Nickname : pkt.playernickname())
	{
		NicknameArray.Add(FString(UTF8_TO_TCHAR(Nickname.c_str())));
	}

	if (UWorld* World = GWorld)
	{
		if (UGameInstance* GameInstance = World->GetGameInstance())
		{
			ATitlePC* PC = Cast<ATitlePC>(GameInstance->GetFirstLocalPlayerController());
			PC->ShowAddFriend(SuccessArray, NicknameArray);
		}
	}


	return false;
}

bool Handle_S_GET_FRIEND(PacketSessionRef& session, Protocol::S_GET_FRIEND& pkt)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Get Friend Success")));

	if (UWorld* World = GWorld)
	{
		if (UGameInstance* GameInstance = World->GetGameInstance())
		{
			if (UTrapperGameInstance* MyGameInstance = Cast<UTrapperGameInstance>(GameInstance))
			{
				for (auto f : pkt.friends())
				{
					MyGameInstance->AddFriends(FString(UTF8_TO_TCHAR(f.nickname().c_str())), f.isonline());
				}
			}
		}
	}

	return false;
}

bool Handle_S_GET_REQUESTS(PacketSessionRef& session, Protocol::S_GET_REQUESTS& pkt)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Get Requests Success")));

	if (UWorld* World = GWorld)
	{
		if (UGameInstance* GameInstance = World->GetGameInstance())
		{
			if (UTrapperGameInstance* MyGameInstance = Cast<UTrapperGameInstance>(GameInstance))
			{
				for (auto r : pkt.requests())
				{
					MyGameInstance->AddFriendRequests(FString(UTF8_TO_TCHAR(r.playerid().c_str())), FString(UTF8_TO_TCHAR(r.nickname().c_str())));
				}
			}
		}
	}

	return false;
}

bool Handle_S_CREATE_ROOM(PacketSessionRef& session, Protocol::S_CREATE_ROOM& pkt)
{
	if (pkt.success() == false)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("CreateRoom Failed")));
		return false;

	}
	if (pkt.success())
	{
		if (UWorld* World = GWorld)
		{
			if (UGameInstance* GameInstance = World->GetGameInstance())
			{
				UTrapperGameInstance* MyGameInstance = Cast<UTrapperGameInstance>(GameInstance);

				MyGameInstance->SetRoomIp(MyGameInstance->GetLocalIPAddress());
				MyGameInstance->SetRoomNumber(pkt.roomnum());
				MyGameInstance->LoadNewLevel(TEXT("Lobby"), TEXT("listen"));

				if(!MyGameInstance->GetLoginVer())
				{
					MyGameInstance->SetPlayerNickname("Riana");
				}

				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("CreateRoom Success")));
			}
		}
	}
	

	return true;
}

bool Handle_S_SHOW_ROOM(PacketSessionRef& session, Protocol::S_SHOW_ROOM& pkt)
{
	ATitlePC* PC = nullptr;
	if (UWorld* World = GWorld)
	{
		if (UGameInstance* GameInstance = World->GetGameInstance())
		{
			PC = Cast<ATitlePC>(GameInstance->GetFirstLocalPlayerController());
		}
	}


	if (pkt.roomnums().size() == 0)
	{
		// 해당하는 방번호가 없다고 알리기
		PC->ShowRoomNotExist(true);
	}
	else
	{	
		for (int i = 0; i < pkt.roomnums_size(); ++i) 
		{
			PC->ShowRoomList(pkt.roomnums(i), *FString(pkt.hostnickname(i).c_str()));
		}
		PC->ShowRoomNotExist(false);
	}


	return false;
}

bool Handle_S_JOIN_ROOM(PacketSessionRef& session, Protocol::S_JOIN_ROOM& pkt)
{
	// 방번호가 있을 때
	if (pkt.success())
	{
		if (UWorld* World = GWorld)
		{
			if (UGameInstance* GameInstance = World->GetGameInstance())
			{
				UTrapperGameInstance* MyGameInstance = Cast<UTrapperGameInstance>(GameInstance);
				MyGameInstance->SetRoomIp(*FString(pkt.ip().c_str()));
				MyGameInstance->SetRoomNumber(pkt.roomnum());
				MyGameInstance->LoadNewLevel(*FString(pkt.ip().c_str()));

				if (!MyGameInstance->GetLoginVer())
				{
					MyGameInstance->SetPlayerNickname("Diana");
				}
			}
		}
	}
	// 방번호가 없거나 방이 다 차있을 때
	else
	{
		if (pkt.ip() == "FULL")
		{
			if (UWorld* World = GWorld)
			{
				if (UGameInstance* GameInstance = World->GetGameInstance())
				{
					ATitlePC* PC = Cast<ATitlePC>(GameInstance->GetFirstLocalPlayerController());
					PC->ShowRoomErrorText(pkt.roomnum());
				}
			}
		}
	}

	return true;
}

bool Handle_S_LEAVE_ROOM(PacketSessionRef& session, Protocol::S_LEAVE_ROOM& pkt)
{
	return false;
}

bool Handle_S_DESTROY_ROOM(PacketSessionRef& session, Protocol::S_DESTROY_ROOM& pkt)
{
	return true;
}

bool Handle_S_SEND_INVITATION(PacketSessionRef& session, Protocol::S_SEND_INVITATION& pkt)
{
	if (!pkt.success())
		return false;

	if (UWorld* World = GWorld)
	{
		if (UGameInstance* GameInstance = World->GetGameInstance())
		{
			ATitlePC* PC = Cast<ATitlePC>(GameInstance->GetFirstLocalPlayerController());

			if (PC)
			{
				PC->ShowInvitationList(pkt.roomnum(), *FString(pkt.mynickname().c_str()));
			}
		}
	}

	return false;
}

bool Handle_S_ENTER_GAME(PacketSessionRef& session, Protocol::S_ENTER_GAME& pkt)
{
	// TODO
	return true;
}

bool Handle_S_CHAT(PacketSessionRef& session, Protocol::S_CHAT& pkt)
{
	//std::cout << pkt.msg() << endl;
	return true;
}
