#include "pch.h"
#include "ThreadManager.h"
#include "CoreTLS.h"
#include "CoreGlobal.h"
#include "GlobalQueue.h"
#include "JobQueue.h"
#include "JobTimer.h"


void ThreadManager::DoGlobalQueueWork()
{
	while (true)
	{
		//일정 시간이 지나면 Execute 빠져나오기
		//LEndTickCount는 DoWorkerJob 함수에서 시간 설정해주었음
		uint64 now = ::GetTickCount64();
		if (LEndTickCount < now)
			break;

		JobQueueRef jobQueue = GGlobalQueue->Pop();
		if (nullptr == jobQueue)
			break;

		jobQueue->Execute();
	}
}



