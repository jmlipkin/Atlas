#pragma once

#include <atomic>
#include <array>

namespace Atlas {

template <typename T, size_t capacity>
class SPSCQueue {
  public:
	bool push(const T& item) {
		size_t head = m_writeHead.load(std::memory_order_relaxed);
		size_t next = (head + 1) % capacity;

		if (next == m_readHead.load(std::memory_order_acquire))
			return false;  // full

		m_buffer[head] = item;
		m_writeHead.store(next, std::memory_order_release);
		return true;
	}

	bool pop(T& item) {
		size_t tail = m_readHead.load(std::memory_order_relaxed);

		if (tail == m_writeHead.load(std::memory_order_acquire))
			return false;  // empty

		item = m_buffer[tail];
		m_readHead.store((tail + 1) % capacity, std::memory_order_release);
		return true;
	}

	bool isEmpty() const {
		return m_readHead.load(std::memory_order_acquire) == m_writeHead.load(std::memory_order_acquire);
	}

  private:
	std::array<T, capacity> m_buffer;
	std::atomic<size_t>		m_writeHead{0};
	std::atomic<size_t>		m_readHead{0};
};

}  // namespace Atlas