#pragma once
#include "IocpCore.h"
#include "IocpEvent.h"
#include "NetAddress.h"
#include "RecvBuffer.h"

/*----------------------------------
					Session
----------------------------------*/

class Service;

class Session : public IocpObject
{
	friend class Listener;
	friend class IocpCore;
	friend class Service;

	enum
	{
		BUFFER_SIZE = 0x10000 //64kb
	};

public:
	Session();
	virtual ~Session();

	Session(const Session& _Other) = delete;
	Session(Session&& _Other) noexcept = delete;
	Session& operator=(const Session& _Other) = delete;
	Session& operator=(const Session&& _Other) noexcept = delete;

private:
	//��Ŀ �����带 ���� �� �Լ��� ȣ��
	virtual void Dispatch(class IocpEvent* iocpEvent, int32 numOfBytes = 0) override;
	
public:
	void		Send(SendBufferRef sendBuffer);
	bool		Connect();
	void		Disconnect(const WCHAR* cause);

private:
	//����� ����
	bool		RegisterConnect();
	bool		RegisterDisconnect();
	void		RegisterRecv();
	void		RegisterSend();

	//����� ó��
	void ProcessConnect();
	void ProcessDisconnect();
	void ProcessRecv(int32 numOfBytes);
	void ProcessSend(int32 numOfBytes);/*SendEvent ���� �ӽ�*/

private:
	/*-----------------------------
						IocpEvent
	-----------------------------*/
	ConnectEvent			_connectEvent;
	DisconnectEvent		_disconnectEvent;
	RecvEvent				_recvEvent;
	SendEvent				_sendEvent;
};
