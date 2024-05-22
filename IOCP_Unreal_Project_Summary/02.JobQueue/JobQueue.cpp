#include "pch.h"
#include "JobQueue.h"
#include "GlobalQueue.h"

/*---------------------------------------------------
				JobQueue
---------------------------------------------------*/

JobQueue::JobQueue()
{

}

JobQueue::~JobQueue()
{

}

void JobQueue::Push(JobRef job, bool pushOnly /*= false*/)
{
	const int32 prevCount = _jobCount.fetch_add(1);
	_jobs.Push(job);

	if (0 != prevCount)
		return;

	if (nullptr == LCurrentJobQueue && false == pushOnly)
		Execute();

	else
		GGlobalQueue->Push(shared_from_this());
}

void JobQueue::Execute()
{
	LCurrentJobQueue = this;

	while (true)
	{
		Vector<JobRef> jobs;
		_jobs.PopAll(jobs);

		const int32 jobCount = static_cast<int32>(jobs.size());
		for (int32 i = 0; i < jobCount; i++)
			jobs[i]->Execute();

		if (_jobCount.fetch_sub(jobCount) == jobCount)
		{
			LCurrentJobQueue = nullptr;
			return;
		}

		const uint64 now = ::GetTickCount64();
		if (LEndTickCount < now)
			continue;

		LCurrentJobQueue = nullptr;

		GGlobalQueue->Push(shared_from_this());
		break;
	}
}
