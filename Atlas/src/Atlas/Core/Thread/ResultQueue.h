#pragma once

#include <functional>
#include <vector>
#include <mutex>

namespace Atlas {

/**
 * @class ResultQueue
 * @brief A thread-safe queue for storing and executing callback functions.
 *
 * ResultQueue implements a double-buffered queue pattern that allows multiple threads
 * to safely push work items while a single thread drains and executes them.
 * This design minimizes lock contention by separating write operations from execution.
 */
class ResultQueue {
  public:
	/**
	 * @brief Pushes a function callback onto the queue.
	 *
	 * Thread-safe method that adds a function to the write buffer. The function will
	 * be executed when drain() is called.
	 *
	 * @param fn A std::function<void()> callback to be queued for execution.
	 *
	 * @note This method acquires a lock during insertion.
	 */
	void push(std::function<void()> fn) {
		std::unique_lock<std::mutex> lock(m_resultQueueMutex);
		m_writeBuffer.push_back(std::move(fn));
	}

	/**
	 * @brief Drains and executes all queued functions.
	 *
	 * Swaps the write buffer with the swap buffer under lock, then executes all queued
	 * functions outside the critical section. This minimizes lock hold time.
	 *
	 * @note Functions are executed sequentially in the order they were pushed.
	 */
	void drain() {
		std::unique_lock<std::mutex> lock(m_resultQueueMutex);
		m_writeBuffer.swap(m_swapBuffer);
		lock.unlock();

		for (auto& job : m_swapBuffer) {
			job();
		}

		m_swapBuffer.clear();
	}

  private:
	std::vector<std::function<void()>> m_writeBuffer;
	std::vector<std::function<void()>> m_swapBuffer;

	std::mutex m_resultQueueMutex;
};

}  // namespace Atlas