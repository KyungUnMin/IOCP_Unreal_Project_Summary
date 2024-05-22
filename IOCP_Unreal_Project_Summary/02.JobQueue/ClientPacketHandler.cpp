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

	//Room�� JobQueue�� ��ӹ��� �ڽ�
	// 
	//�� �Լ��� ��Ƽ �����忡 �������� �ʱ� ������
	//�ϰ��� �ٷ� ó������ �ʰ� ����
	room->DoASync([player, pkt]()
	{
		player->HandleMove(pkt);
	});
	return true;
}
