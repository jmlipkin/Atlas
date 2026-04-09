#pragma once

#include "Atlas/Core/Thread/ResultQueue.h"
#include "Atlas/Core/Thread/ThreadPool.h"

#include <string>
#include <functional>
#include <stack>

namespace Atlas {

/**
 * @struct DedicatedThread
 * @brief Represents a dedicated thread with associated metadata and lifecycle callbacks.
 *
 * @details This structure encapsulates the essential information needed to manage
 * a dedicated thread, including its identification and shutdown behavior.
 *
 */
struct DedicatedThread {
	/** @brief The name identifier for the dedicated thread.
	 * @details Used for logging, debugging, and thread identification purposes.
	 */
	std::string name;

	/** @var DedicatedThread::onShutdown
	 * @brief Callback function invoked when the thread is shutting down.
	 * @details This function will be called during the thread's shutdown sequence,
	 * allowing for proper cleanup and resource deallocation. The function takes
	 * no parameters and returns void.
	 */
	std::function<void()> onShutdown;
};

class ThreadManager {
  public:
	void init(size_t workerCap = 0);

	void shutdown();

	void registerDedicatedThread(DedicatedThread thread) {
		m_dedicatedThreads.push(std::move(thread));
	}

	/**
	 * @brief Submits a function to the thread pool for asynchronous execution.
	 *
	 * @tparam F The type of the callable object to be executed.
	 *
	 * @param fn A callable object (function, lambda, or functor) to be executed by a thread in the pool.
	 *
	 * @return A std::future object that can be used to retrieve the result of the function execution.
	 *         The future's type is deduced from the function's return type using std::invoke_result_t.
	 *
	 * @note The function is executed asynchronously on an available thread in the pool.
	 *       The caller can use the returned future to wait for completion and retrieve the result.
	 */
	template <typename F>
	auto submit(F&& fn) -> std::future<std::invoke_result_t<F>> { return m_pool.submit(std::forward<F>(fn)); }

	void enqueueResult(std::function<void()> fn) { m_resultQueue.push(std::move(fn)); }
	void drainResultQueue() { m_resultQueue.drain(); }

	static ThreadManager& get() { return s_instance; }

  private:
	ThreadManager();
	ThreadManager(const ThreadManager& other) = delete;
	ThreadManager(ThreadManager&& other)	  = delete;

  private:
	ThreadPool	m_pool;
	ResultQueue m_resultQueue;

	std::stack<DedicatedThread> m_dedicatedThreads;

	static ThreadManager s_instance;
};

}  // namespace Atlas