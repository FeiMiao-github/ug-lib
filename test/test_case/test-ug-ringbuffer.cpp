#include <gtest/gtest.h>
#include <memory>
#include <thread>

#include "ug_ringbuffer.hpp"

template<size_t N>
using ring_buffer = ug::ring_buffer<int, N>;

template <size_t N>
using ring_buffer_ptr = std::shared_ptr<ring_buffer<N>>;

template <size_t N>
static void producer(ring_buffer_ptr<N> rbuffer, size_t total_cnt) {
    for (size_t i = 0; i < total_cnt; i++) {
        while (!rbuffer->push(i)) {
            std::this_thread::yield();
        }
    }
}

template <size_t N>
static void consumer(ring_buffer_ptr<N> rbuffer, size_t total_cnt) {
    for (size_t i = 0; i < total_cnt; i++) {
        int item;
        while (!rbuffer->pop(item)) {
            std::this_thread::yield();
        }
        EXPECT_EQ(item, static_cast<int>(i));
    }
}

TEST(UG_RingBuffer, Test) {
    static constexpr int TOTAL_CNT = 100000;
    std::shared_ptr<ug::ring_buffer<int, TOTAL_CNT>> ring_buffer = std::make_shared<ug::ring_buffer<int, TOTAL_CNT>>();
    std::thread td1(producer<TOTAL_CNT>, ring_buffer, TOTAL_CNT);
    std::thread td2(consumer<TOTAL_CNT>, ring_buffer, TOTAL_CNT);
    td1.detach();
    td2.detach();
}