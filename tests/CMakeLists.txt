cmake_minimum_required(VERSION 3.26)
project(DataStructureInC_Tests C CXX)

# 通过模块函数自动收集当前目录下的所有测试源文件
infra_collect_c_files(TEST_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}")

# 查找google test
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

# 添加测试文件
add_executable(DataStructureInC_Tests gtest_arraylist_test.cpp)

# 链接gtest
target_link_libraries(DataStructureInC_Tests GTest::GTest GTest::Main pthread)

# 启用测试
enable_testing()
add_test(NAME DataStructureInC_Tests COMMAND DataStructureInC_Tests)
