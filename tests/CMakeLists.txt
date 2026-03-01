cmake_minimum_required(VERSION 3.26)
project(DataStructureInC_Tests C CXX)

# 设置Debug模式，生成调试符号
set(CMAKE_BUILD_TYPE Debug)

# 收集src目录下的所有测试源文件
file(GLOB_RECURSE TEST_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

# 查找google test
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

# 添加测试文件，添加调试符号
add_executable(DataStructureInC_Tests ${TEST_SOURCES})

# 设置编译选项，生成调试信息
target_compile_options(DataStructureInC_Tests PRIVATE -g -O0)

# 链接项目库和gtest
target_link_libraries(DataStructureInC_Tests
    GTest::GTest
    GTest::Main
    pthread
    lineartable
    infra
)

# 启用测试
enable_testing()
add_test(NAME DataStructureInC_Tests COMMAND DataStructureInC_Tests)
