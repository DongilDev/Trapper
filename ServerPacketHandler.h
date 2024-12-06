#pragma once
#include "Protocol.pb.h"

#if UE_BUILD_DEBUG + UE_BUILD_DEVELOPMENT + UE_BUILD_TEST + UE_BUILD_SHIPPING >= 1
#include "TrapperProject.h"
#endif

using PacketHandlerFunc = std::function<bool(PacketSessionRef&, BYTE*, int32)>;
extern PacketHandlerFunc GPacketHandler[UINT16_MAX];

enum : uint16
{
	PKT_C_CREATE_ACCOUNT = 1000,
	PKT_S_CREATE_ACCOUNT = 1001,
	PKT_C_CHECK_DUPLICATE_ID = 1002,
	PKT_S_CHECK_DUPLICATE_ID = 1003,
	PKT_C_LOGIN = 1004,
	PKT_S_LOGIN = 1005,
	PKT_C_LOGOUT = 1006,
	PKT_S_LOGOUT = 1007,
	PKT_C_AUTO_LOGIN = 1008,
	PKT_S_ACTIVE_FRIEND = 1009,
	PKT_C_SEND_REQUEST = 1010,
	PKT_S_SEND_REQUEST = 1011,
	PKT_C_ADD_FRIEND = 1012,
	PKT_S_ADD_FRIEND = 1013,
	PKT_C_CHECK_FRIEND = 1014,
	PKT_S_CHECK_FRIEND = 1015,
	PKT_C_GET_FRIEND = 1016,
	PKT_S_GET_FRIEND = 1017,
	PKT_C_GET_REQUESTS = 1018,
	PKT_S_GET_REQUESTS = 1019,
	PKT_C_CREATE_ROOM = 1020,
	PKT_S_CREATE_ROOM = 1021,
	PKT_C_SHOW_ROOM = 1022,
	PKT_S_SHOW_ROOM = 1023,
	PKT_C_JOIN_ROOM = 1024,
	PKT_S_JOIN_ROOM = 1025,
	PKT_C_LEAVE_ROOM = 1026,
	PKT_S_LEAVE_ROOM = 1027,
	PKT_C_DESTROY_ROOM = 1028,
	PKT_S_DESTROY_ROOM = 1029,
	PKT_C_SEND_INVITATION = 1030,
	PKT_S_SEND_INVITATION = 1031,
	PKT_C_ENTER_GAME = 1032,
	PKT_S_ENTER_GAME = 1033,
	PKT_C_CHAT = 1034,
	PKT_S_CHAT = 1035,
};

// Custom Handlers
bool Handle_INVALID(PacketSessionRef& session, BYTE* buffer, int32 len);
bool Handle_S_CREATE_ACCOUNT(PacketSessionRef& session, Protocol::S_CREATE_ACCOUNT& pkt);
bool Handle_S_CHECK_DUPLICATE_ID(PacketSessionRef& session, Protocol::S_CHECK_DUPLICATE_ID& pkt);
bool Handle_S_LOGIN(PacketSessionRef& session, Protocol::S_LOGIN& pkt);
bool Handle_S_LOGOUT(PacketSessionRef& session, Protocol::S_LOGOUT& pkt);
bool Handle_S_ACTIVE_FRIEND(PacketSessionRef& session, Protocol::S_ACTIVE_FRIEND& pkt);
bool Handle_S_SEND_REQUEST(PacketSessionRef& session, Protocol::S_SEND_REQUEST& pkt);
bool Handle_S_ADD_FRIEND(PacketSessionRef& session, Protocol::S_ADD_FRIEND& pkt);
bool Handle_S_CHECK_FRIEND(PacketSessionRef& session, Protocol::S_CHECK_FRIEND& pkt);
bool Handle_S_GET_FRIEND(PacketSessionRef& session, Protocol::S_GET_FRIEND& pkt);
bool Handle_S_GET_REQUESTS(PacketSessionRef& session, Protocol::S_GET_REQUESTS& pkt);
bool Handle_S_CREATE_ROOM(PacketSessionRef& session, Protocol::S_CREATE_ROOM& pkt);
bool Handle_S_SHOW_ROOM(PacketSessionRef& session, Protocol::S_SHOW_ROOM& pkt);
bool Handle_S_JOIN_ROOM(PacketSessionRef& session, Protocol::S_JOIN_ROOM& pkt);
bool Handle_S_LEAVE_ROOM(PacketSessionRef& session, Protocol::S_LEAVE_ROOM& pkt);
bool Handle_S_DESTROY_ROOM(PacketSessionRef& session, Protocol::S_DESTROY_ROOM& pkt);
bool Handle_S_SEND_INVITATION(PacketSessionRef& session, Protocol::S_SEND_INVITATION& pkt);
bool Handle_S_ENTER_GAME(PacketSessionRef& session, Protocol::S_ENTER_GAME& pkt);
bool Handle_S_CHAT(PacketSessionRef& session, Protocol::S_CHAT& pkt);

