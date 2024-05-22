#include "PrecompileHeader.h"
#include "GameEngineNet.h"
#include "GameEngineSerializer.h"


GameEngineNet::GameEngineNet()
{
}

GameEngineNet::~GameEngineNet()
{
}

//패킷 수신 전용 스레드
void GameEngineNet::RecvThreadFunction(SOCKET _Socket, GameEngineNet* _Net)
{
	//버퍼
	char Data[4096] = { 0 };
	GameEngineSerializer Serializer;
	
	unsigned int PacketType = -1;
	unsigned int PacketSize = -1;
	while (true == _Net->IsNet())
	{
		int Result = recv(_Socket, Data, sizeof(Data), 0);
		if (-1 == Result)
			return;

		if (SOCKET_ERROR == _Socket || INVALID_SOCKET == _Socket)
			return;

		Serializer.Write(Data, Result);
		if (false == SearchPacketData(Serializer, PacketType, PacketSize))
			continue;

		if (-1 == PacketSize)
		{
			MsgAssert("패킷 사이즈가 -1이 나왔습니다");
			return;
		}

		if (PacketSize > Serializer.GetWriteOffSet())
			continue;

		while (true)
		{
			std::shared_ptr<GameEnginePacket> Packet = _Net->Dispatcher.ConvertPacket(PacketType, Serializer);
			if (nullptr != Packet)
			{
				_Net->RecvPacketLock.lock();
				_Net->RecvPacket.push_back(Packet);
				_Net->RecvPacketLock.unlock();
			}
			else
			{
				MsgAssert("알 수 없는 패킷 타입 : " + std::to_string(PacketType));
				return;
			}

			PacketType = -1;
			PacketSize = -1;

			if (Serializer.GetWriteOffSet() == Serializer.GetReadOffSet())
			{
				Serializer.Reset();
				break;
			}

			else
			{
				if (false == SearchPacketData(Serializer, PacketType, PacketSize))
					break;

				unsigned int RemainSize = Serializer.GetWriteOffSet() - Serializer.GetReadOffSet();
				if (RemainSize < PacketSize)
					break;
			}
		}

	}
}



bool GameEngineNet::SearchPacketData(GameEngineSerializer& _Ser, unsigned int& _PacketType, unsigned int& _PacketSize)
{
	if (0 != _Ser.GetReadOffSet())
	{
		_Ser.ClearReadData();
	}
	
	unsigned int BuffSize = _Ser.GetWriteOffSet() - _Ser.GetReadOffSet();
	if (BuffSize < 8)
		return false;

	if (-1 == _PacketType)
	{
		unsigned char* TypePivotPtr = &_Ser.GetDataPtr()[0];
		unsigned int* ConvertPtr = reinterpret_cast<unsigned int*>(TypePivotPtr);
		_PacketType = *ConvertPtr;
	}

	if (-1 == _PacketSize)
	{
		unsigned char* SizePivotPtr = &_Ser.GetDataPtr()[4];
		unsigned int* ConvertPtr = reinterpret_cast<unsigned int*>(SizePivotPtr);
		_PacketSize = *ConvertPtr;
	}

	return true;
}

//메인스레드에서 프레임 시작될 때 호출
void GameEngineNet::UpdatePacket()
{
	RecvPacketLock.lock();

	if (0 >= RecvPacket.size())
	{
		RecvPacketLock.unlock();
		return;
	}

	ProcessPackets = RecvPacket;
	RecvPacket.clear();
	RecvPacketLock.unlock();

	for (std::shared_ptr<GameEnginePacket> Packet : ProcessPackets)
	{
		Dispatcher.ProcessPacket(Packet);
	}
	ProcessPackets.clear();
}
