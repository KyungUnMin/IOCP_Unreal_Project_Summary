#pragma once
#include <thread>
#include <functional>

/*---------------------------------
				ThreadManager
---------------------------------*/

class ThreadManager
{
public:
	ThreadManager(const ThreadManager& _Other) = delete;
	ThreadManager(ThreadManager&& _Other) noexcept = delete;
	ThreadManager& operator=(const ThreadManager& _Other) = delete;
	ThreadManager& operator=(const ThreadManager&& _Other) noexcept = delete;

	static void DoGlobalQueueWork();
};

