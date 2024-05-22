#pragma once
#include "Job.h"
#include "JobTimer.h"

/*---------------------------------------------------
				JobQueue
---------------------------------------------------*/

using CallbackType = std::function<void()>;

class JobQueue : public std::enable_shared_from_this<JobQueue>
{
public:
	JobQueue();
	~JobQueue();

	JobQueue(const JobQueue& _Other) = delete;
	JobQueue(JobQueue&& _Other) noexcept = delete;
	JobQueue& operator=(const JobQueue& _Other) = delete;
	JobQueue& operator=(const JobQueue&& _Other) noexcept = delete;

	void DoASync(CallbackType&& callback)
	{
		Push(ObjectPool<Job>::MakeShared(std::move(callback)));
	}

	void Execute();

	void Push(JobRef job, bool pushOnly = false);

protected:
	LockQueue<JobRef>	_jobs;
	Atomic<int32>				_jobCount = 0;
};

