cmake_minimum_required(VERSION 3.26)
project(infra C)

# CMake 工具函数通过根 CMakeLists 的 CMAKE_MODULE_PATH + include(infra_utils) 全局加载

add_library(infra INTERFACE)
target_include_directories(infra
        INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include/
)