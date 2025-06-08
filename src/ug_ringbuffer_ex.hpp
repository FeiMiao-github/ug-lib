
#ifndef UG_RINGBUFFER_EX_HPP
#define UG_RINGBUFFER_EX_HPP

#include "ug_def.h"
#include "ug_spin_lock.hpp"

#include <cstddef>
#include <array>
#include <mutex>
#include <thread>

UG_NS_BEGIN

template <typename T, size_t N>
class ring_buffer_ex {
    static constexpr size_t M = N + 1;
    using internal_container_t = std::array<T, M>;
public:
    ring_buffer_ex() : m_buffer(), m_head(0), m_tail(0) {}
    ~ring_buffer_ex() {}

    bool push(const T& item, bool block = false) {

        if (block) {
            while (true)
            {
                m_spin_lock.lock();
                size_t next_head = (m_head + 1) % M;
                if (next_head == m_tail) {
                    m_spin_lock.unlock();
                    continue;
                }
                m_buffer[m_head] = item;
                m_head = next_head;
                m_spin_lock.unlock();
                break;
            }
        }
        else {
            std::lock_guard<std::mutex> lock(m_mutex);
            size_t next_head = (m_head + 1) % M;
            if (next_head == m_tail) {
                return false;
            }
            m_buffer[m_head] = item;
            m_head = next_head;
        }

        return true;
    }

    bool pop(T& item, bool block = false) {
        if (block) {
            while (true)
            {
                m_spin_lock.lock();
                if (m_head == m_tail) {
                    m_spin_lock.unlock();
                    continue;
                }
                item = m_buffer[m_tail];
                m_tail = (m_tail + 1) % M;
                m_spin_lock.unlock();
                break;
            }
        }
        else
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            if (m_head == m_tail) {
                return false;
            }

            item = m_buffer[m_tail];
            m_tail = (m_tail + 1) % M;
        }

        return true;
    }

private:
    internal_container_t m_buffer;
    std::mutex m_mutex;
    ug::spin_lock m_spin_lock;
    size_t m_head, m_tail;
};

UG_NS_END

#endif // UG_RINGBUFFER_EX