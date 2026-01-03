cmake_minimum_required(VERSION 3.26)
project(DataStructureInC C)

set(CMAKE_C_STANDARD 11)

add_executable(DataStructureInC main.c)

# 全局注册并加载 infra 的 CMake 工具模块，供所有子目录使用
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/infra/cmake")
include(infra_utils)

# 引入 lineartable 子项目并链接到主可执行程序
# 先引入 infra，供 lineartable 依赖
add_subdirectory(infra)
target_link_libraries(DataStructureInC PRIVATE infra)

# 再引入 lineartable（其内部依赖 infra）
add_subdirectory(lineartable)
target_link_libraries(DataStructureInC PRIVATE lineartable)

# 显式为可执行目标添加包含目录，确保 IDE 与编译器均能找到头文件
target_include_directories(DataStructureInC PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/lineartable/include
        ${CMAKE_CURRENT_SOURCE_DIR}/infra/include
)

# 启用并集成 CTest 测试
include(CTest)
enable_testing()
add_subdirectory(tests)