class ServerPacketHandler
{
public:
	static void Init()
	{
		for (int32 i = 0; i < UINT16_MAX; i++)
			GPacketHandler[i] = Handle_INVALID;
		GPacketHandler[PKT_S_CREATE_ACCOUNT] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::S_CREATE_ACCOUNT>(Handle_S_CREATE_ACCOUNT, session, buffer, len); };
		GPacketHandler[PKT_S_CHECK_DUPLICATE_ID] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::S_CHECK_DUPLICATE_ID>(Handle_S_CHECK_DUPLICATE_ID, session, buffer, len); };
		GPacketHandler[PKT_S_LOGIN] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::S_LOGIN>(Handle_S_LOGIN, session, buffer, len); };
		GPacketHandler[PKT_S_LOGOUT] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::S_LOGOUT>(Handle_S_LOGOUT, session, buffer, len); };
		GPacketHandler[PKT_S_ACTIVE_FRIEND] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::S_ACTIVE_FRIEND>(Handle_S_ACTIVE_FRIEND, session, buffer, len); };
		GPacketHandler[PKT_S_SEND_REQUEST] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::S_SEND_REQUEST>(Handle_S_SEND_REQUEST, session, buffer, len); };
		GPacketHandler[PKT_S_ADD_FRIEND] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::S_ADD_FRIEND>(Handle_S_ADD_FRIEND, session, buffer, len); };
		GPacketHandler[PKT_S_CHECK_FRIEND] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::S_CHECK_FRIEND>(Handle_S_CHECK_FRIEND, session, buffer, len); };
		GPacketHandler[PKT_S_GET_FRIEND] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::S_GET_FRIEND>(Handle_S_GET_FRIEND, session, buffer, len); };
		GPacketHandler[PKT_S_GET_REQUESTS] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::S_GET_REQUESTS>(Handle_S_GET_REQUESTS, session, buffer, len); };
		GPacketHandler[PKT_S_CREATE_ROOM] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::S_CREATE_ROOM>(Handle_S_CREATE_ROOM, session, buffer, len); };
		GPacketHandler[PKT_S_SHOW_ROOM] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::S_SHOW_ROOM>(Handle_S_SHOW_ROOM, session, buffer, len); };
		GPacketHandler[PKT_S_JOIN_ROOM] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::S_JOIN_ROOM>(Handle_S_JOIN_ROOM, session, buffer, len); };
		GPacketHandler[PKT_S_LEAVE_ROOM] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::S_LEAVE_ROOM>(Handle_S_LEAVE_ROOM, session, buffer, len); };
		GPacketHandler[PKT_S_DESTROY_ROOM] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::S_DESTROY_ROOM>(Handle_S_DESTROY_ROOM, session, buffer, len); };
		GPacketHandler[PKT_S_SEND_INVITATION] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::S_SEND_INVITATION>(Handle_S_SEND_INVITATION, session, buffer, len); };
		GPacketHandler[PKT_S_ENTER_GAME] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::S_ENTER_GAME>(Handle_S_ENTER_GAME, session, buffer, len); };
		GPacketHandler[PKT_S_CHAT] = [](PacketSessionRef& session, BYTE* buffer, int32 len) { return HandlePacket<Protocol::S_CHAT>(Handle_S_CHAT, session, buffer, len); };
	}

	static bool HandlePacket(PacketSessionRef& session, BYTE* buffer, int32 len)
	{
		PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);
		return GPacketHandler[header->id](session, buffer, len);
	}
	static SendBufferRef MakeSendBuffer(Protocol::C_CREATE_ACCOUNT& pkt) { return MakeSendBuffer(pkt, PKT_C_CREATE_ACCOUNT); }
	static SendBufferRef MakeSendBuffer(Protocol::C_CHECK_DUPLICATE_ID& pkt) { return MakeSendBuffer(pkt, PKT_C_CHECK_DUPLICATE_ID); }
	static SendBufferRef MakeSendBuffer(Protocol::C_LOGIN& pkt) { return MakeSendBuffer(pkt, PKT_C_LOGIN); }
	static SendBufferRef MakeSendBuffer(Protocol::C_LOGOUT& pkt) { return MakeSendBuffer(pkt, PKT_C_LOGOUT); }
	static SendBufferRef MakeSendBuffer(Protocol::C_AUTO_LOGIN& pkt) { return MakeSendBuffer(pkt, PKT_C_AUTO_LOGIN); }
	static SendBufferRef MakeSendBuffer(Protocol::C_SEND_REQUEST& pkt) { return MakeSendBuffer(pkt, PKT_C_SEND_REQUEST); }
	static SendBufferRef MakeSendBuffer(Protocol::C_ADD_FRIEND& pkt) { return MakeSendBuffer(pkt, PKT_C_ADD_FRIEND); }
	static SendBufferRef MakeSendBuffer(Protocol::C_CHECK_FRIEND& pkt) { return MakeSendBuffer(pkt, PKT_C_CHECK_FRIEND); }
	static SendBufferRef MakeSendBuffer(Protocol::C_GET_FRIEND& pkt) { return MakeSendBuffer(pkt, PKT_C_GET_FRIEND); }
	static SendBufferRef MakeSendBuffer(Protocol::C_GET_REQUESTS& pkt) { return MakeSendBuffer(pkt, PKT_C_GET_REQUESTS); }
	static SendBufferRef MakeSendBuffer(Protocol::C_CREATE_ROOM& pkt) { return MakeSendBuffer(pkt, PKT_C_CREATE_ROOM); }
	static SendBufferRef MakeSendBuffer(Protocol::C_SHOW_ROOM& pkt) { return MakeSendBuffer(pkt, PKT_C_SHOW_ROOM); }
	static SendBufferRef MakeSendBuffer(Protocol::C_JOIN_ROOM& pkt) { return MakeSendBuffer(pkt, PKT_C_JOIN_ROOM); }
	static SendBufferRef MakeSendBuffer(Protocol::C_LEAVE_ROOM& pkt) { return MakeSendBuffer(pkt, PKT_C_LEAVE_ROOM); }
	static SendBufferRef MakeSendBuffer(Protocol::C_DESTROY_ROOM& pkt) { return MakeSendBuffer(pkt, PKT_C_DESTROY_ROOM); }
	static SendBufferRef MakeSendBuffer(Protocol::C_SEND_INVITATION& pkt) { return MakeSendBuffer(pkt, PKT_C_SEND_INVITATION); }
	static SendBufferRef MakeSendBuffer(Protocol::C_ENTER_GAME& pkt) { return MakeSendBuffer(pkt, PKT_C_ENTER_GAME); }
	static SendBufferRef MakeSendBuffer(Protocol::C_CHAT& pkt) { return MakeSendBuffer(pkt, PKT_C_CHAT); }

