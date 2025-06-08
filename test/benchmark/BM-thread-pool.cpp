#include <iostream>
#include <benchmark/benchmark.h>
#include "ug_thread_pool.hpp"

static void BM_thread_pool(benchmark::State& state)
{
    size_t total_cnt = state.range(0);
    for (auto _ : state) {
        ug::thread_pool pool;
        pool.start(4);
        for (size_t i = 0; i < total_cnt; ++i) {
            pool.add_task([=]() {
                static std::mutex mtx;
                std::lock_guard<std::mutex> lg(mtx);
                std::cout << "=====" << i << std::endl;
            });
        }
        pool.stop();
    }
}

BENCHMARK(BM_thread_pool)->Arg(1000);
