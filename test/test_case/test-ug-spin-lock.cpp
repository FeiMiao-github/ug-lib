#include "gtest/gtest.h"
#include <gtest/gtest.h>
#include <mutex>

#include "ug_spin_lock.hpp"

struct param_spin {
    size_t thread_num;
    size_t cnt;
};

class param_spin_lock_test: public ::testing::TestWithParam<param_spin> {
};

TEST_P(param_spin_lock_test, test)
{
    param_spin p = GetParam();
    ug::spin_lock lock;
    std::vector<std::thread> threads;
    int cnt = 0;

    for (size_t i = 0; i < p.thread_num; i++)
    {
        threads.emplace_back([&]() {
            for (size_t j = 0; j < p.cnt; j++)
            {
                std::lock_guard<ug::spin_lock> guard(lock);
                cnt++;
            }
        });
    }

    for (auto& t: threads)
    {
        t.join();
    }

    EXPECT_EQ(cnt, p.cnt * p.thread_num);
}

INSTANTIATE_TEST_SUITE_P(UG_SpinLock, param_spin_lock_test,
    ::testing::Values(
        param_spin{1, 100},
        param_spin{2, 100},
        param_spin{4, 100},
        param_spin{8, 100},
        param_spin{16, 100}
    ));