#include "pch.h"
#include "IocpCore.h"
#include "IocpEvent.h"


IocpCore::IocpCore()
{
	_iocpHandle = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, 0);
	ASSERT_CRASH(INVALID_HANDLE_VALUE != _iocpHandle);
}

IocpCore::~IocpCore()
{
	::CloseHandle(_iocpHandle);
}

bool IocpCore::Register(IocpObjectRef iocpObject)
{
	HANDLE result = ::CreateIoCompletionPort(
		iocpObject->GetHandle(),
		_iocpHandle, 
		0, 0);

	return result;
}

bool IocpCore::Dispatch(uint32 timeoutMs /*= INFINITE*/)
{
	DWORD numOfBytes = 0;
	ULONG_PTR key = 0;	//��� ����
	IocpEvent* iocpEvent = nullptr;

	if (TRUE == ::GetQueuedCompletionStatus(
		_iocpHandle,
		OUT & numOfBytes,
		OUT &key,
		OUT reinterpret_cast<LPOVERLAPPED*>(&iocpEvent),
		timeoutMs))
	{
		//������� �Ϸ�Ǵ� ���� owner(����)�� ���۷��� ī��Ʈ ����
		IocpObjectRef iocpObject = iocpEvent->owner;
		iocpObject->Dispatch(iocpEvent, numOfBytes);
	}
	else
	{
		int32 errCore = ::WSAGetLastError();
		switch (errCore)
		{
		case WAIT_TIMEOUT:
			return false;
		default:
		{
			IocpObjectRef iocpObject = iocpEvent->owner;
			iocpObject->Dispatch(iocpEvent, numOfBytes);
			break;
		}
		}
	}
	
	return true;
}