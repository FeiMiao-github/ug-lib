#ifndef UG_SPIN_LOCK_HPP
#define UG_SPIN_LOCK_HPP

#include <atomic>
#include <thread>

#include "ug_def.h"

UG_NS_BEGIN

class spin_lock {
    std::atomic_flag flag = ATOMIC_FLAG_INIT;
public:
    void lock() {
        size_t cnt = 0;
        while (flag.test_and_set(std::memory_order_acquire))
        {
            cnt++;
            if (cnt > 100)
            {
                std::this_thread::yield();
                cnt = 0;
            }
        }
    }


    void unlock() {
        flag.clear(std::memory_order_release);
    }
};

UG_NS_END

#endif // UG_SPIN_LOCK