#pragma once
#include "GameEnginePacket.h"
#include "GameEngineDebug.h"

class GameEngineSerializer;

class GameEngineDispatcher
{
public:
	GameEngineDispatcher();
	~GameEngineDispatcher();

	GameEngineDispatcher(const GameEngineDispatcher& _Other) = delete;
	GameEngineDispatcher(GameEngineDispatcher&& _Other) noexcept = delete;
	GameEngineDispatcher& operator=(const GameEngineDispatcher& _Other) = delete;
	GameEngineDispatcher& operator=(const GameEngineDispatcher&& _Other) noexcept = delete;
	
	
	template<typename PacketType>
	void AddHandler(std::function<void(std::shared_ptr<PacketType>)> _CallBack)
	{
		//패킷 아이디
		int Type = static_cast<int>(PacketType::Type);
		if (true == PacketHandlers.contains(Type))
		{
			MsgAssert("이미 존재하는 핸드러를 또 등록하려고 했습니다.");
			return;
		}


		//패킷으로 변환시키는 콜백 등록
		ConvertPacketHandlers[Type] = [=](GameEngineSerializer& _Ser)
		{
			//템플릿을 이용한 동적할당
			std::shared_ptr<PacketType> NewPacket = std::make_shared<PacketType>();
			NewPacket->DeSerializePacket(_Ser);
			return NewPacket;
		};

		//인자로 받은 함수객체를 저장
		PacketHandlers[Type] = [=](std::shared_ptr<GameEnginePacket> _Packet)
		{
			//템플릿을 이용해 다운캐스팅
			std::shared_ptr<PacketType> ConvertPacket = std::dynamic_pointer_cast<PacketType>(_Packet);
			if (nullptr == ConvertPacket)
			{
				MsgAssert("패킷 타입 변환에 실패했습니다.");
			}
			
			//캡처로 저장했던 함수를 호출
			_CallBack(ConvertPacket);
		};
	}

	



	//바이트 덩어리를 패킷으로 변환
	std::shared_ptr<GameEnginePacket> ConvertPacket(int Type, GameEngineSerializer& _Ser)
	{
		if (false == ConvertPacketHandlers.contains(Type))
		{
			MsgAssert(std::to_string(Type) + "타입의 패킷은 등록하지 않은 패킷입니다");
			return nullptr;
		}

		return ConvertPacketHandlers[Type](_Ser);
	}

	//패킷 처리
	void ProcessPacket(std::shared_ptr<GameEnginePacket> _Packet)
	{
		if (false == PacketHandlers.contains(_Packet->GetPacketID()))
		{
			MsgAssert(std::to_string(_Packet->GetPacketID()) + "타입의 패킷은 등록하지 않은 패킷입니다");
			return;
		}

		//AddHandler에서 등록했던 패킷처리 함수 호출
		PacketHandlers[_Packet->GetPacketID()](_Packet);
	}


private:
	//바이트 덩어리에서 패킷 아이디를 읽어 패킷 객체로 변환 
	// <패킷 아이디, 바이트를 패킷으로 변환시키는 함수객체>
	std::map<int, std::function<std::shared_ptr<GameEnginePacket>(GameEngineSerializer&)>> ConvertPacketHandlers;
	
	//해당 패킷을 어떻게 처리할 지에 대한 정보가 담긴 맵 
	//<패킷아이디, 패킷 처리 함수객체>
	std::map<int, std::function<void(std::shared_ptr<GameEnginePacket>)>> PacketHandlers;
};

