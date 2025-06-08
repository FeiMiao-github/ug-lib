#include <benchmark/benchmark.h>
#include <thread>
#include "ug_ringbuffer_ex.hpp"

template <size_t N>
using ring_buffer = ug::ring_buffer_ex<int, N>;

template <size_t N>
using ring_buffer_ptr = std::shared_ptr<ring_buffer<N>>;

template <size_t N>
static void producer(ring_buffer_ptr<N> rbuffer, size_t total_cnt, bool block) {
    for (size_t i = 0; i < total_cnt; i++) {
        while (!rbuffer->push(i, block)) {
        }
    }
}

template <size_t N>
static void consumer(ring_buffer_ptr<N> rbuffer, size_t total_cnt, bool block) {
    for (size_t i = 0; i < total_cnt; i++) {
        int item;
        while (!rbuffer->pop(item, block)) {
        }
    }
}

static void BM_ring_buffer_ex(benchmark::State& state) {
    size_t total_cnt = state.range(0);
    bool block = state.range(1);

    for (auto _ : state) {
        ring_buffer_ptr<1024> rbuffer = std::make_shared<ring_buffer<1024>>();
        std::thread producer_thread(producer<1024>, rbuffer, total_cnt, block);
        std::thread consumer_thread(consumer<1024>, rbuffer, total_cnt, block);
        producer_thread.join();
        consumer_thread.join();
    }
}

BENCHMARK(BM_ring_buffer_ex)->ArgPair(100, false);
BENCHMARK(BM_ring_buffer_ex)->ArgPair(10000, false);

BENCHMARK(BM_ring_buffer_ex)->ArgPair(100, true);
BENCHMARK(BM_ring_buffer_ex)->ArgPair(10000, true);