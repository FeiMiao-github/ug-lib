cmake_minimum_required(VERSION 3.28)

project(ug-lib)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 23)
# set(CMAKE_BUILD_TYPE Release)
set(CMAKE_BUILD_TYPE Debug)

find_package(Boost REQUIRED)
# find_package(GTest REQUIRED)

add_compile_options("-Wall")


set(UG_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

include_directories(${CMAKE_SOURCE_DIR}/inc)

aux_source_directory(${UG_SRC_DIR} SRC_FILES)
message(status "${SRC_FILES}") 
add_library(ug-lib STATIC ${SRC_FILES})

set(GOOGLETEST_PATH ${CMAKE_CURRENT_SOURCE_DIR}/googletest)
set(BENCHMARK_ENABLE_TESTING OFF)
add_subdirectory(benchmark)
# add_subdirectory(googletest)

enable_testing()

aux_source_directory(${CMAKE_SOURCE_DIR}/test/test_case TEST_FILES)
add_executable(test-ug-lib ${TEST_FILES})
target_include_directories(test-ug-lib PRIVATE ${UG_SRC_DIR})
target_link_libraries(test-ug-lib PRIVATE gtest ug-lib)
add_test(test-ug-lib test-ug-lib)

add_executable(test-ug-lib-thread ${TEST_FILES})
target_compile_options(test-ug-lib-thread PRIVATE "-fsanitize=thread")
target_link_options(test-ug-lib-thread PRIVATE "-fsanitize=thread")
target_include_directories(test-ug-lib-thread PRIVATE ${UG_SRC_DIR})
target_link_libraries(test-ug-lib-thread PRIVATE gtest ug-lib)
add_test(test-ug-lib-thread test-ug-lib-thread)

add_executable(test-ug-lib-memory ${TEST_FILES})
target_compile_options(test-ug-lib-memory PRIVATE "-fsanitize=address")
target_link_options(test-ug-lib-memory PRIVATE "-fsanitize=address")
target_include_directories(test-ug-lib-memory PRIVATE ${UG_SRC_DIR})
target_link_libraries(test-ug-lib-memory PRIVATE gtest ug-lib)
add_test(test-ug-lib-memory test-ug-lib-memory)

aux_source_directory(${CMAKE_SOURCE_DIR}/test/benchmark BENCHMARK_FILES)
add_executable(benchmark-ug-lib ${BENCHMARK_FILES})
target_include_directories(benchmark-ug-lib PRIVATE ${UG_SRC_DIR})
target_link_libraries(benchmark-ug-lib PRIVATE benchmark::benchmark benchmark::benchmark_main ug-lib)

aux_source_directory(${CMAKE_SOURCE_DIR}/tools TOOLS_FILES)
add_executable(ug-lib-tools ${TOOLS_FILES})