private:
	template<typename PacketType, typename ProcessFunc>
	static bool HandlePacket(ProcessFunc func, PacketSessionRef& session, BYTE* buffer, int32 len)
	{
		PacketType pkt;
		if (pkt.ParseFromArray(buffer + sizeof(PacketHeader), len - sizeof(PacketHeader)) == false)
			return false;

		return func(session, pkt);
	}

	template<typename T>
	static SendBufferRef MakeSendBuffer(T& pkt, uint16 pktId)
	{
		const uint16 dataSize = static_cast<uint16>(pkt.ByteSizeLong());
		const uint16 packetSize = dataSize + sizeof(PacketHeader);

#if UE_BUILD_DEBUG + UE_BUILD_DEVELOPMENT + UE_BUILD_TEST + UE_BUILD_SHIPPING >= 1
		SendBufferRef sendBuffer = MakeShared<SendBuffer>(packetSize);
#else
		SendBufferRef sendBuffer = GSendBufferManager->Open(packetSize);
#endif
		PacketHeader* header = reinterpret_cast<PacketHeader*>(sendBuffer->Buffer());
		header->size = packetSize;
		header->id = pktId;
		pkt.SerializeToArray(&header[1], dataSize);
		sendBuffer->Close(packetSize);

		return sendBuffer;
	}
};