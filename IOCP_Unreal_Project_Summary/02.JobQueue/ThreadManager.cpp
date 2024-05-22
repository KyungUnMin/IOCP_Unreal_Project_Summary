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
		//���� �ð��� ������ Execute ����������
		//LEndTickCount�� DoWorkerJob �Լ����� �ð� �������־���
		uint64 now = ::GetTickCount64();
		if (LEndTickCount < now)
			break;

		JobQueueRef jobQueue = GGlobalQueue->Pop();
		if (nullptr == jobQueue)
			break;

		jobQueue->Execute();
	}
}



