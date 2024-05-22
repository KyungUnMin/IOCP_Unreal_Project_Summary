#pragma once

class Session;

/*----------------------------------
					EventType
----------------------------------*/

enum class EventType : uint8
{
	Connect,
	Disconnect,
	Accept,
	Recv,
	Send,
};

/*----------------------------------
					IocpEvent
----------------------------------*/

class IocpEvent : public OVERLAPPED
{
public:
	IocpEvent(EventType type);
	~IocpEvent();

	IocpEvent(const IocpEvent& _Other) = delete;
	IocpEvent(IocpEvent&& _Other) noexcept = delete;
	IocpEvent& operator=(const IocpEvent& _Other) = delete;
	IocpEvent& operator=(const IocpEvent&& _Other) noexcept = delete;

	void	Init();

public:
	EventType			eventType;
	IocpObjectRef	owner;
};


/*----------------------------------
					ConnectEvent
---------------------------------*/

class ConnectEvent : public IocpEvent
{
public:
	ConnectEvent() : IocpEvent(EventType::Connect){}
};

/*----------------------------------
					DisconnectEvent
---------------------------------*/

class DisconnectEvent : public IocpEvent
{
public:
	DisconnectEvent() : IocpEvent(EventType::Disconnect) {}
};


/*----------------------------------
					AcceptEvent
---------------------------------*/

class AcceptEvent : public IocpEvent
{
public:
	AcceptEvent() : IocpEvent(EventType::Accept) {}

public:
	SessionRef clientSession = nullptr;
};


/*----------------------------------
					RecvEvent
---------------------------------*/

class RecvEvent : public IocpEvent
{
public:
	RecvEvent() : IocpEvent(EventType::Recv) {}
};


/*----------------------------------
					SendEvent
---------------------------------*/

class SendEvent : public IocpEvent
{
public:
	SendEvent() : IocpEvent(EventType::Send) {}

	Vector<SendBufferRef> sendBuffers;
};
