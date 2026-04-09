#include "atpch.h"
#include "ThreadManager.h"

#include <thread>

namespace Atlas {

void ThreadManager::init(size_t workerCap) {
	size_t hardwareCap = std::thread::hardware_concurrency() - 1;
	hardwareCap		   = (hardwareCap) ? hardwareCap : 1;  // If single-core device, set to 1

	size_t workerCount = (workerCap) ? std::min(workerCap, hardwareCap) : hardwareCap;

	m_pool.start(workerCount);
}

void ThreadManager::shutdown() {
	m_pool.shutdown();

	while (!m_dedicatedThreads.empty()) {
		DedicatedThread& thread = m_dedicatedThreads.top();
		thread.onShutdown();
		m_dedicatedThreads.pop();
	}
}

}  // namespace Atlas