#ifndef UG_RINGBUFFER_HPP
#define UG_RINGBUFFER_HPP

#include "ug_def.h"

#include <cstddef>
#include <array>
#include <atomic>

UG_NS_BEGIN

/**
 * @brief ring buffer, single writer, single reader thread-safe
 */
template <typename T, size_t N>
class ring_buffer {
    static constexpr size_t M = N + 1;
    using internal_container_t = std::array<T, M>;
public:
    ring_buffer() : m_buffer(), m_head(0), m_tail(0) {}
    ~ring_buffer() {}

    bool push(const T& item) {
        size_t head = m_head.load();
        size_t next_head = (head + 1) % M;
        if (next_head == m_tail.load()) {
            return false;
        }

        m_buffer[head] = item;
        m_head.store(next_head);
        return true;
    }

    bool pop(T& item) {
        size_t tail = m_tail.load();
        if (tail == m_head.load()) {
            return false;
        }

        item = m_buffer[tail];
        m_tail.store((tail + 1) % M);
        return true;
    }

    size_t size() const {
        size_t head = m_head.load();
        size_t tail = m_tail.load();
        return (head + 1) % M == tail ? M : (head + M - tail) % M;
    }

private:
    internal_container_t m_buffer;
    std::atomic<size_t> m_head, m_tail;
};

UG_NS_END

#endif // UG_RINGBUFFER