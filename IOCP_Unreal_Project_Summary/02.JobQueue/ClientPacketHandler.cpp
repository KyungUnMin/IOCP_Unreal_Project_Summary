#include "pch.h"
#include "ClientPacketHandler.h"
#include "GameSession.h"
#include "Player.h"
#include "Room.h"
#include "ObjectUtils.h"
#include "DbManager.h"
#include "GenProcedures.h"

PacketHandlerFunc GPacketHandler[UINT16_MAX];

#define CHECK_PLAYER_AND_ROOM(session)								\
								PlayerRef player = session->GetPlayer();		\
								if (nullptr == player)										\
									return false;												\
								RoomRef room = player->GetRoom();			\
								if (nullptr == room)											\
									return false;


bool Handle_C_MOVE(PacketSessionRef& session, Protocol::C_MOVE& pkt)
{
	GameSessionRef gameSession = std::static_pointer_cast<GameSession>(session);
	CHECK_PLAYER_AND_ROOM(gameSession);

	//Room은 JobQueue를 상속받은 자식
	// 
	//이 함수는 멀티 스레드에 안전하지 않기 때문에
	//일감을 바로 처리하지 않고 예약
	room->DoASync([player, pkt]()
	{
		player->HandleMove(pkt);
	});
	return true;
}
