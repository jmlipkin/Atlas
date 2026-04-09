#include "atpch.h"
#include "ThreadPool.h"

#include <mutex>
#include <thread>

namespace Atlas {

void ThreadPool::start(size_t workerCount) {
	for (size_t i = 0; i < workerCount; i++) {
		m_threads.emplace_back(&ThreadPool::workerLoop, this);
	}
}

void ThreadPool::shutdown() {
	// Separate scope to release the lock
	{
		std::unique_lock<std::mutex> lock(m_jobQueueMutex);
		m_stopping = true;
	}

	m_conditionVariable.notify_all();

	for (auto& thread : m_threads) {
		thread.join();
	}
}

void ThreadPool::workerLoop() {
	while (true) {
		std::unique_lock<std::mutex> lock(m_jobQueueMutex);
		m_conditionVariable.wait(lock, [this] { return !m_jobQueue.empty() || m_stopping; });

		if (m_stopping && m_jobQueue.empty())
			return;

		std::function<void()>& job = m_jobQueue.front();
		m_jobQueue.pop();

		lock.unlock();

		job();
	}
}

}  // namespace Atlas