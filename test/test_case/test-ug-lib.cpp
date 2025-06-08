#include <gtest/gtest.h>

// 主函数
int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    std::cout << "hello world !\n";

    // My_Spsc_Queue().ShowLog();

    return RUN_ALL_TESTS();
}