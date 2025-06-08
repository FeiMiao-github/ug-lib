#ifndef UG_THREAD_POOL_HPP
#define UG_THREAD_POOL_HPP

#include <thread>
#include <vector>
#include <functional>

#include "ug_def.h"
#include "ug_thread_safe_queue.hpp"

UG_NS_BEGIN

class thread_pool {
public:
    using task_t = std::function<void()>;

    static constexpr size_t DEFAULT_THREAD_CNT = 4;
    static constexpr size_t DEFAULT_TASK_CNT = 1024;

    thread_pool();
    ~thread_pool();

    void start(size_t thread_cnt = DEFAULT_THREAD_CNT);
    void stop();

    void add_task(task_t task);
    void set_pool_task_cnt(size_t task_cnt = DEFAULT_TASK_CNT);

private:
    void process();

private:
    std::vector<std::thread> m_threads;
    ug::thread_safe_queue<task_t> m_queue;
    std::atomic<bool> m_run;
};

UG_NS_END

#endif // UG_THREAD_POOL