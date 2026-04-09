#pragma once

#include <thread>
#include <future>
#include <condition_variable>
#include <mutex>
#include <queue>

#include <functional>
#include <vector>

namespace Atlas {

/**
 * @class ThreadPool
 * @brief A thread pool for asynchronous task execution
 *
 * ThreadPool manages a pool of worker threads that process jobs from a queue.
 * It provides a mechanism to submit callable objects and receive futures for
 * their results. The pool can be started with a specified number of workers
 * and gracefully shut down when no longer needed.
 *
 * @example
 * ThreadPool pool;
 * pool.start(4); // Create 4 worker threads
 * auto future = pool.submit([]() { return 42; });
 * int result = future.get();
 * pool.shutdown();
 */
class ThreadPool {
  public:
	/**
	 * @brief  Initializes the ThreadPool
	 *
	 * @param workerCount - Number of worker threads to create
	 */
	void start(size_t workerCount);

	/**
	 * @brief Shuts down the ThreadPool. Waits for all threads to close.
	 *
	 */
	void shutdown();

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
	auto submit(F&& fn) -> std::future<std::invoke_result_t<F>>;

  private:
	/**
	 * @brief Internal loop each thread operates on. Waits for job and executes
	 * it once available.
	 *
	 */
	void workerLoop();

  private:
	std::vector<std::thread>		  m_threads;
	std::queue<std::function<void()>> m_jobQueue;
	std::mutex						  m_jobQueueMutex;

	std::condition_variable m_conditionVariable;

	bool m_stopping = false;
};

template <typename F>
auto ThreadPool::submit(F&& fn) -> std::future<std::invoke_result_t<F>> {
	using ResultType = std::invoke_result_t<F>;
	auto task		 = std::make_shared<std::packaged_task<ResultType()>>(std::forward<F>(fn));
	auto future		 = task->get_future();

	std::unique_lock<std::mutex> lock(m_jobQueueMutex);
	m_jobQueue.emplace([task]() { (*task)(); });
	lock.unlock();

	m_conditionVariable.notify_one();
	return future;
}

}  // namespace Atlas