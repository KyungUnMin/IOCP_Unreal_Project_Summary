#include "pch.h"
#include "Session.h"
#include "SocketUtils.h"
#include "Service.h"

/*----------------------------------
					Session
----------------------------------*/

Session::Session()
	:_recvBuffer(BUFFER_SIZE)
{
	_socket = SocketUtils::CreateSocket();
}

Session::~Session()
{
	SocketUtils::Close(_socket);
}

/*-----------------------------------------
				외부에서 호출 (+ 입출력 예약)
-----------------------------------------*/

void Session::Send(SendBufferRef sendBuffer)
{
	if (false == IsConnected())
		return;

	bool regissterSend = false;

	{
		WRITE_LOCK;

		_sendQueue.push(sendBuffer);
		if (false == _sendRegisterd.exchange(true))
		{
			regissterSend = true;
		}
	}

	if (true == regissterSend)
		RegisterSend();
}

bool Session::Connect()
{
	return RegisterConnect();
}

void Session::Disconnect(const WCHAR* cause)
{
	if (false == _connected.exchange(false))
		return;

	RegisterDisconnect();
}



/*-----------------------------------------
				입출력 예약
-----------------------------------------*/

bool Session::RegisterConnect()
{
	if (true == IsConnected())
		return false;

	if (ServiceType::Client != GetService()->GetServiceType())
		return false;

	if (false == SocketUtils::SetReuseAddress(_socket, true))
		return false;

	if (false == SocketUtils::BindAnyAddress(_socket, 0))
		return false;

	_connectEvent.Init();

	//이 세션이 소멸되는 것을 방지하기 위해 레퍼런스 카운트 유지
	_connectEvent.owner = shared_from_this();

	DWORD numOfBytes = 0;

	SOCKADDR_IN sockAddr = GetService()->GetNetAddress().GetSocketAddr();


	//마지막 인자(LPOVERLAPPED)에 _connectEvent 주소를 넣어두었음
	if (TRUE == SocketUtils::ConnectEx(_socket,
		reinterpret_cast<SOCKADDR*>(&sockAddr),
		sizeof(sockAddr), nullptr, 0,
		&numOfBytes, &_connectEvent))
	{
		return true;
	}

	int32 errorCode = ::WSAGetLastError();
	if (WSA_IO_PENDING == errorCode)
		return true;

	_connectEvent.owner = nullptr;
	return false;
}

bool Session::RegisterDisconnect()
{
	_disconnectEvent.Init();

	//이 세션이 소멸되는 것을 방지하기 위해 레퍼런스 카운트 유지
	_disconnectEvent.owner = shared_from_this();

	//LPOVERLAPPED 인자에 _disconnectEvent 주소를 넣어두었음
	if (TRUE == SocketUtils::DisconnectEx(_socket, &_disconnectEvent, TF_REUSE_SOCKET, 0))
		return true;

	int32 errorCode = ::WSAGetLastError();
	if (WSA_IO_PENDING == errorCode)
		return true;

	_disconnectEvent.owner = nullptr;
	return false;
}

void Session::RegisterRecv()
{
	if (false == IsConnected())
		return;

	_recvEvent.Init();

	//이 세션이 소멸되는 것을 방지하기 위해 레퍼런스 카운트 유지
	_recvEvent.owner = shared_from_this();

	WSABUF wsaBuf;
	wsaBuf.buf = reinterpret_cast<char*>(_recvBuffer.WritePos());
	wsaBuf.len = _recvBuffer.FreeSize();

	DWORD numOfBytes = 0;
	DWORD flags = 0;

	//LPOVERLAPPED 인자에 _recvEvent 주소를 넣어두었음
	if (SOCKET_ERROR != ::WSARecv(
		_socket, &wsaBuf, 1,
		OUT & numOfBytes, OUT & flags,
		&_recvEvent, nullptr))
	{
		return;
	}

	int32 errorCode = ::WSAGetLastError();
	if (WSA_IO_PENDING == errorCode)
		return;

	HandleError(errorCode);
	_recvEvent.owner = nullptr;
}

void Session::RegisterSend()
{
	if (false == IsConnected())
		return;

	_sendEvent.Init();

	//이 세션이 소멸되는 것을 방지하기 위해 레퍼런스 카운트 유지
	_sendEvent.owner = shared_from_this();


	{
		WRITE_LOCK;

		int32 writeSize = 0;

		while (false == _sendQueue.empty())
		{
			SendBufferRef sendBuffer = _sendQueue.front();
			_sendQueue.pop();

			writeSize += sendBuffer->WriteSize();
			_sendEvent.sendBuffers.push_back(sendBuffer);
		}
	}

	Vector<WSABUF> wasBufs;
	wasBufs.reserve(_sendEvent.sendBuffers.size());
	for (SendBufferRef sendBuffer : _sendEvent.sendBuffers)
	{
		WSABUF wsaBuf;
		wsaBuf.buf = reinterpret_cast<char*>(sendBuffer->Buffer());
		wsaBuf.len = static_cast<LONG>(sendBuffer->WriteSize());
		wasBufs.push_back(wsaBuf);
	}

	DWORD numOfBytes = 0;

	//LPOVERLAPPED 인자에 _sendEvent주소를 넣어두었음
	if (SOCKET_ERROR != ::WSASend(
		_socket, wasBufs.data(),
		static_cast<DWORD>(wasBufs.size()),
		OUT & numOfBytes, 0, &_sendEvent, nullptr))
	{
		return;
	}

	int32 errorCode = ::WSAGetLastError();
	if (WSA_IO_PENDING == errorCode)
		return;

	HandleError(errorCode);

	_sendEvent.owner = nullptr;
	_sendEvent.sendBuffers.clear();
	_sendRegisterd.store(false);
}


/*-----------------------------------------
				입출력 처리
-----------------------------------------*/

//워커 스레드를 통해 이 함수가 호출
void Session::Dispatch(IocpEvent* iocpEvent, int32 numOfBytes)
{
	switch (iocpEvent->eventType)
	{
	case EventType::Connect:
		ProcessConnect();
		break;
	case EventType::Recv:
		ProcessRecv(numOfBytes);
		break;
	case EventType::Send:
		ProcessSend(numOfBytes);
		break;
	case EventType::Disconnect:
		ProcessDisconnect();
		break;
	default:
		break;
	}
}