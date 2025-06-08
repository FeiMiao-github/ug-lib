#include <benchmark/benchmark.h>
#include <thread>

#include "ug_thread_safe_queue.hpp"

using queue_t = ug::thread_safe_queue<int>;
using queue_ptr_t = std::shared_ptr<queue_t>;

static void producer(queue_ptr_t q, size_t total_cnt) {
    for (size_t i = 0; i < total_cnt; i++) {
        q->push(i, true);
    }
}

static void consumer(queue_ptr_t q, size_t total_cnt) {
    for (size_t i = 0; i < total_cnt; i++) {
        int item;
        q->pop(item, true);
    }
}

static void BM_thread_safe_queue(benchmark::State& state) {
    size_t total_cnt = state.range(0);

    for (auto _ : state) {
        queue_ptr_t q = std::make_shared<queue_t>(1024);
        std::thread producer_thread(producer, q, total_cnt);
        std::thread consumer_thread(consumer, q, total_cnt);
        producer_thread.join();
        consumer_thread.join();
    }
}

BENCHMARK(BM_thread_safe_queue)->Arg(100);
BENCHMARK(BM_thread_safe_queue)->Arg(10000);
