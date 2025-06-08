#ifndef UG_THREAD_SAFE_QUEUE_HPP
#define UG_THREAD_SAFE_QUEUE_HPP

#include "ug_def.h"
#include <deque>
#include <mutex>
#include <condition_variable>

UG_NS_BEGIN

template <typename T>
class thread_safe_queue {
public:
    thread_safe_queue(size_t size) : m_size(size) {}
    ~thread_safe_queue() {}

    bool push(const T& item, bool block = false) {
        std::unique_lock<std::mutex> lock(m_mutex);
        if (block)
        {
            m_cond_push.wait(lock, [this]() { return m_queue.size() < m_size; });
            m_queue.push_back(item);
            m_cond_pop.notify_one();
        }
        else
        {
            if (m_queue.size() >= m_size) {
                return false;
            }
            m_queue.push_back(item);
            m_cond_pop.notify_one();
        }
        return true;
    }

    bool pop(T& item, bool block = false) {
        std::unique_lock<std::mutex> lg(m_mutex);
        if (block)
        {
            m_cond_pop.wait(lg, [this]() { return m_queue.size() > 0; });
            item = m_queue.front();
            m_queue.pop_front();
            m_cond_push.notify_one();
        }
        else
        {
            if (m_queue.empty()) {
                return false;
            }
            item = m_queue.front();
            m_queue.pop_front();
        }
        return true;
    }

    void resize(size_t new_size)
    {
        m_size = new_size;
    }

private:
    std::atomic<size_t> m_size;
    std::deque<T> m_queue;
    std::mutex m_mutex;
    std::condition_variable m_cond_pop;
    std::condition_variable m_cond_push;
};

UG_NS_END

#endif // UG_THREAD_SAFE_QUEUE