cmake_minimum_required(VERSION 3.26)
project(lineartable C)

# 使本子目录中的源文件也能直接包含头文件
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include/)

# 直接调用在 infra 模块中定义的函数，避免绝对路径 include
infra_collect_c_files(LINEARTABLE_C_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src")

# 编译 lineartable 的所有实现文件
add_library(lineartable STATIC ${LINEARTABLE_C_SOURCES})

# 导出公共头文件目录供上游使用
target_include_directories(lineartable
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include/
)

# lineartable 自身在编译时使用 infra，并将其传递给上游
target_link_libraries(lineartable PUBLIC infra)