cmake_minimum_required(VERSION 3.26)
project(threadpool C)

# 包含本项目的头文件， 无需使用相对路径
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

infra_collect_c_files(THREADPOOL_C_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src")

add_library(threadpool STATIC ${THREADPOOL_C_SOURCES})

# 对外暴露头文件
target_include_directories(threadpool
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include/
)

target_link_libraries(threadpool PUBLIC infra)
