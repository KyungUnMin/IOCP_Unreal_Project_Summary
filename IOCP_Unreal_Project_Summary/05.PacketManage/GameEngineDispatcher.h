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
		//��Ŷ ���̵�
		int Type = static_cast<int>(PacketType::Type);
		if (true == PacketHandlers.contains(Type))
		{
			MsgAssert("�̹� �����ϴ� �ڵ巯�� �� ����Ϸ��� �߽��ϴ�.");
			return;
		}


		//��Ŷ���� ��ȯ��Ű�� �ݹ� ���
		ConvertPacketHandlers[Type] = [=](GameEngineSerializer& _Ser)
		{
			//���ø��� �̿��� �����Ҵ�
			std::shared_ptr<PacketType> NewPacket = std::make_shared<PacketType>();
			NewPacket->DeSerializePacket(_Ser);
			return NewPacket;
		};

		//���ڷ� ���� �Լ���ü�� ����
		PacketHandlers[Type] = [=](std::shared_ptr<GameEnginePacket> _Packet)
		{
			//���ø��� �̿��� �ٿ�ĳ����
			std::shared_ptr<PacketType> ConvertPacket = std::dynamic_pointer_cast<PacketType>(_Packet);
			if (nullptr == ConvertPacket)
			{
				MsgAssert("��Ŷ Ÿ�� ��ȯ�� �����߽��ϴ�.");
			}
			
			//ĸó�� �����ߴ� �Լ��� ȣ��
			_CallBack(ConvertPacket);
		};
	}

	



	//����Ʈ ����� ��Ŷ���� ��ȯ
	std::shared_ptr<GameEnginePacket> ConvertPacket(int Type, GameEngineSerializer& _Ser)
	{
		if (false == ConvertPacketHandlers.contains(Type))
		{
			MsgAssert(std::to_string(Type) + "Ÿ���� ��Ŷ�� ������� ���� ��Ŷ�Դϴ�");
			return nullptr;
		}

		return ConvertPacketHandlers[Type](_Ser);
	}

	//��Ŷ ó��
	void ProcessPacket(std::shared_ptr<GameEnginePacket> _Packet)
	{
		if (false == PacketHandlers.contains(_Packet->GetPacketID()))
		{
			MsgAssert(std::to_string(_Packet->GetPacketID()) + "Ÿ���� ��Ŷ�� ������� ���� ��Ŷ�Դϴ�");
			return;
		}

		//AddHandler���� ����ߴ� ��Ŷó�� �Լ� ȣ��
		PacketHandlers[_Packet->GetPacketID()](_Packet);
	}


private:
	//����Ʈ ������� ��Ŷ ���̵� �о� ��Ŷ ��ü�� ��ȯ 
	// <��Ŷ ���̵�, ����Ʈ�� ��Ŷ���� ��ȯ��Ű�� �Լ���ü>
	std::map<int, std::function<std::shared_ptr<GameEnginePacket>(GameEngineSerializer&)>> ConvertPacketHandlers;
	
	//�ش� ��Ŷ�� ��� ó���� ���� ���� ������ ��� �� 
	//<��Ŷ���̵�, ��Ŷ ó�� �Լ���ü>
	std::map<int, std::function<void(std::shared_ptr<GameEnginePacket>)>> PacketHandlers;
};

