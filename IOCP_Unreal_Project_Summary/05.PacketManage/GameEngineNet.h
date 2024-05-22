#pragma once
#include "GameEngineDispatcher.h"

class GameEngineNet
{
public:
	GameEngineNet();
	~GameEngineNet();
	
	GameEngineNet(const GameEngineNet& _Other) = delete;
	GameEngineNet(GameEngineNet&& _Other) noexcept = delete;
	GameEngineNet& operator=(const GameEngineNet& _Other) = delete;
	GameEngineNet& operator=(GameEngineNet&& _Other) noexcept = delete;

	GameEngineDispatcher Dispatcher;

	//���ν����忡�� ������ ���۵� �� ȣ��
	void UpdatePacket();

protected:
	static void RecvThreadFunction(SOCKET _Socket, GameEngineNet* _Net);
	static bool SearchPacketData(GameEngineSerializer& _Ser, unsigned int& _PacketType, unsigned int& _PacketSize);

private:
	std::mutex RecvPacketLock;
	std::list<std::shared_ptr<GameEnginePacket>> RecvPacket;
	std::list<std::shared_ptr<GameEnginePacket>> ProcessPackets;
};

