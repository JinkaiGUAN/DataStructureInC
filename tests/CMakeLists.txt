cmake_minimum_required(VERSION 3.26)
project(DataStructureInC_Tests C CXX)

# 设置C++标准为C++17（GTest要求）
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 设置Debug模式，生成调试符号
set(CMAKE_BUILD_TYPE Debug)

# 直接列出测试源文件
set(TEST_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/gtest_arraylist_test.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/gtest_threadpool_test.cpp"
)

# 查找google test
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

# 添加测试可执行文件
add_executable(DataStructureInC_Tests ${TEST_SOURCES})

# 设置编译选项，生成调试信息
target_compile_options(DataStructureInC_Tests PRIVATE -g -O0)

# 链接项目库和gtest
target_link_libraries(DataStructureInC_Tests
    GTest::GTest
    GTest::Main
    pthread
    lineartable
    threadpool
    infra
)

# 启用测试
enable_testing()
add_test(NAME DataStructureInC_Tests COMMAND DataStructureInC_Tests)
