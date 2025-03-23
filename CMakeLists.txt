cmake_minimum_required(VERSION 3.28)

project(ug-lib)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 23)

find_package(Boost REQUIRED)
find_package(GTest REQUIRED)

include_directories(${CMAKE_SOURCE_DIR}/inc)

aux_source_directory(${CMAKE_SOURCE_DIR}/src SRC_FILES)
add_library(ug-lib STATIC ${SRC_FILES})

enable_testing()

add_executable(test-ug-lib ${CMAKE_SOURCE_DIR}/test/test-ug-lib.cpp)
target_link_libraries(test-ug-lib PRIVATE GTest::GTest GTest::Main ug-lib)
add_test(test-ug-lib test-ug-lib)