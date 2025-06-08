#include <benchmark/benchmark.h>
#include <thread>

#include "ug_ringbuffer.hpp"

template <size_t N>
using ring_buffer = ug::ring_buffer<int, N>;

template <size_t N>
using ring_buffer_ptr = std::shared_ptr<ug::ring_buffer<int, N>>;

template <size_t N>
static void producer(ring_buffer_ptr<N> rbuffer, size_t total_cnt) {
    for (size_t i = 0; i < total_cnt; i++) {
        while (!rbuffer->push(i)) {
        }
    }
}

template <size_t N>
static void consumer(ring_buffer_ptr<N> rbuffer, size_t total_cnt) {
    for (size_t i = 0; i < total_cnt; i++) {
        int item;
        bool ret = false;
        do {
            ret = rbuffer->pop(item);
        } while (!ret);
    }
}

static void BM_ring_buffer(benchmark::State& state) {
    size_t total_cnt = state.range(0);

    for (auto _ : state) {
        ring_buffer_ptr<1024> rbuffer = std::make_shared<ring_buffer<1024>>();
        std::thread producer_thread(producer<1024>, rbuffer, total_cnt);
        std::thread consumer_thread(consumer<1024>, rbuffer, total_cnt);
        producer_thread.join();
        consumer_thread.join();
    }
}

BENCHMARK(BM_ring_buffer)->Arg(100);
BENCHMARK(BM_ring_buffer)->Arg(10000);
