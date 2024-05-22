#include "pch.h"
#include <iostream>

#include "ThreadManager.h"
#include "Service.h"
#include "GameSession.h"
#include "GameSessionManager.h"
#include "BufferWriter.h"
#include "ClientPacketHandler.h"
#include "Protocol.pb.h"
#include "Job.h"
#include "Room.h"
#include "GenProcedures.h"


enum
{
	WORKER_TICK = 64
};

//워커 스레드
void DoWorkerJob(ServerServiceRef& service)
{
	while (true)
	{
		LEndTickCount = ::GetTickCount64() + WORKER_TICK;
		
		//네트워크 입출력 처리
		service->GetIocpCore()->Dispatch(10);
		
		ThreadManager::DistributeReserveJobs();
		ThreadManager::DoGlobalQueueWork();
	}
}

int main()
{
	ClientPacketHandler::Init();

	ServerServiceRef service = MakeShared<ServerService>(
		NetAddress(L"127.0.0.1", 7777),
		MakeShared<IocpCore>(),
		MakeShared<GameSession>, 100);

	ASSERT_CRASH(service->Start());

	const int32 threadCount = (2 * std::thread::hardware_concurrency()) + 1;
	for (int32 i = 0; i < threadCount; i++)
	{
		GThreadManager->Launch([&service]()
		{
			DoWorkerJob(service);
		});
	}

	//MainThread
	DoWorkerJob(service);
	GThreadManager->Join();
	return 0;
}

