#include "ug_thread_pool.hpp"

using namespace ug;

thread_pool::thread_pool()
    : m_threads(),
      m_queue(DEFAULT_TASK_CNT),
      m_run(false)
{
}

thread_pool::~thread_pool() {}

void thread_pool::start(size_t thread_cnt)
{
    if (m_run == true)
    {
        return;
    }

    m_run = true;

    for (size_t i = 0; i < thread_cnt; ++i) {
        m_threads.emplace_back(std::thread(&thread_pool::process, this));
    }
}

void thread_pool::stop()
{
    if (!m_run)
    {
        return;
    }

    m_run = false;

    for (auto& thread : m_threads) {
        if (thread.joinable())
            thread.join();
    }
}

void thread_pool::add_task(task_t task)
{
    m_queue.push(task, true);
}

void thread_pool::set_pool_task_cnt(size_t task_cnt)
{
    m_queue.resize(task_cnt);
}

#include <iostream>

void thread_pool::process()
{
    static std::mutex mtx;
    while (m_run) {
        task_t task;
        m_queue.pop(task, true);
        task();
    }
}
